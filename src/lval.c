#include <arena.h>
#include <lval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"

lval_t* lval_op_add(lval_t* cells) {
    lval_t* curr = cells;
    double acc = 0;
    while (curr != NULL) {
        lval_t* curr_evaluated = eval(curr);
        if (curr_evaluated->type != LVAL_NUM) {
            fprintf(stderr, "Cannot add non numbers values.\n");
            // TODO: Return proper error.
            return cells;
        }
        acc += curr_evaluated->content.num;
        curr = curr->next;
    }

    lval_t* result = context_alloc(&tmp_arena, sizeof(lval_t));
    result->type = LVAL_NUM;
    result->content.num = acc;
    return result;
}

lval_t* lval_op_mult(lval_t* cells) {
    lval_t* curr = cells;
    if (curr == NULL) return 0;
    double acc = eval(curr)->content.num;
    curr = curr->next;
    while (curr != NULL) {
        lval_t* curr_evaluated = eval(curr);
        if (curr_evaluated->type != LVAL_NUM) {
            fprintf(stderr, "Cannot multiply non numbers values.\n");
            // TODO: Return proper error.
            return cells;
        }
        acc *= curr_evaluated->content.num;
        curr = curr->next;
    }

    lval_t* result = context_alloc(&tmp_arena, sizeof(lval_t));
    result->type = LVAL_NUM;
    result->content.num = acc;
    return result;
}

void print_lval_debug(const lval_t* v, int indent);

void print_indent(int indent) {
    for (int i = 0; i < indent; i++) putchar(' ');
}

const char* lval_type_name(enum LVAL_TYPE type) {
    switch (type) {
        case LVAL_ERR:
            return "ERR";
        case LVAL_NUM:
            return "NUM";
        case LVAL_SYM:
            return "SYM";
        case LVAL_SEXPR:
            return "SEXPR";
        default: {
            printf("type: %d\n", type);
        }
            return "UNKNOWN";
    }
}

void print_lval_debug(const lval_t* v, int indent) {
    if (!v) return;

    print_indent(indent);
    printf("type: %s\n", lval_type_name(v->type));

    print_indent(indent);
    printf("value: ");
    switch (v->type) {
        case LVAL_NUM:
            printf("%g\n", v->content.num);
            break;
        case LVAL_ERR:
            printf("%s\n", v->content.err);
            break;
        case LVAL_SYM:
            printf(SV_Fmt "\n", SV_Arg(v->content.sym));
            break;
        case LVAL_SEXPR:
            break;
    }

    if (v->type == LVAL_SEXPR) {
        print_indent(indent);
        printf("children:\n");
        const lval_t* child = v->content.cells;
        while (child) {
            print_lval_debug(child, indent + 2);
            child = child->next;
        }
    }
}

lval_t* allocate_lval(arena_t* arena, enum LVAL_TYPE type) {
    lval_t* lval = context_alloc(arena, sizeof(lval_t));
    lval->type = type;
    return lval;
}

lval_t* lval_clone(arena_t* arena, const lval_t* v, bool clone_next_too) {
    if (v == NULL) return NULL;

    // Allocate new lval
    lval_t* copy = allocate_lval(arena, v->type);
    if (clone_next_too) {
        copy->next = lval_clone(arena, v->next, true);  // Copy next in the linked list
    }

    switch (v->type) {
        case LVAL_NUM:
            copy->content.num = v->content.num;
            break;

        case LVAL_SYM:
            copy->content.sym.count = v->content.sym.count;

            // Allocate and copy the symbol data
            copy->content.sym.data = context_alloc(arena, v->content.sym.count);
            memcpy((void*)copy->content.sym.data, v->content.sym.data, v->content.sym.count);
            break;

        case LVAL_ERR:

            // Allocate and copy the error string
            if (v->content.err != NULL) {
                size_t len = strlen(v->content.err);
                copy->content.err = context_alloc(arena, len + 1);
                memcpy(copy->content.err, v->content.err, len + 1);
            } else {
                copy->content.err = NULL;
            }
            break;
        case LVAL_SEXPR:
            copy->content.cells = lval_clone(arena, v->content.cells, true);  // Recursively copy all cells
            break;
    }

    return copy;
}
