#include <stdio.h>
#include <stdint.h>
#include "json.h"

void null_check(struct JsonEntry *entry, uint32_t length, char *json_str) {
    if (entry == NULL) {
        printf("Error parsing JSON '%s'\n", json_str);
        printf("Position: %i\n", length);
    }
    else {
        json_destroy_entry(entry);
        printf("Successfully parsed JSON '%s'\n", json_str);
    }
}

int main() {
    uint32_t length;
    unsigned char *ptr;
    unsigned char valid_json1[] = "{\"key\": 1}";
    unsigned char valid_json2[] = "{\"key\": {\"Key\": 2}}";
    // valid_json1 test
    length = 11;
    ptr = (unsigned char *)valid_json1;
    struct JsonEntry *entry = json_parse_entry(&ptr, &length);
    null_check(entry, length, (char *)valid_json1);
    return 0;
}
