#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

/*
 * Storage manager stub for SPIFFS or LittleFS.  Handles mounting
 * the filesystem and providing file access helpers.  In this
 * skeleton the functions simply log that they were called.
 */

int storage_mount(void);
int storage_unmount(void);

#endif /* STORAGE_MANAGER_H */