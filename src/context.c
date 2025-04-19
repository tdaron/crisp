#include <context.h>
#include <stdio.h>
#include <string.h>

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

lval_t* lookup_symbol(context_t* context, SV name) {
    symbol_t* curr = context->symbols;
    while (curr != NULL) {
        if (sv_eq(curr->sym->content.sym, name)) {
            return curr->value;
        }
        curr = curr->next;
    }
    if (context->parent == NULL) return NULL;
    // lookup recursively into parent contexts
    return lookup_symbol(context->parent, name);
}
