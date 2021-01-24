#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "json.h"

#define PRIME_VALUE 0x01000193
#define JSON_BUCKETS 1024

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

struct JsonObject *json_init_object() {
    struct JsonObject *object;
    uint32_t *sizes = malloc(JSON_BUCKETS * sizeof(uint32_t));
    if (sizes == NULL) return NULL;
    struct JsonEntry **buckets = malloc(JSON_BUCKETS * sizeof(struct JsonEntry *));
    if (buckets == NULL) {
        free(sizes);
        return NULL;
    }
    object = malloc(sizeof(struct JsonObject));
    if (object == NULL) {
        free(sizes);
        free(buckets);
        return NULL;
    }
    object->sizes = sizes;
    object->buckets = buckets;
    object->entry = NULL;
    object->bucket_count = JSON_BUCKETS;
    return object;
}

void json_destroy_object(struct JsonObject *object) {
    struct JsonEntry *tmp_entry;
    struct JsonEntry *entry = object->entry;
    while (entry != NULL) {
        tmp_entry = entry->next;
        json_destroy_entry(entry);
        entry = tmp_entry;
    }
    free(object->sizes);
    free(object->buckets);
    free(object);
}

struct JsonArray *json_init_array() {
    struct JsonArray *array = malloc(sizeof(struct JsonArray));
    if (array == NULL) return NULL;
    array->items = malloc(JSON_BUCKETS * sizeof(struct JsonEntry));
    array->capacity = JSON_BUCKETS;
    array->size = 0;
    return array;
}

void json_destroy_array(struct JsonArray *array) {
    int index;
    struct JsonEntry *entry;
    for (index = 0; index < array->size; index++) {
        entry = array->items[index];
        json_destroy_entry(entry);
    }
    free(array->items);
}

struct JsonEntry *json_init_entry() {
    struct JsonEntry *entry = malloc(sizeof(struct JsonEntry));
    if (entry == NULL) return NULL;
    entry->key = NULL;
    entry->value = NULL;
    entry->next = NULL;
    entry->inferred_type = JSON_UNKNOWN;
    return entry;
}

void json_destroy_entry(struct JsonEntry *entry) {
    if (entry->value != NULL) {
        if (entry->inferred_type == JSON_OBJECT)
            json_destroy_object((struct JsonObject *) entry->value);
        else if (entry->inferred_type == JSON_ARRAY)
            json_destroy_array((struct JsonArray *) entry->value);
        else
            json_destroy_entry((struct JsonEntry *) entry->value);
    }
    if (entry->key != NULL) {
        free(entry->key);
    }
    free(entry);
}
