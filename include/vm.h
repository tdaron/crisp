#include <compiler.h>
#include <stdint.h>
#include <unistd.h>
#ifndef VM_H
#define VM_H

#define STACK_SIZE 2048
#define NUM_GLOBALS 512

typedef enum {
    VAL_NULL = 0,
    VAL_DOUBLE,
    VAL_BOOL,
    VAL_PTR,  // Pointer to an object
} ValueType;

typedef struct {
    ValueType type;
    union {
        double d;
        bool b;
        void* ptr;
    } as;
} Value;

typedef struct s {
    struct s* next;
    Value val;
    SV name;
} Symbol;

typedef struct cf {
    size_t return_addr;
    Symbol* symbols;
} CallFrame;

typedef struct f {
    struct f* next;  // linked list
    size_t position;
    size_t args_number;
    SV name;
} Function;

typedef struct {
    Function* functions;  // Functions linked list
    CallFrame call_stack[STACK_SIZE];
    Value stack[STACK_SIZE];
    int sp;                      // Stack Pointer
    size_t ip;                   // Instruction Pointer
    int csp;                     // Call Stack Pointer
    Value globals[NUM_GLOBALS];  // variables globales simples
} VM;

#define IS_DOUBLE(v) ((v).type == VAL_DOUBLE)
#define IS_BOOL(v) ((v).type == VAL_BOOL)
#define IS_NULL(v) ((v).type == VAL_NULL)

#define DOUBLE_VAL(x) ((Value){VAL_DOUBLE, {.d = (x)}})
#define BOOL_VAL(x) ((Value){VAL_BOOL, {.b = (x)}})
#define NULL_VAL ((Value){VAL_NULL, {.d = 0}})
#define PTR_VAL(p) ((Value){VAL_PTR, {.ptr = (p)}})

void execute(VM* vm, bytecode_t* code, size_t start_ip);

#endif
