#include <stdio.h>
#include <string.h>
#include "file_manager.h"

/*
 * File manager implementation.
 *
 * Saves and loads arbitrary binary data to and from the filesystem
 * using standard C file I/O.  Returns 0 on success and -1 on
 * failure.  Note: this implementation does not create missing
 * directories.
 */

int file_save(const char *path, const void *data, size_t size)
{
    if (!path || !data || size == 0) {
        return -1;
    }
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("file_save fopen");
        return -1;
    }
    size_t written = fwrite(data, 1, size, f);
    fclose(f);
    return (written == size) ? 0 : -1;
}

int file_load(const char *path, void *buffer, size_t max_size)
{
    if (!path || !buffer || max_size == 0) {
        return -1;
    }
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("file_load fopen");
        return -1;
    }
    size_t read = fread(buffer, 1, max_size, f);
    fclose(f);
    return (int)read;
}