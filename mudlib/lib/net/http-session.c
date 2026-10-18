// HTTP/1.x session.
//
// One instance per HTTP connection.  It is not the driver-level
// connection: the binary front (BINARY_OB) owns that, detects the
// protocol and creates this session, then forwards the bytes it
// receives.  The session accumulates them until a full request is in hand,
// dispatches through the router and writes the response through the front.
// The connection is closed after each response (no keep-alive for now).
//
// Only Content-Length-delimited request bodies are accepted; chunked
// transfer encoding is not implemented.

#include <mud/secure.h>
#include <net/http.h>

// connection state
#define _STATE_HEADERS 0     // accumulating until \r\n\r\n
#define _STATE_BODY    1     // accumulating until content_length reached
#define _STATE_DONE    2     // response sent, awaiting destruction

static int    _state;
static string _buffer;       // raw bytes received so far
static int    _start_time;
static int    _timestamp;
static object _front;        // the connection that owns this session
static string _outgoing;     // response bytes the driver has not taken yet
static string _file;         // file streamed as the body, nil when none
static int    _file_offset;  // bytes of it already read
static int    _file_size;

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
  _outgoing       = "";
  _file           = nil;
  _file_offset    = 0;
  _file_size      = 0;
}

void create()
{
  seteuid(NETWORK_EUID);

  _start_time = time();
  _timestamp  = _start_time;
  _reset_request();
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
    case 301: return "Moved Permanently";
    case 302: return "Found";
    case 400: return "Bad Request";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 413: return "Payload Too Large";
    case 500: return "Internal Server Error";
    default:  return "Unknown";
  }
}

/**
 * Hands the pending response bytes to the front and closes the connection
 * when none are left.
 */
private void _flush_response()
{
  int sent;

  // a file body is read in chunks, since a string cannot hold a big file
  if (::strlen(_outgoing) == 0 && _file != nil && _file_offset < _file_size)
  {
    _outgoing = read_file(_file, _file_offset, HTTP_FILE_CHUNK);
    if (_outgoing == nil)
      _outgoing = "";
    _file_offset += HTTP_FILE_CHUNK;
  }

  if (::strlen(_outgoing) == 0)
  {
    _front->end_connection();
    return;
  }

  // send returns how many bytes the driver accepted; the front calls
  // message_done() once the driver buffer has emptied
  sent = _front->send(_outgoing);
  _outgoing = (sent >= ::strlen(_outgoing) ? "" : _outgoing[sent..]);
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

  // a response with "file" sends that file as its body instead of "body"
  if (response["file"] != nil)
  {
    _file      = response["file"];
    _file_size = file_size(_file);
    body       = "";
  }

  // always set Content-Length and Connection: close for HTTP/1.0-style
  headers["Content-Length"] = (string)(_file != nil ? _file_size : ::strlen(body));
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

  // the whole response may not fit in the output buffer: it is written in
  // pieces and the connection is closed once message_done() finds
  // nothing left (Content-Length is a byte count, DGD strings are bytes)
  _state    = _STATE_DONE;
  _outgoing = message;
  _flush_response();
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
}

// ---------------------------------------------------------------------
// entry points, only the front may call them
// ---------------------------------------------------------------------

void receive(string str);

/**
 * Binds the session to its front and feeds it the bytes the front had
 * already buffered.  Only the first caller becomes the front.
 */
void start(object front, string data)
{
  if (_front != nil || front == nil || previous_object() != front)
    return;

  _front     = front;
  _timestamp = time();

  receive(data);
}

/** Called by the front when the driver has emptied its output buffer. */
void message_done()
{
  if (previous_object() != _front || _front == nil)
    return;

  if (_state == _STATE_DONE)
    _flush_response();
}

/** Called by the front with every chunk of bytes the client sends. */
void receive(string str)
{
  if (previous_object() != _front || _front == nil)
    return;

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
