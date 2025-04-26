#include <hashmap.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_LOAD_FACTOR 0.7

static void* safe_realloc(void* ptr, size_t size) {
    void* tmp = realloc(ptr, size);
    if (!tmp) {
        // Handle allocation failure (you can exit or return NULL)
        exit(1);  // or use a custom error handling mechanism
    }
    return tmp;
}

static void hashmap_resize(Hashmap* h) {
    uint32_t old_capacity = h->capacity;
    Bucket* old_buckets = h->buckets;

    // Double the capacity
    h->capacity *= 2;
    if (h->capacity == 0) h->capacity = 1;
    h->buckets = malloc(h->capacity * sizeof(Bucket));

    // Initialize new buckets to zero
    for (uint32_t i = 0; i < h->capacity; ++i) {
        h->buckets[i].size = 0;
        h->buckets[i].capacity = 0;
        h->buckets[i].items = NULL;
    }

    // Rehash all elements into the new buckets
    for (uint32_t i = 0; i < old_capacity; ++i) {
        Bucket* old_bucket = &old_buckets[i];
        for (size_t j = 0; j < old_bucket->size; ++j) {
            hashmap_add(h, old_bucket->items[j].hash,
                        old_bucket->items[j].value);
        }
    }

    // Free old bucket memory
    for (uint32_t i = 0; i < old_capacity; ++i) {
        free(old_buckets[i].items);
    }
    if (old_buckets != NULL) free(old_buckets);
}

void* bucket_lookup(Bucket* bucket, Hash hash, size_t* idx) {
    for (size_t i = 0; i < bucket->size; i++) {
        if (bucket->items[i].hash == hash) {
            if (idx != NULL) *idx = i;
            return bucket->items[i].value;
        }
    }
    return NULL;
}

static void bucket_add(Bucket* bucket, Element element) {
    // If the element is already present, we want to replace it.
    size_t idx;
    if (bucket_lookup(bucket, element.hash, &idx)) {
        bucket->items[idx] = element;
        return;
    }
    if (bucket->size >= bucket->capacity) {
        if (bucket->capacity == 0) {
            bucket->capacity = 16;  // Start with a reasonable capacity
        } else {
            bucket->capacity *= 2;
        }
        bucket->items = safe_realloc(bucket->items,
                                     bucket->capacity * sizeof(*bucket->items));
    }

    bucket->items[bucket->size++] = element;
}

void hashmap_add(Hashmap* h, Hash hash, void* value) {
    if (h->capacity == 0 ||
        (float)h->size / h->capacity > HASHMAP_LOAD_FACTOR) {
        hashmap_resize(h);  // Rehash if load factor is exceeded
    }
    uint32_t idx = hash % h->capacity;
    bucket_add(&h->buckets[idx], (Element){.value = value, .hash = hash});
    h->size++;
}
void* hashmap_lookup(Hashmap* h, Hash hash) {
    if (h->size == 0) return NULL;
    uint32_t idx = hash % h->capacity;
    Bucket* bucket = &h->buckets[idx];
    return bucket_lookup(bucket, hash, NULL);
}

void hashmap_free(Hashmap* h) {
    for (uint32_t i = 0; i < h->capacity; ++i) {
        free(h->buckets[i].items);
    }
    free(h->buckets);
}
