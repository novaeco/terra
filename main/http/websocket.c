#include <stdio.h>
#include <stddef.h>
#include "websocket.h"

int ws_handle_connect(void)
{
    printf("[ws] handle connect stub called\n");
    return 0;
}

int ws_broadcast(const char *message)
{
    (void)message;
    printf("[ws] broadcast stub called\n");
    return 0;
}

int ws_handle_frame(const char *data, size_t len)
{
    (void)data;
    (void)len;
    printf("[ws] handle frame stub called\n");
    return 0;
}