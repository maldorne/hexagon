
// HTTP router.  Maps a request path to a handler object that exposes
// handle_request(method, path, headers, body) -> response mapping.
//
// The mapping below is the single source of truth for which endpoints
// exist; adding one is: write the handler under /lib/net/handlers/,
// then add an entry here.

#include <net/http.h>

private mapping _routes;

void create()
{
  _routes = ([
    "/status" : "/lib/net/handlers/status",
  ]);
}

mapping dispatch(string method, string path, mapping headers, string body)
{
  string target;
  object handler;

  target = _routes[path];

  if (target == nil)
    return ([
      "status":  404,
      "headers": ([ "Content-Type" : "text/plain" ]),
      "body":    "not found",
    ]);

  handler = find_object(target);
  if (handler == nil)
    handler = compile_object(target);

  return handler->handle_request(method, path, headers, body);
}
