#include <compiler.h>
#include <stdint.h>
#include <stdio.h>

// Forward declarations
double count_and_compile_cells(lval_t* cells, bytecode_t* bytecode);
void compile_add(lval_t* args, bytecode_t* bytecode);
void compile_mult(lval_t* args, bytecode_t* bytecode);
void compile_simple(lval_t* args, bytecode_t* bytecode, Opcode opcode);
#define SYM_IS(str) \
    sv_eq(sym_cell->content.sym, sv_from_parts(str, sizeof(str) - 1))

bytecode_t* compile(lval_t* val, bytecode_t* bytecode) {
    if (bytecode == NULL) bytecode = calloc(1, sizeof(bytecode_t));

    switch (val->type) {
        case LVAL_ERR:
            break;

        case LVAL_NUM:
            bytecode_append(bytecode, OP_PUSH_CONST);
            // printf("\t%f\n", val->content.num);
            bytecode_append_block(bytecode, &val->content.num, sizeof(double));
            break;

        case LVAL_SYM:
            bytecode_append(bytecode, OP_LOAD_SYMBOL);
            double size = (double)val->content.sym.count;
            bytecode_append_block(bytecode, &size, sizeof(double));
            bytecode_append_block(bytecode, val->content.sym.data,
                                  sizeof(char) * size);

            break;

        case LVAL_SEXPR: {
            lval_t* sym_cell = val->content.cells;
            lval_t* args = sym_cell->next;

            if (SYM_IS("+")) {
                compile_add(args, bytecode);
            } else if (SYM_IS("*")) {
                compile_mult(args, bytecode);
            } else if (SYM_IS("-")) {
                compile_simple(args, bytecode, OP_SUB);
            } else if (SYM_IS("/")) {
                compile_simple(args, bytecode, OP_DIV);
            } else if (SYM_IS("=")) {
                compile_simple(args, bytecode, OP_EQ);
            } else if (SYM_IS("<=")) {
                compile_simple(args, bytecode, OP_LEQ);
            } else if (SYM_IS("<")) {
                compile_simple(args, bytecode, OP_LE);
            } else if (SYM_IS(">=")) {
                compile_simple(args, bytecode, OP_GEQ);
            } else if (SYM_IS(">")) {
                compile_simple(args, bytecode, OP_GE);
            } else if (SYM_IS("define")) {
                bytecode_append(bytecode, OP_FUNCDEF);
                lval_t* signature = args->content.cells;
                SV name = signature->content.sym;
                double size = (double)name.count;
                bytecode_append_block(bytecode, &size, sizeof(double));
                bytecode_append_block(bytecode, name.data,
                                      sizeof(char) * name.count);

                size_t size_pos = bytecode->size;
                double body_size = 0;
                bytecode_append_block(bytecode, &body_size, sizeof(double));
                compile(args->next, bytecode);
                bytecode_append(bytecode, OP_RET);
                body_size = bytecode->size - size_pos - sizeof(double);
                *((double*)(bytecode->items +size_pos)) = body_size;

            } else if (SYM_IS("if")) {
                // evaluate the if condition
                compile(args, bytecode);

                // 1. Compute the size of the true branch
                // If the condition is false, jump over it
                bytecode_append(bytecode, OP_JUMPF);

                // Placeholder for jump length. Will be filled later.
                double true_length = 0;
                // Position of the jump length
                size_t jump_offset_pos = bytecode->size;
                bytecode_append_block(bytecode, &true_length, sizeof(double));

                // Compile the true branch
                compile(args->next, bytecode);

                // Computing the size to jump
                // The -sizeof(double) is because we jump after the JUMP
                // instruction that has one double operand.
                true_length = bytecode->size - jump_offset_pos - sizeof(double);
                true_length +=
                    sizeof(uint8_t) +
                    sizeof(double);  // Don't forget to also jump over the JUMP
                                     // at the end of the true part

                // Replacing placeholder with correct jump length
                *(double*)(bytecode->items + jump_offset_pos) = true_length;

                // If the condition is true, after the true case we need to jump
                // over the false case. Same maths as before.
                bytecode_append(bytecode, OP_JUMP);
                true_length = 0;
                jump_offset_pos = bytecode->size;
                bytecode_append_block(bytecode, &true_length, sizeof(double));
                // compile the false branch
                compile(args->next->next, bytecode);
                true_length = bytecode->size - jump_offset_pos - sizeof(double);
                *(double*)(bytecode->items + jump_offset_pos) = true_length;
            } else if (SYM_IS("begin")) {
                count_and_compile_cells(args, bytecode);
            } else if (SYM_IS("pop")) {
                bytecode_append(bytecode, OP_POP);
            } else {
                count_and_compile_cells(args, bytecode);
                bytecode_append(bytecode, OP_CALL);
                double size = (double)sym_cell->content.sym.count;
                bytecode_append_block(bytecode, &size, sizeof(double));
                bytecode_append_block(bytecode, sym_cell->content.sym.data,
                                      sizeof(char) * size);
                
            }
            break;
        }
    }

    return bytecode;
}

double count_and_compile_cells(lval_t* cells, bytecode_t* bytecode) {
    lval_t* curr = cells;
    double count = 0;
    while (curr != NULL) {
        count++;
        compile(curr, bytecode);
        curr = curr->next;
    }
    return count;
}

void compile_add(lval_t* args, bytecode_t* bytecode) {
    double count = count_and_compile_cells(args, bytecode);
    bytecode_append(bytecode, OP_ADD);
    bytecode_append_block(bytecode, &count, sizeof(double));
}

void compile_mult(lval_t* args, bytecode_t* bytecode) {
    double count = count_and_compile_cells(args, bytecode);
    bytecode_append(bytecode, OP_MULT);
    bytecode_append_block(bytecode, &count, sizeof(double));
}

void compile_simple(lval_t* args, bytecode_t* bytecode, Opcode opcode) {
    // TODO: assert number of args.
    count_and_compile_cells(args, bytecode);
    bytecode_append(bytecode, opcode);
}
