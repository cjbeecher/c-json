#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

#define PRIME_VALUE 0x01000193

uint32_t defaultHash(const unsigned char *byte, size_t count) {
    const unsigned char *ptr = byte;
    uint32_t hash = 0x811C9DC5;
    while (count--) {
        hash = (*ptr ^ hash) * PRIME_VALUE;
        ptr++;
    }
    return hash;
}

uint32_t (*stringHash)(const unsigned char *, size_t) = &defaultHash;

int json_object_add(struct JsonObject *object, struct JsonEntry *entry) {
    uint32_t bucket;
    uint32_t size;
    struct JsonEntry **tmp_buckets;
    if (entry->value == NULL) return JSON_ENTRY_KEY_NULL;
    bucket = (*stringHash)((const unsigned char *)entry->key, strlen(entry->key)) % object->bucket_count;
    size = object->sizes[bucket] - 1;
    while (size >= 0) {
        if (object->buckets[bucket][size] == NULL) {
            object->buckets[bucket][size] = entry;
            break;
        }
        if (strcmp(object->buckets[bucket][size]->key, entry->key) == 0) return JSON_ENTRY_KEY_EXISTS;
        size--;
    }
    if (size == -1) {
        tmp_buckets = realloc(object->buckets[bucket], object->sizes[bucket] + JSON_DEPTH);
        if (tmp_buckets == NULL) return JSON_ENTRY_REALLOC_FAILED;
        object->sizes[bucket] += JSON_DEPTH;
        size += JSON_DEPTH;
    }
    object->buckets[bucket][size] = entry;
    entry->next = object->buckets[bucket][size]->next;
    object->buckets[bucket][size]->next = entry;
    return JSON_ENTRY_ADDED;
}
