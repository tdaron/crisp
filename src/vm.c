#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vm.h>

void* _consume_bytecode(VM* vm, bytecode_t* code, size_t size) {
    void* addr = &code->items[vm->ip];
    vm->ip += size;
    return addr;
}

#define consume_bytecode(vm, code, x) \
    *(x*)_consume_bytecode(vm, code, sizeof(x))

void push_value(VM* vm, StackValue val) { vm->stack[vm->sp++] = val; }

StackValue pop_value(VM* vm) { return vm->stack[--vm->sp]; }

#define apply_cond(vm, v1, v2, cond) push_value(vm, BOOL_VAL(v1 cond v2));
const char* opcode_to_string(Opcode op) {
    switch (op) {
        case OP_PUSH_CONST:
            return "OP_PUSH_CONST";
        case OP_ADD:
            return "OP_ADD";
        case OP_SUB:
            return "OP_SUB";
        case OP_MULT:
            return "OP_MULT";
        case OP_DIV:
            return "OP_DIV";
        case OP_EQ:
            return "OP_EQ";
        case OP_GEQ:
            return "OP_GEQ";
        case OP_LEQ:
            return "OP_LEQ";
        case OP_GE:
            return "OP_GE";
        case OP_LE:
            return "OP_LE";
        case OP_JUMPF:
            return "OP_JUMPF";
        case OP_JUMP:
            return "OP_JUMP";
        case OP_POP:
            return "OP_POP";
        case OP_CALL:
            return "OP_CALL";
        case OP_LOAD_SYMBOL:
            return "OP_LOAD_SYMBOL";
        case OP_RET:
            return "OP_RET";
        case OP_FUNCDEF:
            return "OP_FUNCDEF";

        default:
            return "UNKNOWN_OPCODE";
    }
}

void set_arg(VM* vm, StackValue val, Hash name) {
    size_t index = vm->call_stack[vm->csp].asp++;
    vm->call_stack[vm->csp].args_stack[index] = val;
    hashmap_add(&vm->call_stack[vm->csp].symbols, name,
                &vm->call_stack[vm->csp].args_stack[index]);
}

void set_symbol(VM* vm, StackValue val, Hash name, size_t csp) {
    StackValue* old = hashmap_lookup(&vm->call_stack[csp].symbols, name);
    if (old != NULL) {
        *old = val;
        return;
    }
    size_t index = vm->call_stack[csp].ssp++;
    vm->call_stack[csp].symbols_stack[index] = val;
    hashmap_add(&vm->call_stack[csp].symbols, name,
                &vm->call_stack[csp].symbols_stack[index]);
}


void add_ffi_func(VM* vm, SV functionName, void(*fptr)(VM* vm)) {
    Hash hash = hash_sv(functionName);
    hashmap_add(&vm->ffi, hash, fptr);
}

void execute(VM* vm, bytecode_t* code, size_t start_ip) {
    // Reset stack pointer and instruction pointer
    // vm->sp = 0;
    vm->ip = start_ip;

    while (vm->ip < code->size) {
        Opcode opcode = code->items[vm->ip];
        vm->ip++;
        switch (opcode) {
            case OP_PUSH_CONST: {
                double v = consume_bytecode(vm, code, double);
                push_value(vm, DOUBLE_VAL(v));
                break;
            }
            case OP_ADD: {
                double num = consume_bytecode(vm, code, double);
                double acc = 0;
                for (size_t i = 0; i < num; i++) {
                    StackValue val = pop_value(vm);
                    acc += val.as.d;
                }
                push_value(vm, DOUBLE_VAL(acc));
                break;
            }
            case OP_MULT: {
                double num = consume_bytecode(vm, code, double);
                double acc = pop_value(vm).as.d;
                for (size_t i = 0; i < num - 1; i++) {
                    StackValue val = pop_value(vm);
                    acc *= val.as.d;
                }
                push_value(vm, DOUBLE_VAL(acc));
                break;
            }
            case OP_SUB: {
                double to_substract = pop_value(vm).as.d;
                push_value(vm, DOUBLE_VAL(pop_value(vm).as.d - to_substract));
                break;
            }
            case OP_DIV: {
                double denom = pop_value(vm).as.d;
                push_value(vm, DOUBLE_VAL(pop_value(vm).as.d / denom));
                break;
            }
            case OP_EQ: {
                double right = pop_value(vm).as.d;
                double left = pop_value(vm).as.d;
                apply_cond(vm, left, right, ==);
                break;
            }
            case OP_LEQ: {
                double right = pop_value(vm).as.d;
                double left = pop_value(vm).as.d;
                apply_cond(vm, left, right, <=);
                break;
            }
            case OP_GEQ: {
                double right = pop_value(vm).as.d;
                double left = pop_value(vm).as.d;
                apply_cond(vm, left, right, >=);
                break;
            }
            case OP_GE: {
                double right = pop_value(vm).as.d;
                double left = pop_value(vm).as.d;
                apply_cond(vm, left, right, >);
                break;
            }
            case OP_LE: {
                double right = pop_value(vm).as.d;
                double left = pop_value(vm).as.d;
                apply_cond(vm, left, right, <);
                break;
            }
            case OP_JUMPF: {
                bool cond = pop_value(vm).as.b;
                double distance = consume_bytecode(vm, code, double);
                if (!cond) {
                    vm->ip += (size_t)distance;
                }
                break;
            }
            case OP_JUMP: {
                double distance = consume_bytecode(vm, code, double);
                vm->ip += distance;
                break;
            }
            case OP_POP: {
                printf("pop\n");
                pop_value(vm);
                break;
            }
            case OP_LOAD_SYMBOL: {
                Hash hash = consume_bytecode(vm, code, Hash);
                StackValue* val = NULL;
                for (int i = vm->csp; i >= 0; i--) {
                    val = hashmap_lookup(&vm->call_stack[i].symbols, hash);
                    if (val != NULL) break;
                }
                if (val == NULL) {
                    printf("Unknown symbol...\n");
                    break;
                }
                StackValue to_push = {.type = val->type, .as = val->as};
                push_value(vm, to_push);
                break;
            }
            case OP_FUNCDEF: {
                Hash name_hash = consume_bytecode(vm, code, Hash);
                uint8_t args_number = consume_bytecode(vm, code, uint8_t);

                Hash* args = (Hash*)(code->items + vm->ip);
                vm->ip += sizeof(Hash) * args_number;

                double body_length = consume_bytecode(vm, code, double);
                size_t function_position = vm->ip;
                vm->ip += body_length;

                Function* f = malloc(sizeof(Function));
                f->position = function_position;
                printf(
                    "Registered function %08X at position %ld with %d args\n",
                    name_hash, function_position, args_number);
                // TODO: support args
                f->args_number = args_number;
                f->name = name_hash;
                f->next = vm->functions;
                f->args = args;
                vm->functions = f;

                break;
            }
            case OP_CALL: {
                Hash f_hash = consume_bytecode(vm, code, Hash);
                void* v = hashmap_lookup(&vm->ffi, f_hash);
                if (v != NULL) {
                    void(*fptr)(VM* vm) = v;
                    fptr(vm);
                    break;
                }
                Function* function = vm->functions;
                while (function != NULL) {
                    if (function->name == f_hash) {
                        break;
                    }
                    function = function->next;
                }
                if (function == NULL) {
                    printf("Function  %08X does not exist\n", f_hash);
                } else {
                    // TCO
                    if (vm->ip < code->size && code->items[vm->ip] == OP_RET) {
                        // Tail Call Optimization: reuse current CallFrame
                        vm->ip++;  // Skip the OP_RET
                        vm->call_stack[vm->csp].ssp = 0;
                        for (size_t i = 0; i < function->args_number; i++) {
                            // Re-setting args without re-computing hashes/...
                            StackValue arg = pop_value(vm);
                            size_t index = vm->call_stack[vm->csp].asp;
                            vm->call_stack[vm->csp]
                                .args_stack[index - function->args_number +
                                               i] = arg;
                        }
                        vm->ip = function->position;
                        break;  // Jump directly without call stack push
                    }
                    vm->csp++;
                    vm->call_stack[vm->csp].return_addr = vm->ip;
                    for (size_t i = 0; i < function->args_number; i++) {
                        StackValue arg = pop_value(vm);
                        Hash name =
                            function->args[function->args_number - i - 1];
                        set_arg(vm, arg, name);
                        // vm->call_stack[vm->csp].symbols_stack[i] = arg;
                        // hashmap_add(&vm->call_stack[vm->csp].symbols, name,
                        //             &vm->call_stack[vm->csp].symbols_stack[i]);
                    }
                    vm->ip = function->position;
                }
                break;
            }
            case OP_RET: {
                vm->call_stack[vm->csp].asp = 0;
                vm->call_stack[vm->csp].ssp = 0;
                vm->ip = vm->call_stack[vm->csp--].return_addr;
                break;
            }
            case OP_SET_SYMBOL: {
                Hash hash = consume_bytecode(vm, code, Hash);
                StackValue val = pop_value(vm);
                // TODO: fix this. can currently be overwritten by TCO
                StackValue* curr = NULL;
                size_t csp = vm->csp;
                for (int i = vm->csp; i >= 0; i--) {
                    curr = hashmap_lookup(&vm->call_stack[i].symbols, hash);
                    if (curr != NULL) csp = i;
                }
                set_symbol(vm, val, hash, csp);
                break;
            }
            default:
                printf("Opcode %d not supported\n", opcode);
                break;
        }
    }
    if (vm->sp > 0) {
        printf("Stack: [");
        for (int i = 0; i < vm->sp; i++) {
            StackValue* val = &vm->stack[i];
            if (val->type == VAL_DOUBLE) {
                printf("%f ", vm->stack[i].as.d);
            }
            if (val->type == VAL_BOOL) {
                printf("B%d ", vm->stack[i].as.b);
            }
        }
        printf("]\n");
    }
    vm->sp = 0;
}

