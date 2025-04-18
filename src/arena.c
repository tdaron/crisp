#include <arena.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *context_alloc(arena_t *context_arena, size_t size) {
  if (context_arena == NULL) {
    return NULL;
  }

  if (context_arena->data == NULL) {
    context_arena->data = malloc(DEFAULT_ARENA_SIZE);
    context_arena->available = DEFAULT_ARENA_SIZE;
  }
  if (context_arena->available < size) {
    // TODO: Support allocating a new region.
    return NULL;
  }

  context_arena->pad += size;
  context_arena->available -= size;
  memset(context_arena->data + context_arena->pad, 0, size);
  return context_arena->data + context_arena->pad - size;
}

void context_reset(arena_t *context_arena) {
  context_arena->available += context_arena->pad;
  context_arena->pad = 0;
}
void context_free(arena_t *context_arena) {
  if (context_arena->data != NULL) {
    free(context_arena->data);
  }
}
