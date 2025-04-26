#include <arena.h>
#include <compiler.h>
#include <editline/readline.h>
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sv.h>
#include <time.h>
#include <raylib_bind.h>
#include <vm.h>

Arena parsing_arena = {0};

void test(VM* vm) {
    double a1 = POP_DOUBLE(vm);
    printf("Hello, World ! %f\n", a1);
}

int main(int argc, char* argv[]) {
    VM vm = {0};
    add_ffi_func(&vm, sv_from_cstr("test"), test);
    bind_raylib(&vm);
    vm.sp = 0;
    bytecode_t* bytecode = calloc(1, sizeof(bytecode_t));
    
    // Check if a file was provided as an argument
    if (argc > 1) {
        // Open the file
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
            return 1;
        }
        
        // Get file size
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // Read file contents
        char* file_contents = context_alloc(&parsing_arena, file_size + 1);
        if (file_contents == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            fclose(file);
            return 1;
        }
        
        size_t bytes_read = fread(file_contents, 1, file_size, file);
        fclose(file);
        
        if (bytes_read < file_size) {
            fprintf(stderr, "Error: Could not read entire file\n");
            return 1;
        }
        
        file_contents[bytes_read] = '\0';
        
        // Parse and execute the file contents
        SV input = sv_from_parts(file_contents, bytes_read);
        lval_t* val = parse(&input);
        
        if (val) {
            size_t starting_point = bytecode->size;
            bytecode = compile(val, bytecode);
            
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            execute(&vm, bytecode, starting_point);
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                              (end.tv_nsec - start.tv_nsec) / 1000000;
            printf("Execution time: %ld ms\n", elapsed_ms);
        } else {
            fprintf(stderr, "Error: Failed to parse file\n");
        }
    } else {
        // No file provided, start REPL
        printf("=================\n");
        printf("CRISP REPL v0.0.1\n");
        printf("=================\n");
        
        add_history("(+ (if (= 1 1) 2 3) 4)");
        add_history("(begin (define (square n) (+ n n)) (+ 1 2))");
        add_history("(define (fib n) (if (< n 2) n (+ (fib (- n 1)) (fib (- n 2)))))");
        
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
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            execute(&vm, bytecode, starting_point);
            clock_gettime(CLOCK_MONOTONIC, &end);
            long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                              (end.tv_nsec - start.tv_nsec) / 1000000;
            printf("Execution time: %ld ms\n", elapsed_ms);
            context_reset(&parsing_arena);
        }
    }
    
    free(bytecode->items);
    free(bytecode);
    context_free(&parsing_arena);
    return 0;
}
