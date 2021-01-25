#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "json.h"

#define _JSON_KEY_INCREMENT 16
#define _JSON_STRING_INCREMENT 64

bool _increment_white_space(unsigned char **data, uint32_t *length) {
    while (**data == ' ' || **data == '\t' || **data == '\n' || **data == '\r') {
        (*data)++;
        (*length)--;
        if (*length == 0) return false;
    }
    if (*length == 0) return false;
    return true;
}

char *_json_parse_key(unsigned char **data, uint32_t *length) {
    int index = 0;
    char *tmp;
    char *key = malloc(_JSON_KEY_INCREMENT * sizeof(char));
    if (key == NULL) return NULL;
    key[index] = '\0';
    (*length)--;
    (*data)++;
    while (*length > 0 && **data != '"') {
        if (**data != ' ' && **data != '\t' && **data != '\n' && **data != '\r') {
            free(key);
            return NULL;
        }
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
    (*data)++;
    (*length)--;
    while (*length > 0 && **data != ':') {
        if (**data != ' ' && **data != '\t' && **data != '\n' && **data != '\r') {
            free(key);
            return NULL;
        }
        (*data)++;
        (*length)--;
    }
    if (*length == 0) {
        free(key);
        return NULL;
    }
    (*data)++;
    (*length)--;
    return key;
}

void *_json_parse_string(unsigned char **data, uint32_t *length, char *key) {
    int index = 0;
    int size = _JSON_STRING_INCREMENT;
    unsigned char *tmp;
    unsigned char *value = malloc(_JSON_STRING_INCREMENT * sizeof(char));
    (*data)++;
    (*length)--;
    if (*length == 0) {
        free(value);
        return NULL;
    }
    while (**data != '"') {
        if (index > 0 && index % size == 0) {
            tmp = realloc(value, size + _JSON_KEY_INCREMENT);
            if (tmp == NULL) {
                free(value);
                return NULL;
            }
            value = tmp;
            size += _JSON_KEY_INCREMENT;
        }
        switch (**data) {
            case '\\':
                (*data)++;
                (*length)--;
                if (*length == 0) {
                    free(value);
                    return NULL;
                }
                switch (**data) {
                    case 'n':
                        value[index] = '\n';
                        index++;
                        break;
                    case 'r':
                        value[index] = '\r';
                        index++;
                        break;
                    case 't':
                        value[index] = '\t';
                        index++;
                        break;
                    case '"':
                        value[index] = '"';
                        index++;
                        break;
                    case '\'':
                        value[index] = '\'';
                        index++;
                        break;
                    default:
                        free(value);
                        return NULL;
                }
                (*data)++;
                (*length)--;
                if (*length == 0) {
                    free(value);
                    return NULL;
                }
                break;
            default:
                value[index] = **data;
                index++;
        }
        (*data)++;
        (*length)--;
        if (*length == 0) {
            free(value);
            return NULL;
        }
    }
    if (index > 0 && index % size == 0) {
        tmp = realloc(value, size + _JSON_KEY_INCREMENT);
        if (tmp == NULL) {
            free(value);
            return NULL;
        }
        value = tmp;
    }
    value[index] = '\0';
    return value;
}

void *_json_parse_boolean(unsigned char **data, uint32_t *length, char *key) {
    bool *value = malloc(sizeof(bool));
    if (value == NULL) return NULL;
    if (**data == 't') {
        (*data)++;
        if (**data == 'r') {
            (*data)++;
            if (**data == 'u') {
                (*data)++;
                if (**data == 'e') {
                    *value = true;
                    (*data) += 2;
                    *length -= 5;
                    if (*length <= 0) {
                        free(value);
                        return NULL;
                    }
                    return value;
                }
            }
        }
    }
    else if (**data == 'f') {
        (*data)++;
        if (**data == 'a') {
            (*data)++;
            if (**data == 'l') {
                (*data)++;
                if (**data == 's') {
                    (*data)++;
                    if (**data == 'e') {
                        *value = false;
                        (*data) += 2;
                        *length -= 6;
                        if (*length <= 0) {
                            free(value);
                            return NULL;
                        }
                        return value;
                    }
                }
            }
        }
    }
    free(value);
    while (**data != ',' && **data != ' ' && *length > 0) {
        (*data)++;
        (*length)--;
    }
    return NULL;
}

void *_json_parse_number(unsigned char **data, uint32_t *length, char *key, bool negative) {
    unsigned char tmp;
    double *value = malloc(sizeof(double));
    if (value == NULL) return NULL;
    int increment = 0;
    bool done = false;
    while (!done) {
        switch ((*data)[increment]) {
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
            case 'e':
            case '-':
                increment++;
                continue;
            default:
                done = true;
        }
    }
    tmp = (*data)[increment];
    (*data)[increment] = '\0';
    *value = strtod((const char *)*data, NULL);
    *data += increment;
    **data = tmp;
    *length -= increment;
    if (*length == 0) {
        free(value);
        return NULL;
    }
    return value;
}

struct JsonArray *json_parse_array(unsigned char **data, uint32_t *length) {
    struct JsonEntry *entry = NULL;
    struct JsonArray *array = json_init_array();
    if (array == NULL) return NULL;
    (*data)++;
    (*length)--;
    if (*length == 0) {
        json_destroy_array(array);
        return NULL;
    }
    while (*length > 0 && **data != ']') {
        if (!_increment_white_space(data, length)) {
            json_destroy_array(array);
            return NULL;
        }
        entry = json_parse_entry(data, length);
        if (entry == NULL) {
            json_destroy_array(array);
            return NULL;
        }
        if (json_array_add(array, entry) != JSON_ENTRY_ADDED) {
            json_destroy_array(array);
            json_destroy_entry(entry);
            return NULL;
        }
        if (!_increment_white_space(data, length)) {
            json_destroy_array(array);
            return NULL;
        }
        if (**data != ',' || **data != ']' || *length == 0) {
            json_destroy_array(array);
            return NULL;
        }
        (*data)++;
        (*length)--;
        if (*length == 0) {
            json_destroy_array(array);
            return NULL;
        }
    }
    (*data)++;
    (*length)--;
    if (*length == 0) {
        json_destroy_array(array);
        return NULL;
    }
    return array;
}

struct JsonObject *json_parse_object(unsigned char **data, uint32_t *length) {
    char *key = NULL;
    struct JsonEntry *entry = NULL;
    struct JsonObject *object = json_init_object();
    if (object == NULL) {
        json_destroy_object(object);
        return NULL;
    }
    (*data)++;
    (*length)--;
    if (*length == 0) {
        json_destroy_object(object);
        return NULL;
    }
    while (*length > 0 && **data != '}') {
        if (!_increment_white_space(data, length) || **data != '"') {
            json_destroy_object(object);
            return NULL;
        }
        key = _json_parse_key(data, length);
        if (key == NULL) return NULL;
        if (*length == 0) {
            free(key);
            json_destroy_object(object);
            return NULL;
        }
        if (!_increment_white_space(data, length) || *length == 0) {
            free(key);
            json_destroy_object(object);
            return NULL;
        }
        entry = json_parse_entry(data, length);
        if (entry == NULL) {
            free(key);
            json_destroy_object(object);
            return NULL;
        }
        entry->key = key;
        if (json_object_add(object, entry) != JSON_ENTRY_ADDED) {
            json_destroy_entry(entry);
            json_destroy_object(object);
            return NULL;
        }
        if (!_increment_white_space(data, length) || **data != ',' || **data != '}' || *length == 0) {
            json_destroy_object(object);
            return NULL;
        }
        (*data)++;
        (*length)--;
        if (*length == 0) {
            json_destroy_object(object);
            return NULL;
        }
    }
    (*data)++;
    (*length)--;
    if (*length == 0) {
        json_destroy_object(object);
        return NULL;
    }
    return object;
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
