#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

int nvs_init(void);
int nvsman_get_str(const char *key, char *value, size_t max_len);
int nvsman_set_str(const char *key, const char *value);

#endif /* NVS_MANAGER_H */
