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

void execute(VM* vm, bytecode_t* code) {
    // Reset stack pointer and instruction pointer
    vm->sp = 0;
    vm->ip = 0;

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
                    Value val = pop_value(vm);
                    acc += val.as.d;
                }
                push_value(vm, DOUBLE_VAL(acc));
                break;
            }
            case OP_MULT: {
                double num = consume_bytecode(vm, code, double);
                double acc = pop_value(vm).as.d;
                for (size_t i = 0; i < num-1; i++) {
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
                push_value(vm, DOUBLE_VAL(pop_value(vm).as.d/denom));
                break;
            }
            default:
                printf("Opcode %d not supported\n", opcode);
                break;
        }
    }

    Value val = pop_value(vm);
    printf("%f\n", val.as.d);
}
