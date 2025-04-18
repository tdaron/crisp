#include <arena.h>
#include <editline/readline.h>
#include <eval.h>
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sv.h>

#include "context.h"

arena_t tmp_arena = {0};

context_t* current_context = NULL;

int main() {
    printf("=================\n");
    printf("CRISP REPL v0.0.1\n");
    printf("=================\n");
    push_context();
    while (1) {
        char* raw_input = readline("crisp> ");
        add_history(raw_input);
        size_t len = strlen(raw_input);

        char* state_input = context_alloc(&current_context->arena, len + 1);
        memcpy(state_input, raw_input, len + 1);
        free(raw_input);

        SV input = sv_from_parts(state_input, len);
        if (sv_eq(input, sv_from_cstr("exit"))) {
            break;
        }
        lval_t* val = parse(&input);
        // print_lval_debug(val, 0);
        lval_t* result = eval(val);
        print_lval_debug(result, 0);
        context_reset(&tmp_arena);
    }
    context_free(&tmp_arena);
    pop_context();
    return 0;
}
