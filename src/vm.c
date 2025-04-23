#include <stdint.h>
#include <stdio.h>
#include <vm.h>

void* _consume_bytecode(VM* vm, bytecode_t* code, size_t size) {
    void* addr = &code->items[vm->ip];
    vm->ip += size;
    return addr;
}

#define consume_bytecode(vm, code, x) \
    *(x*)_consume_bytecode(vm, code, sizeof(x))

void push_value(VM* vm, Value val) { vm->stack[vm->sp++] = val; }

Value pop_value(VM* vm) { return vm->stack[--vm->sp]; }

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
        default:
            return "UNKNOWN_OPCODE";
    }
}

void execute(VM* vm, bytecode_t* code) {
    // Reset stack pointer and instruction pointer
    // vm->sp = 0;
    vm->ip = 0;

    while (vm->ip < code->size) {
        Opcode opcode = code->items[vm->ip];
        // printf("%s\n", opcode_to_string(opcode));
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
                    Value val = pop_value(vm);
                    acc += val.as.d;
                }
                push_value(vm, DOUBLE_VAL(acc));
                break;
            }
            case OP_MULT: {
                double num = consume_bytecode(vm, code, double);
                double acc = pop_value(vm).as.d;
                for (size_t i = 0; i < num - 1; i++) {
                    Value val = pop_value(vm);
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
            default:
                printf("Opcode %d not supported\n", opcode);
                break;
        }
        printf("Stack: [");
        for (int i = 0; i < vm->sp; i++) {
            Value* val = &vm->stack[i];
            if (val->type == VAL_DOUBLE) {
                printf("%f ", vm->stack[i].as.d);
            }
            if (val->type == VAL_BOOL) {
                printf("B%d ", vm->stack[i].as.b);
            }
        }
        printf("]\n");
    }
    pop_value(vm);
}
