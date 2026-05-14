
// HTTP/1.x connection object.
//
// One instance per accepted TCP connection on HTTP_PORT.  Receives raw
// bytes via the driver's receive_message() callback, accumulates them
// until a full request is in hand, dispatches through the router and
// writes the response.  Connection is closed after each response
// (no keep-alive for now).
//
// Only Content-Length-delimited request bodies are accepted; chunked
// transfer encoding is not implemented.

#include <user/login.h>
#include <mud/secure.h>
#include <net/http.h>
#include <kernel.h>

inherit obj "/lib/core/object.c";

// connection state
#define _STATE_HEADERS 0     // accumulating until \r\n\r\n
#define _STATE_BODY    1     // accumulating until content_length reached
#define _STATE_DONE    2     // response sent, awaiting destruction

static int    _state;
static string _buffer;       // raw bytes received so far
static int    _start_time;
static int    _timestamp;

// parsed request
static string  _method;
static string  _path;
static string  _version;
static mapping _headers;
static int     _content_length;
static int     _headers_end;       // offset of body start (after \r\n\r\n)

private void _reset_request()
{
  _state          = _STATE_HEADERS;
  _buffer         = "";
  _method         = nil;
  _path           = nil;
  _version        = nil;
  _headers        = ([ ]);
  _content_length = 0;
  _headers_end    = 0;
}

void create()
{
  obj::create();

  seteuid(NETWORK_EUID);

  _start_time = time();
  _timestamp  = _start_time;
  _reset_request();
}

// pin the connection in place; the obj base wants a location, so route
// it to the same lounge the binary connection uses.
int move(mixed dest, varargs mixed messin, mixed messout)
{
  return obj::move(LOGIN_LOUNGE, messin, messout);
}

nomask int query_link() { return 0; }
nomask int query_player() { return 0; }
nomask int query_user() { return 0; }
nomask object user() { return this_object(); }
nomask int query_timestamp() { return _timestamp; }
nomask int query_start_time() { return _start_time; }
nomask int query_idle() { return time() - _timestamp; }

nomask void save_me() {}
nomask int restore_me(varargs string whatever) { return 0; }

void dest_me()
{
  if (clonep(this_object()))
    find_object(BINARY_HANDLER)->remove_connection(this_object());

  obj::dest_me();
}

// ---------------------------------------------------------------------
// request parsing
// ---------------------------------------------------------------------

// case-insensitive header lookup helper
static string query_header(string name)
{
  return _headers[lower_case(name)];
}

private int _parse_headers()
{
  int    end, i, j, colon;
  string head, line, key, value, request_line;
  string * lines;
  string * parts;

  // look for \r\n\r\n (end of headers)
  end = -1;
  for (i = 0; i + 3 < ::strlen(_buffer); i++)
  {
    if (_buffer[i]     == '\r' && _buffer[i + 1] == '\n' &&
        _buffer[i + 2] == '\r' && _buffer[i + 3] == '\n')
    {
      end = i;
      break;
    }
  }

  if (end == -1)
  {
    // still incomplete — reject if oversized
    if (::strlen(_buffer) > HTTP_MAX_REQUEST)
      return -1;
    return 0;
  }

  head = _buffer[0..end - 1];
  _headers_end = end + 4;

  lines = full_explode(head, "\r\n");
  if (sizeof(lines) < 1)
    return -1;

  // request line: METHOD PATH HTTP/x.y
  request_line = lines[0];
  parts = full_explode(request_line, " ");
  if (sizeof(parts) < 3)
    return -1;

  _method  = parts[0];
  _path    = parts[1];
  _version = parts[2];

  // headers
  for (i = 1; i < sizeof(lines); i++)
  {
    line = lines[i];
    if (::strlen(line) == 0)
      continue;

    colon = -1;
    for (j = 0; j < ::strlen(line); j++)
      if (line[j] == ':') { colon = j; break; }

    if (colon == -1)
      return -1;

    key = lower_case(trim(line[0..colon - 1]));
    value = (colon + 1 < ::strlen(line) ? trim(line[colon + 1..]) : "");
    _headers[key] = value;
  }

  if (_headers["content-length"] != nil)
  {
    if (sscanf(_headers["content-length"], "%d", _content_length) != 1 ||
        _content_length < 0)
      return -1;
  }

  return 1;
}

// ---------------------------------------------------------------------
// response writing
// ---------------------------------------------------------------------

private string _status_text(int code)
{
  switch (code)
  {
    case 200: return "OK";
    case 204: return "No Content";
    case 400: return "Bad Request";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 413: return "Payload Too Large";
    case 500: return "Internal Server Error";
    default:  return "Unknown";
  }
}

private void _send_response(mapping response)
{
  int       status;
  string    body, message;
  mapping   headers;
  mixed *   keys;
  int       i;

  status  = response["status"];
  body    = response["body"]    != nil ? response["body"]    : "";
  headers = response["headers"] != nil ? response["headers"] : ([ ]);

  // always set Content-Length and Connection: close for HTTP/1.0-style
  headers["Content-Length"] = (string)::strlen(body);
  headers["Connection"]     = "close";

  if (HTTP_CORS_ENABLE)
  {
    headers["Access-Control-Allow-Origin"]  = HTTP_CORS_ORIGIN;
    headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
    headers["Access-Control-Allow-Headers"] = "Content-Type";
  }

  message = "HTTP/1.1 " + status + " " + _status_text(status) + "\r\n";

  keys = map_indices(headers);
  for (i = 0; i < sizeof(keys); i++)
    message += keys[i] + ": " + headers[keys[i]] + "\r\n";

  message += "\r\n" + body;

  send_message(message);
}

private void _handle_complete_request()
{
  mapping  response;
  string   body;

  body = (_content_length > 0
          ? _buffer[_headers_end..(_headers_end + _content_length - 1)]
          : "");

  // CORS preflight short-circuit
  if (HTTP_CORS_ENABLE && _method == "OPTIONS")
  {
    response = ([
      "status":  204,
      "headers": ([ ]),
      "body":    "",
    ]);
  }
  else
  {
    response = HTTP_ROUTER->dispatch(_method, _path, _headers, body);
  }

  _send_response(response);
  _state = _STATE_DONE;
  destruct_object(this_object());
}

// ---------------------------------------------------------------------
// driver callbacks
// ---------------------------------------------------------------------

static void open()
{
  _timestamp = time();
}

static void close(int flag) {}

void send_message(string str)
{
  if (str == nil)
    return;
  ::send_message(str);
}

static void receive_message(string str)
{
  rlimits (HTTP_MAX_DEPTH; HTTP_MAX_TICKS)
  {
    int parsed;

    _timestamp = time();

    if (_state == _STATE_DONE)
      return;

    _buffer += str;

    if (::strlen(_buffer) > HTTP_MAX_REQUEST)
    {
      _send_response(([ "status":  413,
                        "headers": ([ "Content-Type" : "text/plain" ]),
                        "body":    "request too large" ]));
      _state = _STATE_DONE;
      destruct_object(this_object());
      return;
    }

    if (_state == _STATE_HEADERS)
    {
      parsed = _parse_headers();
      if (parsed == -1)
      {
        _send_response(([ "status":  400,
                          "headers": ([ "Content-Type" : "text/plain" ]),
                          "body":    "bad request" ]));
        _state = _STATE_DONE;
        destruct_object(this_object());
        return;
      }
      if (parsed == 0)
        return;  // wait for more bytes

      _state = _STATE_BODY;
    }

    if (_state == _STATE_BODY)
    {
      if (::strlen(_buffer) - _headers_end < _content_length)
        return;  // still waiting for body
      _handle_complete_request();
    }
  }
}

mixed * stats()
{
  return ({
    ({ "State (nosave)",     _state, }),
    ({ "Buffered (nosave)",  ::strlen(_buffer), }),
    ({ "Method (nosave)",    _method != nil ? _method : "<unparsed>", }),
    ({ "Path (nosave)",      _path   != nil ? _path   : "<unparsed>", }),
  }) + obj::stats();
}
