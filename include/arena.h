#include <unistd.h>
#ifndef ARENA_H
#define ARENA_H

#define DEFAULT_ARENA_SIZE 1024 * 1024

typedef struct Arena {
  void* data;
  size_t pad;
  size_t available;
} arena_t;

extern arena_t parsing_arena;

void* context_alloc(arena_t* arena, size_t size);
void context_reset(arena_t* arena);
void context_free(arena_t* arena);
#endif
