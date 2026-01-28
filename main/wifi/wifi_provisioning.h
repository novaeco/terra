#ifndef WIFI_PROVISIONING_H
#define WIFI_PROVISIONING_H

/*
 * Provisioning interface for configuring Wi‑Fi credentials via a
 * captive portal.  In a real project this module would start a
 * lightweight HTTP server to collect SSID and password from the
 * user.  For this skeleton the functions are placeholders.
 */

int prov_start(void);
int prov_handle_web(void);
int prov_complete(void);

#endif /* WIFI_PROVISIONING_H */