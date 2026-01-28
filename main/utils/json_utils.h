#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stddef.h>

/*
 * Helper functions for working with JSON.  In the real project
 * cJSON or another library would be used.  These functions are
 * placeholders that do nothing.
 */
int json_parse(const char *text, void **out_obj);
int json_serialize(const void *obj, char *out, size_t max_len);

#endif /* JSON_UTILS_H */