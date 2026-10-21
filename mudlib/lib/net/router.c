// HTTP router.  Maps a request path to a handler object that exposes
// handle_request(method, path, headers, body) -> response mapping.
// The response holds "status", "headers" and either "body" or "file" (a
// path the HTTP session streams, for files too big for a string).
//
// The routes are the mapping below, the mudlib's own, plus those a package
// declares: as it adds commands in its cmds/ folder, a package adds routes
// with query_http_routes() in its master (path -> handler file). A route of
// the mudlib, or of a package loaded before, is never replaced.  A key
// ending in "/" is a prefix route: it matches every path under it, the
// longest prefix winning over shorter ones; exact keys win over any prefix.

#include <net/http.h>

private mapping _routes;

/**
 * Adds the routes every loadable package declares in its master.
 */
private void _add_package_routes()
{
  string * names, * paths;
  mapping  declared;
  object   master;
  int      i, j;

  names = get_dir("/packages/*");
  if (names == nil)
    return;

  for (i = 0; i < sizeof(names); i++)
  {
    // a folder without a master is not a loadable package
    if (file_size("/packages/" + names[i] + "/master.c") < 0)
      continue;

    master = package(names[i]);
    if (!master || !function_object("query_http_routes", master))
      continue;

    declared = master->query_http_routes();
    if (!mappingp(declared))
      continue;

    paths = map_indices(declared);
    for (j = 0; j < sizeof(paths); j++)
      if (_routes[paths[j]] == nil && stringp(declared[paths[j]]))
        _routes[paths[j]] = declared[paths[j]];
  }
}

void create()
{
  _routes = ([
    "/status"   : "/lib/net/handlers/status",
  ]);

  _add_package_routes();
}

/**
 * @return request path -> handler file, a copy
 */
mapping query_routes()
{
  return ([ ]) + _routes;
}

/**
 * Finds the handler file for a path: the exact route, else the longest
 * prefix route.
 * @param path request path without query string
 * @return the handler file, or nil when no route matches
 */
private string _find_route(string path)
{
  string * keys;
  string   best;
  int      i, len;

  if (_routes[path] != nil)
    return _routes[path];

  keys = map_indices(_routes);
  best = nil;

  for (i = 0; i < sizeof(keys); i++)
  {
    len = strlen(keys[i]);

    if (len < 2 || keys[i][len - 1] != '/')
      continue;

    if (strlen(path) >= len && path[0..len - 1] == keys[i] &&
        (best == nil || len > strlen(best)))
      best = keys[i];
  }

  return (best == nil ? nil : _routes[best]);
}

/**
 * Dispatches a request to its handler.  The query string (after the
 * first "?") is cut from the path before routing and handed to the
 * handler in the headers mapping under the key ":query" (no real header
 * can start with a colon).
 */
mapping dispatch(string method, string path, mapping headers, string body)
{
  string target, query;
  object handler;
  int    pos;

  query = "";
  for (pos = 0; pos < strlen(path); pos++)
  {
    if (path[pos] == '?')
    {
      query = path[pos + 1..];
      path = (pos > 0 ? path[0..pos - 1] : "");
      break;
    }
  }

  headers[":query"] = query;

  target = _find_route(path);

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
