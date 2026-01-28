#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

int file_save(const char *path, const void *data, size_t size);
int file_load(const char *path, void *buffer, size_t max_size);

#endif /* FILE_MANAGER_H */