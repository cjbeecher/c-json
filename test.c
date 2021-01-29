#include <stdio.h>
#include <stdint.h>
#include "json.h"

int main() {
    uint32_t length = 11;
    unsigned char *ptr;
    unsigned char valid_json1[] = "{\"key\": 1}";
    ptr = (unsigned char *)valid_json1;
    struct JsonEntry *entry = json_parse_entry(&ptr, &length);
    if (entry == NULL)
        printf("Error parsing JSON");
    else {
        json_destroy_entry(entry);
        printf("Successfully parsed JSON");
    }
    return 0;
}
