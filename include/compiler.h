#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <lval.h>
#ifndef COMPILER_H
#define COMPILER_H

typedef enum {
  OP_PUSH_CONST = 0,
  OP_ADD,
  OP_SUB,
  OP_MULT,
  OP_DIV,
  OP_EQ,
  OP_GEQ,
  OP_LEQ,
  OP_GE,
  OP_LE,
  OP_JUMPF,
  OP_JUMP,
  OP_POP,
  OP_CALL,
  OP_LOAD_SYMBOL,
  OP_SET_SYMBOL,
  OP_RET,
  OP_FUNCDEF
} Opcode ;

const char* opcode_to_string(Opcode op);

typedef struct Bytecode {
  uint8_t* items;
  size_t size;
  size_t capacity;
} bytecode_t;



bytecode_t* compile(lval_t* val, bytecode_t* code);

#endif
