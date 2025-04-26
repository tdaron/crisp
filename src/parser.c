#include <arena.h>
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "sv.h"

bool is_whitespace(char v) { return v == ' ' || v == '\n'; }

bool valid_sym(char v) { return !(is_whitespace(v)) && v != ')'; }
lval_t* parse_atom(SV* input);

lval_t* parse_sexpr(SV* input) {
    consume(input);  // first (
    sv_chop_left_while(input, is_whitespace);

    lval_t* sexpr = allocate_lval(&parsing_arena, LVAL_SEXPR);

    while (input->count > 0 && *input->data != ')') {
        lval_t* child = parse(input);
        lval_t* last = sexpr->content.cells;
        if (last == NULL) {
            sexpr->content.cells = child;
        } else {
            while (last != NULL && last->next != NULL) {
                last = last->next;
            }
            last->next = child;
        }
        sv_chop_left_while(input, is_whitespace);
    }

    if (input->count == 0) {
        printf(
            "Unexpectd EOF at the end of the S-Expr. Did you forgot a ) ?\n");
    }

    consume(input);  // last )
    return sexpr;
}

lval_t* parse_atom(SV* input) {
    SV atom_content = sv_chop_left_while(input, valid_sym);
    char buf[16];

    // do not support numbers of more than 16 digits long.
    if (atom_content.count >= 16) {
        goto sym;
    }

    // TODO: Maybe add a \0 temporarly ito atom_content.data to emulate a
    // null terminated string instead of making a copy in memory. IDK if it
    // will look better.
    memcpy(buf, atom_content.data, atom_content.count);
    buf[atom_content.count] = '\0';
    char* end;
    double result = strtod(buf, &end);
    if (end != NULL && (size_t)(end - buf) == atom_content.count) {
        lval_t* val = allocate_lval(&parsing_arena, LVAL_NUM);
        val->content.num = result;
        return val;
    }

sym: {
    lval_t* val = allocate_lval(&parsing_arena, LVAL_SYM);
    val->content.sym = atom_content;
    val->next = NULL;
    return val;
}
}

lval_t* parse(SV* input) {
    sv_chop_left_while(input, is_whitespace);

    if (*input->data == '(') {
        return parse_sexpr(input);
    }

    return parse_atom(input);
}
