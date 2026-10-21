// Handler for the balance viewer, served under /balance:
//
//   /balance                    index of the games that have balance data
//   /balance/<game>             redirect to /balance/<game>/
//   /balance/<game>/            the viewer page (www/balance.html)
//   /balance/<game>/files.json  names of the csv files of the game
//   /balance/<game>/<name>.csv  one csv file
//
// The csv files live in /games/<game>/docs/balance/.

#include <net/http.h>

inherit "/lib/net/static";

#define BALANCE_PAGE "/packages/balance/www/balance.html"

private string _balance_dir(string game)
{
  return "/games/" + game + "/docs/balance/";
}

// a game name is a plain directory name: letters, digits and hyphens
private int _valid_game_name(string game)
{
  int i;
  int c;

  if (!strlen(game))
    return 0;

  for (i = 0; i < strlen(game); i++)
  {
    c = game[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == '-'))
      return 0;
  }

  return 1;
}

// a game has balance data when its docs/balance directory has entries
private int _has_balance(string game)
{
  return _valid_game_name(game) && sizeof(get_dir(_balance_dir(game) + "*")) > 0;
}

private string * _csv_files(string game)
{
  string * files;

  files = get_dir(_balance_dir(game) + "*.csv");
  return (files == nil ? ({ }) : files);
}

private mapping _index_page()
{
  string * games;
  string   body;
  int      i;

  games = get_dir("/games/*");
  if (games == nil)
    games = ({ });

  body = "<!doctype html>\n<html><head><meta charset=\"utf-8\">" +
         "<title>Balance</title></head><body>\n<h1>Balance</h1>\n<ul>\n";

  for (i = 0; i < sizeof(games); i++)
  {
    if (_has_balance(games[i]))
      body += "<li><a href=\"/balance/" + games[i] + "/\">" + games[i] +
              "</a></li>\n";
  }

  body += "</ul>\n</body></html>\n";

  return ([
    "status":  200,
    "headers": ([ "Content-Type" : "text/html; charset=utf-8" ]),
    "body":    body,
  ]);
}

mapping handle_request(string method, string path, mapping headers, string body)
{
  string * segments;
  string   game, name;
  int      trailing;

  if (method != "GET")
    return ([ "status":  405,
              "headers": ([ "Content-Type" : "text/plain",
                            "Allow"        : "GET" ]),
              "body":    "method not allowed" ]);

  trailing = (strlen(path) > 0 && path[strlen(path) - 1] == '/');

  // "/balance/a//b" is refused: empty segments are never valid
  segments = explode(path, "/");
  if (sizeof(segments) && segments[0] == "")
    segments = segments[1..];
  if (trailing && sizeof(segments) && segments[sizeof(segments) - 1] == "")
    segments = segments[0..sizeof(segments) - 2];

  if (!sizeof(segments) || segments[0] != "balance")
    return text_response(404, "not found");

  if (sizeof(segments) == 1)
    return _index_page();

  if (!safe_segments(segments[1..]))
    return text_response(403, "forbidden");

  game = segments[1];
  if (!_has_balance(game))
    return text_response(404, "not found");

  if (sizeof(segments) == 2)
  {
    if (!trailing)
      return ([ "status":  301,
                "headers": ([ "Location"     : "/balance/" + game + "/",
                              "Content-Type" : "text/plain" ]),
                "body":    "moved" ]);

    return serve_file(BALANCE_PAGE, ([ "Cache-Control" : "no-cache" ]));
  }

  if (sizeof(segments) != 3 || trailing)
    return text_response(404, "not found");

  name = segments[2];

  if (name == "files.json")
    return ([
      "status":  200,
      "headers": ([ "Content-Type"  : "application/json; charset=utf-8",
                    "Cache-Control" : "no-cache" ]),
      "body":    json_encode(_csv_files(game)),
    ]);

  if (strlen(name) > 4 && name[strlen(name) - 4..] == ".csv")
    return serve_file(_balance_dir(game) + name,
                      ([ "Cache-Control" : "no-cache" ]));

  return text_response(404, "not found");
}
