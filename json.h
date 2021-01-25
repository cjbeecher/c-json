#ifndef C_JSON_INIT_H
#define C_JSON_INIT_H

#include <stdint.h>

#define JSON_BUCKETS 1024
#define JSON_DEPTH 5

#define JSON_ENTRY_ADDED 0
#define JSON_ENTRY_KEY_NULL 1
#define JSON_ENTRY_KEY_EXISTS 2
#define JSON_ENTRY_REALLOC_FAILED 3
#define JSON_ENTRY_KEY_NOT_NULL 4
#define JSON_ENTRY_OUT_OF_BOUNDS 5

#define JSON_ENTRY_REMOVED 0
#define JSON_ENTRY_NOT_EXISTS 2

#define JSON_UNKNOWN -1
#define JSON_STRING   0
#define JSON_INTEGER  1
#define JSON_FLOAT    2
#define JSON_OBJECT   3
#define JSON_ARRAY    4
#define JSON_NULL     5

struct JsonEntry {
    char *key;
    void *value;
    struct JsonEntry *previous;
    struct JsonEntry *next;
    char inferred_type;
    uint32_t value_length;
    uint16_t key_length;
};

struct JsonArray {
    struct JsonEntry **items;
    uint32_t capacity;
    uint32_t size;
};

struct JsonObject {
    uint32_t bucket_count;
    uint32_t *sizes;
    struct JsonEntry ***buckets;
    struct JsonEntry *entry;
};

void json_set_default_hash(uint32_t (*stringHash)(const unsigned char *, size_t));
struct JsonObject *json_init_object();
void json_destroy_object(struct JsonObject *object);
struct JsonArray *json_init_array();
void json_destroy_array(struct JsonArray *array);
struct JsonEntry *json_init_entry();
void json_destroy_entry(struct JsonEntry *entry);

int json_object_add(struct JsonObject *object, struct JsonEntry *entry);
int json_object_remove(struct JsonObject *object, const char *key);
int json_array_add(struct JsonArray *array, struct JsonEntry *entry);
int json_array_remove(struct JsonArray *array, size_t index);

struct JsonArray *json_parse_array(unsigned char **data, uint32_t *length);
struct JsonObject *json_parse_object(unsigned char **data, uint32_t *length);
struct JsonEntry *json_parse_entry(unsigned char **data, uint32_t *length);

#endif //C_JSON_INIT_H
