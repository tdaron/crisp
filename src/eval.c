#include "lval.h"
#include "sv.h"
#include <parser.h>
#include <stdio.h>

lval_t *eval(lval_t *val) {
  switch (val->type) {
  case LVAL_NUM:
    return val;

  case LVAL_SYM:
    fprintf(stderr, "Symbols not yet implemented\n");
    return val;
  case LVAL_SEXPR:
    if (sv_eq(val->content.sym, sv_from_cstr("+"))) {
      return lval_op_add(val->cells);
    }
    break;

  default:
    // Optionally handle unexpected types
    fprintf(stderr, "Unknown lval type: %d\n", val->type);
    return val;
  }
  return val;
}
