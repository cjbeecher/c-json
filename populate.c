#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

#define PRIME_VALUE 0x01000193

uint32_t defaultHash(const char *byte, size_t count) {
    const char *ptr = byte;
    uint32_t hash = 0x811C9DC5;
    while (count--) {
        hash = (*ptr ^ hash) * PRIME_VALUE;
        ptr++;
    }
    return hash;
}

uint32_t (*stringHash)(const char *, size_t) = &defaultHash;

int json_object_add(struct JsonObject *object, struct JsonEntry *entry) {
    uint32_t bucket;
    int32_t size;
    struct JsonEntry *tmp_entry;
    struct JsonEntry **tmp_buckets;
    if (entry->key == NULL) return JSON_ENTRY_KEY_NULL;
    bucket = (*stringHash)((const char *)entry->key, strlen(entry->key)) % object->bucket_count;
    size = object->sizes[bucket] - 1;
    while (size >= 0) {
        if (object->buckets[bucket][size] == NULL)
            break;
        if (strcmp(object->buckets[bucket][size]->key, entry->key) == 0) return JSON_ENTRY_KEY_EXISTS;
        size--;
    }
    if (size == -1) {
        tmp_buckets = realloc(object->buckets[bucket], object->sizes[bucket] + JSON_DEPTH);
        if (tmp_buckets == NULL) return JSON_ENTRY_REALLOC_FAILED;
        object->sizes[bucket] += JSON_DEPTH;
        size += JSON_DEPTH;
    }
    if (object->entry == NULL) {
        object->entry = entry;
        entry->previous = NULL;
        entry->next = NULL;
    }
    else {
        tmp_entry = object->entry;
        entry->next = tmp_entry;
        tmp_entry->previous = entry;
        object->entry = entry;
    }
    object->buckets[bucket][size] = entry;
    return JSON_ENTRY_ADDED;
}

struct JsonEntry *json_object_get(struct JsonObject *object, const char *key) {
    uint32_t bucket = (*stringHash)((const char *)key, strlen(key)) % object->bucket_count;
    uint32_t size = object->sizes[bucket] - 1;
    while (size >= 0) {
        if (object->buckets[bucket][size] == NULL) return NULL;
        if (strcmp(object->buckets[bucket][size]->key, key) == 0) {
            return object->buckets[bucket][size];
        }
        size--;
    }
    return NULL;
}

int json_object_remove(struct JsonObject *object, const char *key) {
    uint32_t size;
    uint32_t bucket;
    struct JsonEntry *entry = NULL;
    if (key == NULL) return JSON_ENTRY_KEY_NULL;
    bucket = (*stringHash)((const char *)key, strlen(key)) % object->bucket_count;
    size = object->sizes[bucket] - 1;
    while (size >= 0) {
        if (object->buckets[bucket][size] == NULL) return JSON_ENTRY_NOT_EXISTS;
        if (strcmp(object->buckets[bucket][size]->key, key) == 0) {
            entry = object->buckets[bucket][size];
            break;
        }
        size--;
    }
    if (entry == NULL || size == -1) return JSON_ENTRY_NOT_EXISTS;
    while (size > 0) {
        object->buckets[bucket][size] = object->buckets[bucket][size - 1];
        size--;
    }
    object->buckets[bucket][0] = NULL;
    entry->previous->next = entry->next;
    entry->next->previous = entry->previous;
    json_destroy_entry(entry);
    return JSON_ENTRY_REMOVED;
}

int json_array_add(struct JsonArray *array, struct JsonEntry *entry) {
    struct JsonEntry **entries;
    if (entry->key != NULL) return JSON_ENTRY_KEY_NOT_NULL;
    if (array->size == array->capacity) {
        entries = realloc(array->items, array->capacity + JSON_BUCKETS);
        if (entries == NULL) return JSON_ENTRY_REALLOC_FAILED;
        array->items = entries;
        array->capacity += JSON_BUCKETS;
    }
    array->items[array->size] = entry;
    array->size++;
    return JSON_ENTRY_ADDED;
}

int json_array_remove(struct JsonArray *array, size_t index) {
    if (index >= array->size) return JSON_ENTRY_OUT_OF_BOUNDS;
    json_destroy_entry(array->items[index]);
    array->items[index] = array->items[--array->size];
    array->items[array->size] = NULL;
    return JSON_ENTRY_REMOVED;
}
