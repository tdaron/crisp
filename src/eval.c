#include <parser.h>
#include <stdio.h>

#include "lval.h"
#include "state.h"
#include "sv.h"

lval_t* eval(lval_t* val) {
    switch (val->type) {
        case LVAL_NUM:
            return val;

        case LVAL_SYM: {
            symbol_t* curr = state.symbols;
            while (curr != NULL) {
                if (sv_eq(curr->name, val->content.sym)) {
                    return curr->value;
                }
                curr = curr->next;
            }

            fprintf(stderr, "Symbol " SV_Fmt " not found\n",
                    SV_Arg(val->content.sym));
            return val;
        }
        case LVAL_SEXPR:
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr("+"))) {
                return lval_op_add(val->content.cells->next);
                break;
            }
            if (sv_eq(val->content.cells->content.sym,
                      sv_from_cstr("define"))) {
                // TODO: Handle errors of number of args.
                lval_t* name = val->content.cells->next;
                lval_t* value = name->next;

                // TODO: Handle errors of (define NUM something)
                // that means nothing.
                if (name->type == LVAL_SYM) {
                    value = eval(value);
                }
                state_define_symbol(name, value);
                return NULL;
            }
            break;

        default:
            // Optionally handle unexpected types
            fprintf(stderr, "Unknown lval type: %d\n", val->type);
            return val;
    }
    fprintf(stderr, "Couldnt execute this. Returning value as is.\n");
    return val;
}
