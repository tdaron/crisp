#pragma once

#include "arena.h"
#include "lval.h"
#include "sv.h"

typedef struct Symbol {
    struct Symbol* next;
    lval_t* sym;
    lval_t* value;
} symbol_t;

typedef struct Context {
    // linked list eheh boy
    struct Context* parent;
    symbol_t* symbols;
    arena_t arena;
} context_t;


extern context_t* current_context;
void context_define_symbol(lval_t* symbol_val, lval_t* tmp_val);
void pop_context();
void push_context();
lval_t* lookup_symbol(context_t* context, SV name, lval_t** symbol_info);
void match_args_params(lval_t* function, lval_t* call);
