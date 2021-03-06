#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "json.h"

struct JsonObject *json_init_object() {
    int index;
    struct JsonObject *object;
    uint32_t *sizes = calloc(JSON_BUCKETS, sizeof(uint32_t));
    if (sizes == NULL) return NULL;
    struct JsonEntry ***buckets = calloc(JSON_BUCKETS, sizeof(struct JsonEntry **));
    if (buckets == NULL) {
        free(sizes);
        return NULL;
    }
    for (index = 0; index < JSON_BUCKETS; index++) {
        buckets[index] = calloc(JSON_DEPTH, sizeof(struct JsonEntry *));
        if (buckets[index] == NULL) {
            index--;
            while (index >= 0) {
                free(buckets[index]);
                index--;
            }
            free(sizes);
            free(buckets);
            return NULL;
        }
        sizes[index] = JSON_DEPTH;
    }
    object = malloc(sizeof(struct JsonObject));
    if (object == NULL) {
        for (index = 0; index < JSON_BUCKETS; index++) free(buckets[index]);
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
    int index;
    struct JsonEntry *tmp_entry;
    struct JsonEntry *entry = object->entry;
    while (entry != NULL) {
        tmp_entry = entry->next;
        json_destroy_entry(entry);
        entry = tmp_entry;
    }
    for (index = 0; index < object->bucket_count; index++) free(object->buckets[index]);
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
    entry->previous = NULL;
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
            free(entry->value);
    }
    if (entry->key != NULL) {
        free(entry->key);
    }
    free(entry);
}
