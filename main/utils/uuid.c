#include <stdio.h>
#include <string.h>
#include "uuid.h"

/*
 * UUID v4 generator.
 *
 * Generates a random UUID version 4 string using esp_random().
 * The format is xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx where x is
 * random hex and y is one of 8, 9, A or B.  Returns 0 on success.
 */

#include "esp_random.h"

int uuid_generate(char *out, size_t max_len)
{
    if (!out || max_len < 37) {
        return -1;
    }
    uint8_t bytes[16];
    for (int i = 0; i < 16; i += 4) {
        uint32_t r = esp_random();
        bytes[i]     = (r >> 0) & 0xFF;
        bytes[i + 1] = (r >> 8) & 0xFF;
        bytes[i + 2] = (r >> 16) & 0xFF;
        bytes[i + 3] = (r >> 24) & 0xFF;
    }
    // Set version (4) and variant (10xx)
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;
    snprintf(out, max_len,
             "%02x%02x%02x%02x-"
             "%02x%02x-"
             "%02x%02x-"
             "%02x%02x-"
             "%02x%02x%02x%02x%02x%02x",
             bytes[0], bytes[1], bytes[2], bytes[3],
             bytes[4], bytes[5],
             bytes[6], bytes[7],
             bytes[8], bytes[9],
             bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
    return 0;
}
