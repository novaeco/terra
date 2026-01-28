#ifndef WEBSOCKET_H
#define WEBSOCKET_H

/*
 * WebSocket handler stubs.  The full implementation would manage
 * connections, broadcast sensor updates and handle bidirectional
 * communication.  The functions here are placeholders.
 */

int ws_handle_connect(void);
int ws_broadcast(const char *message);
int ws_handle_frame(const char *data, size_t len);

#endif /* WEBSOCKET_H */