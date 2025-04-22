#include <context.h>
#include <stdio.h>
#include <string.h>
#include <eval.h>
#include "arena.h"
#include "lval.h"

void context_define_symbol(lval_t* tmp_symbol, lval_t* tmp_val) {
    symbol_t* symbol = context_alloc(&current_context->arena, sizeof(symbol_t));
    lval_t* symbol_clone =
        lval_clone(&current_context->arena, tmp_symbol, false);
    lval_t* val = lval_clone(&current_context->arena, tmp_val, false);
    symbol->sym = symbol_clone;
    symbol->value = val;
    symbol->next = current_context->symbols;
    current_context->symbols = symbol;
}

void push_context() {
    context_t* new_context = malloc(sizeof(context_t));
    memset(new_context, 0, sizeof(context_t));
    new_context->parent = current_context;
    current_context = new_context;
}

void pop_context() {
    if (current_context == NULL) {
        return;
    }

    context_t* o = current_context;
    current_context = current_context->parent;
    context_free(&o->arena);  // free all objects inside the context arena.
    free(o);
}

lval_t* lookup_symbol(context_t* context, SV name, lval_t** symbol_info) {
    symbol_t* curr = context->symbols;
    while (curr != NULL) {
        if (curr->sym->type == LVAL_SYM) {
            if (sv_eq(curr->sym->content.sym, name)) {
                *symbol_info = curr->sym;
                return curr->value;
            }
            
        }
        if (curr->sym->type == LVAL_SEXPR) {
            if (sv_eq(curr->sym->content.cells->content.sym, name)) {
                
                *symbol_info = curr->sym;
                return curr->value;
            }
            
        }
        curr = curr->next;
    }
    if (context->parent == NULL) return NULL;
    // lookup recursively into parent contexts
    return lookup_symbol(context->parent, name, symbol_info);
}

void match_args_params(lval_t* function, lval_t* call) {
    lval_t* curr_fct = function->content.cells->next;
    lval_t* curr_call = call->content.cells->next;
    while (curr_fct != NULL && curr_call != NULL) {
        
        if (curr_fct->type != LVAL_SEXPR) curr_call = eval(curr_call);
        context_define_symbol(curr_fct, curr_call);
        
        curr_fct = curr_fct->next;
        curr_call = curr_call->next;
    }
}
