#pragma once

#include "sv.h"
enum LVAL_TYPE {
  LVAL_ERR = 0,
  LVAL_NUM,
  LVAL_SYM,
  LVAL_SEXPR
};

typedef struct lval {
  enum LVAL_TYPE type;
  // linked list
  struct lval *cells;
  union {
      char* err;
      SV sym;
      double num;
  } content;
  struct lval* next;
} lval_t;

lval_t* lval_op_add(lval_t* cells);

void print_lval_debug(const lval_t* v, int indent);
