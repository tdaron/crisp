#include <arena.h>
#include <editline/readline.h>
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sv.h>
#include <compiler.h>
#include <vm.h>

Arena parsing_arena = {0};


int main() {
    printf("=================\n");
    printf("CRISP REPL v0.0.1\n");
    printf("=================\n");
    VM vm = {0};
    vm.sp = 0;
    add_history("(+ (if (= 1 1) 2 3) 4)");
    add_history("(begin (define (square n) (+ n n)) (+ 1 2))");
    bytecode_t* bytecode = calloc(1, sizeof(bytecode_t));
    while (1) {
        char* raw_input = readline("crisp> ");
        add_history(raw_input);
        size_t len = strlen(raw_input);

        char* state_input = context_alloc(&parsing_arena, len + 1);
        memcpy(state_input, raw_input, len + 1);
        free(raw_input);

        SV input = sv_from_parts(state_input, len);
        if (sv_eq(input, sv_from_cstr("exit"))) {
            break;
        }
        lval_t* val = parse(&input);
        // print_lval_debug(val, 0);

        size_t starting_point = bytecode->size;
        bytecode = compile(val, bytecode);
        execute(&vm, bytecode, starting_point);
        // lval_t* result = eval(val);
        // print_lval_debug(result, 0);
        context_reset(&parsing_arena);
    }
    free(bytecode->items);
    free(bytecode);
    context_free(&parsing_arena);
    return 0;
}
