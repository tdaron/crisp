#include "sv.h"
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arena.h>

bool is_whitespace(char v) { return v == ' '; }

bool valid_sym(char v) { return !(is_whitespace(v)) && v != ')'; }
lval_t *parse_atom(SV *input);

lval_t *parse_sexpr(SV *input) {
  consume(input); // first (
  sv_chop_left_while(input, is_whitespace);

  lval_t*operand = parse_atom(input);

  if (operand->type != LVAL_SYM) {
    printf("S-Expressions should start with a symbol...\n");
  }

  operand->type = LVAL_SEXPR;

  sv_chop_left_while(input, is_whitespace);
  while (input->count > 0 && *input->data != ')') {
    lval_t*child = parse(input);
    lval_t*last = operand->cells;
    if (last == NULL) {
      operand->cells = child;
    } else {
      while (last != NULL && last->next != NULL) {
        last = last->next;
      }
      last->next = child;
    }
    sv_chop_left_while(input, is_whitespace);
  }

  if (input->count == 0) {
    printf("Unexpectd EOF at the end of the S-Expr. Did you forgot a ) ?\n");
  }

  consume(input); // last )
  return operand;
}

lval_t*parse_atom(SV *input) {
  SV atom_content = sv_chop_left_while(input, valid_sym);
  char buf[16];

  // do not support numbers of more than 16 digits long.
  if (atom_content.count >= 16) {
    goto sym;
  }

  // TODO: Maybe add a \0 temporarly ito atom_content.data to emulate a null
  // terminated string instead of making a copy in memory. IDK if it will look
  // better.
  memcpy(buf, atom_content.data, atom_content.count);
  buf[atom_content.count] = '\0';
  char *end;
  double result = strtod(buf, &end);
  if (end != NULL && (size_t)(end - buf) == atom_content.count) {
    lval_t*val = context_alloc(&tmp_arena, sizeof(lval_t));
    val->type = LVAL_NUM;
    val->cells = NULL;
    val->content.num = result;
    val->next = NULL;
    return val;
  }

sym: {
  lval_t*val = context_alloc(&tmp_arena, sizeof(lval_t));
  val->type = LVAL_SYM;
  val->cells = NULL;
  val->content.sym = atom_content;
  val->next = NULL;
  return val;
}
}

lval_t*parse(SV *input) {
  sv_chop_left_while(input, is_whitespace);

  if (*input->data == '(') {
    return parse_sexpr(input);
  }

  return parse_atom(input);
}
