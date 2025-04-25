#include "arena.h"
#include "sv.h"

#ifndef LVAL_H
#define LVAL_H

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
lval_t* lval_op_mult(lval_t* cells);

void print_lval_debug(const lval_t* v, int indent);
lval_t* allocate_lval(Arena* arena, enum LVAL_TYPE type);
lval_t* lval_clone(Arena* arena, const lval_t* v, bool clone_next_too);
#endif
