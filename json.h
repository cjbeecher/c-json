#ifndef C_JSON_INIT_H
#define C_JSON_INIT_H

#include <stdint.h>

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
    struct JsonEntry **buckets;
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

struct JsonArray *json_parse_array(unsigned char **data, uint32_t *length);
struct JsonObject *json_parse_object(unsigned char **data, uint32_t *length);
struct JsonEntry *json_parse_entry(unsigned char **data, uint32_t *length);

#endif //C_JSON_INIT_H
