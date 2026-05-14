
// Handler for GET /status — basic health/info payload.
//
// Returns a mapping with the response shape the router expects:
//   ([ "status":  <int>,
//      "headers": ([ "Header-Name" : "value", ... ]),
//      "body":    <string> ])

#include <net/http.h>
#include <mud/config.h>
#include <status.h>

mapping handle_request(string method, string path, mapping headers, string body)
{
  mapping payload;

  if (method != "GET")
    return ([ "status":  405,
              "headers": ([ "Content-Type" : "text/plain",
                            "Allow"        : "GET" ]),
              "body":    "method not allowed" ]);

  payload = ([
    "mud":         MUD_NAME,
    "uptime":      time() - status()[ST_STARTTIME],
    "user_count":  sizeof(users()),
    "now":         time(),
  ]);

  return ([
    "status":  200,
    "headers": ([ "Content-Type" : "application/json; charset=utf-8" ]),
    "body":    json_encode(payload),
  ]);
}
