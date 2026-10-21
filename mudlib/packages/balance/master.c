// balance: dumps the numbers that balance a game to CSV, and serves a viewer
// of them over HTTP. See readme.md.

inherit "/packages/balance/src/balance.c";

/**
 * The HTTP routes this package adds to the mud's server.
 *
 * @return request path -> handler file; a path ending in "/" is a prefix
 */
mapping query_http_routes()
{
  return ([
    "/balance"  : "/packages/balance/net/balance.c",
    "/balance/" : "/packages/balance/net/balance.c",
  ]);
}
