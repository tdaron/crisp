#pragma once

#include "lval.h"
#include "sv.h"

typedef struct Symbol {
  struct Symbol* next;
  SV name;
  lval_t* value;
} symbol_t;

typedef struct State {
  //linked list eheh boy
  symbol_t* symbols;
} state_t;

extern state_t state;
void state_define_symbol(lval_t* symbol_val, lval_t* tmp_val);
