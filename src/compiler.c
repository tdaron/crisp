#include <compiler.h>
#include <stdint.h>
#include <stdio.h>

// Forward declarations
double count_and_compile_cells(lval_t* cells, bytecode_t* bytecode);
void compile_add(lval_t* args, bytecode_t* bytecode);
void compile_mult(lval_t* args, bytecode_t* bytecode);
void compile_simple(lval_t* args, bytecode_t* bytecode, Opcode opcode);

bytecode_t* compile(lval_t* val, bytecode_t* bytecode) {
    if (bytecode == NULL) bytecode = calloc(1, sizeof(bytecode_t));

    switch (val->type) {
        case LVAL_ERR:
            break;

        case LVAL_NUM:
            da_append(bytecode, OP_PUSH_CONST);
            printf("\t%f\n", val->content.num);
            da_append_block(bytecode, &val->content.num, sizeof(double));
            break;

        case LVAL_SYM:
            break;

        case LVAL_SEXPR: {
            lval_t* sym_cell = val->content.cells;
            lval_t* args = sym_cell->next;

            if (sv_eq(sym_cell->content.sym, sv_from_parts("+", 1))) {
                compile_add(args, bytecode);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("*", 1))) {
                compile_mult(args, bytecode);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("-", 1))) {
                compile_simple(args, bytecode, OP_SUB);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("/", 1))) {
                compile_simple(args, bytecode, OP_DIV);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("=", 1))) {
                compile_simple(args, bytecode, OP_EQ);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("<=", 2))) {
                compile_simple(args, bytecode, OP_LEQ);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("<", 1))) {
                compile_simple(args, bytecode, OP_LE);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts(">=", 2))) {
                compile_simple(args, bytecode, OP_GEQ);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts(">", 1))) {
                compile_simple(args, bytecode, OP_GE);
            } else if (sv_eq(sym_cell->content.sym, sv_from_parts("if", 2))) {
                // evaluate the if condition
                compile(args, bytecode);

                // 1. Compute the size of the true branch
                // If the condition is false, jump over it
                da_append(bytecode, OP_JUMPF);

                // Placeholder for jump length. Will be filled later.
                double true_length = 0;
                // Position of the jump length
                size_t jump_offset_pos = bytecode->size;
                da_append_block(bytecode, &true_length, sizeof(double));

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
                da_append(bytecode, OP_JUMP);
                true_length = 0;
                jump_offset_pos = bytecode->size;
                da_append_block(bytecode, &true_length, sizeof(double));
                // compile the false branch
                compile(args->next->next, bytecode);
                true_length = bytecode->size - jump_offset_pos - sizeof(double);
                *(double*)(bytecode->items + jump_offset_pos) = true_length;
            } else if (sv_eq(sym_cell->content.sym, sv_from_cstr("begin"))) {
                count_and_compile_cells(args, bytecode);
            } else if (sv_eq(sym_cell->content.sym, sv_from_cstr("pop"))) {
                da_append(bytecode, OP_POP);
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
    da_append(bytecode, OP_ADD);
    da_append_block(bytecode, &count, sizeof(double));
}

void compile_mult(lval_t* args, bytecode_t* bytecode) {
    double count = count_and_compile_cells(args, bytecode);
    da_append(bytecode, OP_MULT);
    da_append_block(bytecode, &count, sizeof(double));
}

void compile_simple(lval_t* args, bytecode_t* bytecode, Opcode opcode) {
    // TODO: assert number of args.
    count_and_compile_cells(args, bytecode);
    da_append(bytecode, opcode);
}
