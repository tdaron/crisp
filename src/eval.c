#include <parser.h>
#include <stdio.h>

#include "lval.h"
#include "context.h"
#include "sv.h"

lval_t* eval(lval_t* val) {
    switch (val->type) {
        case LVAL_NUM:
            return val;

        case LVAL_SYM: {
            lval_t* info;
            lval_t* result = lookup_symbol(current_context, val->content.sym, &info);
            if (result != NULL) return result;
            fprintf(stderr, "Symbol " SV_Fmt " not found\n",
                    SV_Arg(val->content.sym));
            return val;
        }
        case LVAL_SEXPR:
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr("+"))) {
                return lval_op_add(val->content.cells->next);
                break;
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr("*"))) {
                return lval_op_mult(val->content.cells->next);
                break;
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr("="))) {
                lval_t* lhs = val->content.cells->next;
                lval_t* rhs = lhs->next;
                lval_t* result = allocate_lval(&current_context->arena, LVAL_NUM);
                result->content.num = eval(lhs)->content.num == eval(rhs)->content.num;
                return result;
                break;
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr("<"))) {
                lval_t* lhs = val->content.cells->next;
                lval_t* rhs = lhs->next;
                lval_t* result = allocate_lval(&current_context->arena, LVAL_NUM);
                result->content.num = eval(lhs)->content.num < eval(rhs)->content.num;
                return result;
                break;
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr(">"))) {
                lval_t* lhs = val->content.cells->next;
                lval_t* rhs = lhs->next;
                lval_t* result = allocate_lval(&current_context->arena, LVAL_NUM);
                result->content.num = eval(lhs)->content.num > eval(rhs)->content.num;
                return result;
                break;
            }
            if (sv_eq(val->content.cells->content.sym, sv_from_cstr("if"))) {
                lval_t* cond = val->content.cells->next;
                lval_t* is_true = cond->next;
                lval_t* not_true = is_true->next;

                lval_t* cond_eval = eval(cond);

                if (cond_eval->type == LVAL_NUM && cond_eval->content.num) {
                    return eval(is_true);
                }
                return eval(not_true);
                break;
            }
            if (sv_eq(val->content.cells->content.sym,
                      sv_from_cstr("define"))) {
                // TODO: Handle errors of number of args.
                // TODO: Dont allow defining functions with numbers in their definition
                lval_t* name = val->content.cells->next;
                lval_t* value = name->next;

                // TODO: Handle errors of (define NUM something)
                // that means nothing.
                if (name->type == LVAL_SYM) {
                    value = eval(value);
                }
                context_define_symbol(name, value);
                return NULL;
            }

            // Check if there is a function with the name of the first cell (function_name arg1 arg2)
            lval_t* info;
            lval_t* result = lookup_symbol(current_context, val->content.cells->content.sym, &info);

            // we just found a corresponding function
            if (result != NULL && info->type == LVAL_SEXPR) {
                push_context();
                match_args_params(info, val);
                lval_t* r = eval(result);
                r = lval_clone(&current_context->parent->arena, r, false);
                pop_context();
                return r;
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
