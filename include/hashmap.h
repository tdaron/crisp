#include <stddef.h>
#include <stdint.h>
#include <sv.h>
#ifndef HASHMAP_H
#define HASHMAP_H

typedef uint32_t Hash;


typedef struct {
  Hash hash;
  void* value;
} Element;

typedef struct {
  size_t capacity;
  size_t size;
  Element* items;
} Bucket;

typedef struct {
  size_t capacity;
  size_t size;
  Bucket* buckets;
} Hashmap;

static inline uint32_t __hash(const void *key, size_t len) {
    const uint8_t *data = (const uint8_t *)key;
    uint32_t hash = 2166136261u; // FNV offset basis
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619u; // FNV prime
    }
    return hash;
}
#define hash_sv(sv) __hash(sv.data, sv.count)


void hashmap_add(Hashmap* h, Hash key, void* value);
void* hashmap_lookup(Hashmap* h, Hash key);
#endif
