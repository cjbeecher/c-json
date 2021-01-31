#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "json.h"

bool null_check(struct JsonEntry *entry, uint32_t length, char *json_str) {
    if (entry == NULL) {
        printf("status=failed|json='%s'|position=%i\n", json_str, length);
        return false;
    }
    else {
        printf("status=success|json='%s'\n", json_str);
        return true;
    }
}

int main() {
    uint32_t length;
    unsigned char *ptr;
    struct JsonEntry *entry;
    struct JsonEntry *tmp_entry;
    struct JsonObject *tmp_object;
    unsigned char valid_json1[] = "{\"key\": 1}";
    unsigned char valid_json2[] = "{\"key\": {\"subKey\": 2}}";
    // valid_json1 test
    length = 11;
    ptr = (unsigned char *)valid_json1;
    entry = json_parse_entry(&ptr, &length);
    if (null_check(entry, length, (char *)valid_json1))
        json_destroy_entry(entry);
    // valid_json2 test
    length = 23;
    ptr = (unsigned char *)valid_json2;
    entry = json_parse_entry(&ptr, &length);
    if (null_check(entry, length, (char *)valid_json2)) {
        tmp_object = JSON_TO_OBJECT(entry);
        tmp_entry = tmp_object->entry;
        if (tmp_entry == json_object_get(tmp_object, "key")) {
            printf(
                    "object=valid2|key=key|type=%i|entry=%p\n",
                    (JSON_TO_OBJECT(tmp_entry))->entry->inferred_type,
                    tmp_entry
                );
            if ((JSON_TO_OBJECT(tmp_entry))->entry == json_object_get(JSON_TO_OBJECT(tmp_entry), "subKey"))
                printf(
                        "object=valid2|key=subKey|type=%i|entry=%p|value=%f\n",
                        (JSON_TO_OBJECT(tmp_entry))->entry->inferred_type,
                        (JSON_TO_OBJECT(tmp_entry))->entry,
                        (JSON_TO_DOUBLE((JSON_TO_OBJECT(tmp_entry))->entry))
                    );
            else
                printf(
                        "object=valid2|key=subKey|type=%i|entry=null\n",
                        (JSON_TO_OBJECT(tmp_entry))->entry->inferred_type
                    );
        }
        else
            printf("object=valid2|key=key|entry=null\n");
        json_destroy_entry(entry);
    }
    return 0;
}
