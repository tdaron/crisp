#include <unistd.h>
#include <stdint.h>
#include <compiler.h>
#ifndef VM_H
#define VM_H

#define STACK_SIZE 2048
#define NUM_GLOBALS 512

typedef enum {
    VAL_NULL = 0,
    VAL_DOUBLE,
    VAL_BOOL,
    VAL_PTR, // Pointer to an object
} ValueType;

typedef struct {
    ValueType type;
    union {
        double d;
        bool b;
        void* ptr;
    } as;
} Value;

typedef struct {
    Value stack[STACK_SIZE];
    int sp; // Stack Pointer
    size_t ip; // Instruction Pointer
    Value globals[NUM_GLOBALS]; // variables globales simples
} VM;


#define IS_DOUBLE(v)   ((v).type == VAL_DOUBLE)
#define IS_BOOL(v)  ((v).type == VAL_BOOL)
#define IS_NULL(v)  ((v).type == VAL_NULL)

#define DOUBLE_VAL(x)  ((Value){VAL_DOUBLE,  {.d = (x)}})
#define BOOL_VAL(x) ((Value){VAL_BOOL, {.b = (x)}})
#define NULL_VAL    ((Value){VAL_NULL, {.d = 0}})
#define PTR_VAL(p)  ((Value){VAL_PTR,  {.ptr = (p)}})

void execute(VM* vm, bytecode_t* code);


#endif
