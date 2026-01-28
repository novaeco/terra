#include <stdio.h>
#include <string.h>
#include "json_utils.h"

/*
 * JSON utilities implementation.
 *
 * Wraps the cJSON library to parse and serialise JSON documents.
 * The caller is responsible for freeing the returned cJSON object
 * created by json_parse().  The json_serialize() function writes
 * the serialised JSON into the provided buffer and returns 0 on
 * success.
 */

#include "cJSON.h"

int json_parse(const char *text, void **out_obj)
{
    if (!text || !out_obj) {
        return -1;
    }
    cJSON *obj = cJSON_Parse(text);
    if (!obj) {
        return -1;
    }
    *out_obj = obj;
    return 0;
}

int json_serialize(const void *obj, char *out, size_t max_len)
{
    if (!obj || !out || max_len == 0) {
        return -1;
    }
    char *json_str = cJSON_PrintUnformatted((const cJSON *)obj);
    if (!json_str) {
        return -1;
    }
    strncpy(out, json_str, max_len - 1);
    out[max_len - 1] = '\0';
    cJSON_free(json_str);
    return 0;
}