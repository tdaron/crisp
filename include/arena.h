#pragma once
#include <unistd.h>

#define DEFAULT_ARENA_SIZE 1024 * 1024

typedef struct Arena {
  void* data;
  size_t pad;
  size_t available;
} arena_t;
extern arena_t* context_arena;
void* context_alloc(size_t size);
void context_reset();
void context_free();
