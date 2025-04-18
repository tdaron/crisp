#include "arena.h"
#include "lval.h"
#include <state.h>
#include <stdio.h>
#include <string.h>


void state_define_symbol(lval_t* symbol_val, lval_t* tmp_val) {
  symbol_t* symbol = context_alloc(&state_arena, sizeof(symbol_t));
  symbol->name = symbol_val->content.sym;

  lval_t* val = context_alloc(&state_arena, sizeof(lval_t));
  //TODO: Deep copy of tmp_val - right now cells are not preserved
  memcpy(val, tmp_val, sizeof(lval_t));
  val->cells = NULL; // Remove when deep copy is done.

  symbol->value = val;
  symbol->next = state.symbols;
  state.symbols = symbol;
}
