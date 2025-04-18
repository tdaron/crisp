#include <parser.h>
#include <eval.h>
#include <arena.h>
#include <stdio.h>
#include <editline/readline.h>
#include <stdlib.h>
#include <sv.h>


#include <stdio.h>

static arena_t arena = {0};
arena_t* context_arena = &arena;



int main() {
  printf("=================\n");
  printf("CRISP REPL v0.0.1\n");
  printf("=================\n");

  while (1) {
    char* raw_input = readline("crisp> ");
    add_history(raw_input);

    SV input = sv_from_cstr(raw_input);
    if (sv_eq(input, sv_from_cstr("exit"))) {
        free(raw_input);
        break;
    }
    lval_t* val = parse(&input);
    // print_lval_debug(val, 0);
    lval_t* result = eval(val);
    print_lval_debug(result, 0);
    free(raw_input);
    context_reset();

  }
  context_free();
  return 0;
}


