#include <compiler.h>
#include <stdio.h>

double count_and_compile_cells(lval_t* cells, bytecode_t* bytecode) {
    // Adding each term one by one
    lval_t* curr = cells;
    double count = 0;
    while (curr != NULL) {
        count++;
        compile(curr, bytecode);
        curr = curr->next;
    }
    return count;
}

bytecode_t* compile(lval_t* val, bytecode_t* bytecode) {
    if (bytecode == NULL) bytecode = calloc(1, sizeof(bytecode_t));

    switch (val->type) {
        case LVAL_ERR:
            break;
        case LVAL_NUM:
            da_append(bytecode, OP_PUSH_CONST);
            da_append_block(bytecode, &val->content.num, sizeof(double));
            break;
        case LVAL_SYM:
            break;
        case LVAL_SEXPR:
            // if the operation is a *
            if (sv_eq(val->content.cells->content.sym, sv_from_parts("*", 1))) {
                double count =
                    count_and_compile_cells(val->content.cells->next, bytecode);
                da_append(bytecode, OP_MULT);
                da_append_block(bytecode, &count, sizeof(double));
            }
            // if the operation is a +
            if (sv_eq(val->content.cells->content.sym, sv_from_parts("+", 1))) {
                double count =
                    count_and_compile_cells(val->content.cells->next, bytecode);
                da_append(bytecode, OP_ADD);
                da_append_block(bytecode, &count, sizeof(double));
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_parts("-", 1))) {
                // TODO: assert only two cells.
                count_and_compile_cells(val->content.cells->next, bytecode);
                da_append(bytecode, OP_SUB);
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_parts("/", 1))) {
                // TODO: assert only two cells.
                count_and_compile_cells(val->content.cells->next, bytecode);
                da_append(bytecode, OP_DIV);
            }
            break;
    }

    return bytecode;
}
