#include <unistd.h>
#ifndef ARENA_H
#define ARENA_H

#define DEFAULT_ARENA_SIZE 1024 * 1024

typedef struct Arena {
  void* data;
  size_t pad;
  size_t available;
} Arena;

extern Arena parsing_arena;

void* context_alloc(Arena* arena, size_t size);
void context_reset(Arena* arena);
void context_free(Arena* arena);
#endif
