
#ifndef _HTTP_H
#define _HTTP_H

#define HTTP_OB             "/lib/net/http_connection"
#define HTTP_ROUTER         "/lib/net/router"

#define HTTP_PORT           8080

// hard caps for an in-flight request
#define HTTP_MAX_REQUEST    65536     // bytes accepted in a single request
#define HTTP_REQUEST_TIMEOUT 10       // seconds before an idle connection is dropped

// CORS. When HTTP_CORS_ENABLE is non-zero, the configured origin is
// emitted in Access-Control-Allow-Origin on every response (and the
// preflight OPTIONS is answered). Off by default during initial tests;
// flip to 1 (and adjust HTTP_CORS_ORIGIN) once a browser client needs it.
#define HTTP_CORS_ENABLE    0
#define HTTP_CORS_ORIGIN    "*"

// rlimits applied while a request is being processed
#define HTTP_MAX_DEPTH      100
#define HTTP_MAX_TICKS      10000000

#endif
