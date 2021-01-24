#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "json.h"

#define _JSON_KEY_INCREMENT 16

char *_json_parse_key(unsigned char **data, uint32_t *length) {
    int index = 0;
    char *tmp;
    char *key = malloc(_JSON_KEY_INCREMENT * sizeof(char));
    if (key == NULL) return NULL;
    key[index] = '\0';
    (*length)--;
    (*data)++;
    while (*length > 0 && **data != '"') {
        key[index] = (char)**data;
        if ((index + 1) % _JSON_KEY_INCREMENT == 0) {
            tmp = realloc(key, _JSON_KEY_INCREMENT * (index + 1));
            if (tmp == NULL) {
                free(key);
                return NULL;
            }
            key = tmp;
        }
        key[index + 1] = '\0';
        index++;
        (*length)--;
        (*data)++;
    }
    if (*length == 0) {
        free(key);
        return NULL;
    }
    return key;
}

void *_json_parse_string(unsigned char **data, uint32_t *length, char *key) {
    return NULL;
}

void *_json_parse_boolean(unsigned char **data, uint32_t *length, char *key) {
    return NULL;
}

void *_json_parse_number(unsigned char **data, uint32_t *length, char *key, bool negative) {
    return NULL;
}

struct JsonObject *json_parse_object(unsigned char **data, uint32_t *length) {
    return NULL;
}

struct JsonEntry *json_parse_entry(unsigned char **data, uint32_t *length) {
    void *value;
    bool negative = false;
    struct JsonEntry *entry = NULL;
    while (*length > 0) {
        switch (**data) {
            case '\0':
                return entry;
            case '{':
                value = (void *)json_parse_object(data, length);
                if (value == NULL) return NULL;
                entry = json_init_entry();
                if (entry == NULL) return NULL;
                entry->value = value;
                entry->inferred_type = JSON_OBJECT;
                return entry;
            case '[':
                value = (void *)json_parse_array(data, length);
                if (value == NULL) return NULL;
                entry = json_init_entry();
                if (entry == NULL) return NULL;
                entry->value = value;
                entry->inferred_type = JSON_ARRAY;
                return entry;
            case '"':
                value = _json_parse_string(data, length, NULL);
                if (value == NULL) return NULL;
                entry = json_init_entry();
                if (entry == NULL) return NULL;
                entry->value = value;
                entry->inferred_type = JSON_STRING;
                return entry;
            case 't':
            case 'f':
                value = _json_parse_boolean(data, length, NULL);
                if (value == NULL) return NULL;
                entry = json_init_entry();
                if (entry == NULL) return NULL;
                entry->value = value;
                entry->inferred_type = JSON_STRING;
                return entry;
            case '-':
                negative = true;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                value = _json_parse_number(data, length, NULL, negative);
                if (value == NULL) return NULL;
                entry = json_init_entry();
                if (entry == NULL) return NULL;
                entry->value = value;
                entry->inferred_type = JSON_STRING;
                return entry;
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                break;
            default:
                return NULL;
        }
        (*data)++;
        (*length)--;
    }
    return NULL;
}
