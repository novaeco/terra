#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

/*
 * HTTP/HTTPS server stub.
 *
 * The real server would register route handlers for the REST API
 * endpoints defined in the specification.  It would also enable
 * optional TLS, JWT middleware, CORS and rate limiting.  Here we
 * expose a single function to start the server.
 */

int http_server_start(void);

#endif /* HTTP_SERVER_H */