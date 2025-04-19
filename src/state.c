#include <state.h>
#include <string.h>

#include "arena.h"
#include "lval.h"

void state_define_symbol(lval_t* symbol_val, lval_t* tmp_val) {
    symbol_t* symbol = context_alloc(&state_arena, sizeof(symbol_t));
    symbol->name = symbol_val->content.sym;

    lval_t* val = allocate_lval(&state_arena, tmp_val->type);
    // TODO: Deep copy of tmp_val - right now cells are not preserved
    memcpy(val, tmp_val, sizeof(lval_t));
    val->next = NULL; // until deep copy

    symbol->value = val;
    symbol->next = state.symbols;
    state.symbols = symbol;
}
