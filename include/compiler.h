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
  OP_DIV
} Opcode ;


typedef struct Bytecode {
  uint8_t* items;
  size_t size;
  size_t capacity;
} bytecode_t;


#define da_append(xs, x) \
  do { \
    if (xs->size >= xs->capacity) { \
      if (xs->capacity == 0) xs->capacity = 256; \
      else xs->capacity *= 2; \
      xs->items = realloc(xs->items, xs->capacity*sizeof(*xs->items)); \
    } \
    xs->items[xs->size++] = (uint8_t)x; \
     \
  } while (0) \


#define da_append_block(xs, src_ptr, src_size) \
  do { \
    size_t needed = xs->size + src_size; \
    if (needed > xs->capacity) { \
      while (xs->capacity < needed) { \
        xs->capacity = xs->capacity == 0 ? src_size*10 : xs->capacity * 2; \
      } \
      xs->items = realloc(xs->items, xs->capacity * sizeof(*xs->items)); \
    } \
    for (size_t i = 0; i < src_size; i++) { \
      xs->items[xs->size++] = ((uint8_t*)src_ptr)[i]; \
    } \
  } while (0)


bytecode_t* compile(lval_t* val, bytecode_t* code);

#endif
