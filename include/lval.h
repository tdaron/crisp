#pragma once

#include "arena.h"
#include "sv.h"
enum LVAL_TYPE {
  LVAL_ERR = 0,
  LVAL_NUM,
  LVAL_SYM,
  LVAL_SEXPR
};

typedef struct lval {
  enum LVAL_TYPE type;
  union {
      char* err;
      SV sym;
      double num;
      struct lval *cells;
  } content;

  // linked list
  struct lval* next;
} lval_t;

lval_t* lval_op_add(lval_t* cells);

void print_lval_debug(const lval_t* v, int indent);
lval_t* allocate_lval(arena_t* arena, enum LVAL_TYPE type);
lval_t* lval_clone(arena_t* arena, const lval_t* v, bool clone_next_too);
