#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

/*
 * Wi‑Fi manager module.
 *
 * Responsibilities (from architecture):
 *  - Manage station and access point modes
 *  - Handle auto‑reconnection and provisioning
 *  - Persist configuration to NVS
 *
 * These functions wrap the ESP‑IDF Wi‑Fi APIs to provide a simple
 * interface for initialising the network stack, connecting to a
 * configured access point and starting an access point for
 * provisioning.  The implementation is in wifi_manager.c.
 */

int wifi_init(void);
int wifi_connect(void);
int wifi_start_ap(void);

#endif /* WIFI_MANAGER_H */