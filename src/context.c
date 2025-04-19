#include <context.h>

#include "arena.h"
#include "lval.h"

void context_define_symbol(lval_t* tmp_symbol, lval_t* tmp_val) {
    symbol_t*  symbol = context_alloc(&current_context.arena, sizeof(symbol_t));
    lval_t* symbol_clone = lval_clone(&current_context.arena, tmp_symbol, false);
    lval_t* val = lval_clone(&current_context.arena, tmp_val, false);
    symbol->sym = symbol_clone;
    symbol->value = val;
    symbol->next = current_context.symbols;
    current_context.symbols = symbol;
}

void push_context() {
    context_t* new_context = context_alloc(&current_context.arena, sizeof(context_t));
}
