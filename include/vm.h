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
} StackValue;

typedef struct Symbol {
    struct Symbol* next;
    StackValue val;
    SV name;
} Symbol;

typedef struct {
    size_t return_addr;
    Symbol* symbols;
} CallFrame;

typedef struct Function {
    struct Function* next;  // linked list
    size_t position;
    size_t args_number;
    SV name;
} Function;

typedef struct {
    Function* functions;  // Functions linked list
    CallFrame call_stack[STACK_SIZE];
    StackValue stack[STACK_SIZE];
    int sp;                      // Stack Pointer
    size_t ip;                   // Instruction Pointer
    int csp;                     // Call Stack Pointer
    StackValue globals[NUM_GLOBALS];  // variables globales simples
} VM;

#define IS_DOUBLE(v) ((v).type == VAL_DOUBLE)
#define IS_BOOL(v) ((v).type == VAL_BOOL)
#define IS_NULL(v) ((v).type == VAL_NULL)

#define DOUBLE_VAL(x) ((StackValue){VAL_DOUBLE, {.d = (x)}})
#define BOOL_VAL(x) ((StackValue){VAL_BOOL, {.b = (x)}})
#define NULL_VAL ((StackValue){VAL_NULL, {.d = 0}})
#define PTR_VAL(p) ((StackValue){VAL_PTR, {.ptr = (p)}})

void execute(VM* vm, bytecode_t* code, size_t start_ip);

#endif
