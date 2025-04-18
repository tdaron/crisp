#include "parser.h"
#include <stdio.h>
#include <editline/readline.h>
#include <stdlib.h>
#include <sv.h>


#include <stdio.h>

void print_lval_debug(const lval* v, int indent);

void print_indent(int indent) {
    for (int i = 0; i < indent; i++) putchar(' ');
}

const char* lval_type_name(enum LVAL_TYPE type) {
    switch (type) {
        case LVAL_ERR:  return "ERR";
        case LVAL_NUM:  return "NUM";
        case LVAL_SYM:  return "SYM";
        case LVAL_SEXPR: return "SEXPR";
        default:        return "UNKNOWN";
    }
}

void print_lval_debug(const lval* v, int indent) {
    if (!v) return;

    print_indent(indent);
    printf("type: %s\n", lval_type_name(v->type));

    print_indent(indent);
    printf("value: ");
    switch (v->type) {
        case LVAL_NUM:
            printf("%g\n", v->content.num);
            break;
        case LVAL_ERR:
            printf("%s\n", v->content.err);
            break;
        case LVAL_SYM:
            printf(SV_Fmt"\n", SV_Arg(v->content.sym));
            break;
        case LVAL_SEXPR:
            printf(SV_Fmt"\n", SV_Arg(v->content.sym));
            break;
    }

    if (v->type == LVAL_SEXPR && v->cells) {
        print_indent(indent);
        printf("children:\n");
        const lval* child = v->cells;
        while (child) {
            print_lval_debug(child, indent + 2);
            child = child->next;
        }
    }
}


int main() {
  printf("=================\n");
  printf("CRISP REPL v0.0.1\n");
  printf("=================\n");

  while (1) {
    char* raw_input = readline("crisp> ");
    add_history(raw_input);

    SV input = sv_from_cstr(raw_input);
    lval* result = parse(&input);
    print_lval_debug(result, 0);
    free(raw_input);
  }
  return 0;
}


