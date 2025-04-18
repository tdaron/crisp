#include "lval.h"
#include "state.h"
#include "sv.h"
#include <parser.h>
#include <stdio.h>

lval_t *eval(lval_t *val) {
  switch (val->type) {
  case LVAL_NUM:
    return val;

  case LVAL_SYM: {
    symbol_t *curr = state.symbols;
    while (curr != NULL) {
      if (sv_eq(curr->name, val->content.sym)) {
        return curr->value;
      }
      curr = curr->next;
    }

    fprintf(stderr, "Symbol "SV_Fmt" not found\n", SV_Arg(val->content.sym));
    return val;
  }
  case LVAL_SEXPR:
    if (sv_eq(val->content.sym, sv_from_cstr("+"))) {
      return lval_op_add(val->cells);
      break;
    }
    if (sv_eq(val->content.sym, sv_from_cstr("define"))) {
      lval_t *name = val->cells;
      lval_t *value = val->cells->next;
      state_define_symbol(name, value);
      return eval(name);
    }
    break;

  default:
    // Optionally handle unexpected types
    fprintf(stderr, "Unknown lval type: %d\n", val->type);
    return val;
  }
  return val;
}
