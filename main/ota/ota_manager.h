#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

int ota_init(void);
int ota_update_from_url(const char *url);

#endif /* OTA_MANAGER_H */