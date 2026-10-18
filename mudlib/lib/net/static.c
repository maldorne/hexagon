// Helpers for HTTP handlers that serve files from disk.  Inherit it and
// call serve_file() with a path that was built from checked segments.

/**
 * Builds a plain-text error response.
 * @param status HTTP status code
 * @param message body of the response
 * @return response mapping in the shape the router expects
 */
mapping text_response(int status, string message)
{
  return ([
    "status":  status,
    "headers": ([ "Content-Type" : "text/plain; charset=utf-8" ]),
    "body":    message,
  ]);
}

/**
 * Content type for a file name, from its extension.
 * @param name file name or path
 * @return the media type; application/octet-stream when unknown
 */
string file_content_type(string name)
{
  int    i, dot;
  string ext;

  dot = -1;
  for (i = strlen(name) - 1; i >= 0; i--)
  {
    if (name[i] == '/')
      break;
    if (name[i] == '.')
    {
      dot = i;
      break;
    }
  }

  if (dot == -1)
    return "application/octet-stream";

  ext = lower_case(name[dot + 1..]);

  switch (ext)
  {
    case "html": return "text/html; charset=utf-8";
    case "csv":  return "text/csv; charset=utf-8";
    case "js":   return "application/javascript";
    case "css":  return "text/css";
    case "json": return "application/json";
    case "txt":  return "text/plain; charset=utf-8";
    default:     return "application/octet-stream";
  }
}

/**
 * Whether every path segment is safe to use under a fixed root: none is
 * empty, "..", or hidden (starts with a dot).
 * @param segments the path split on "/", without the root
 */
int safe_segments(string * segments)
{
  int i;

  if (!sizeof(segments))
    return 0;

  for (i = 0; i < sizeof(segments); i++)
  {
    if (!strlen(segments[i]) || segments[i][0] == '.')
      return 0;
  }

  return 1;
}

/**
 * Reads a file and wraps it in a response.
 * @param file absolute path, already checked by the caller
 * @param extra_headers headers added to the response (may be nil)
 * @return 200 with the file, or 404 when it does not exist
 */
mapping serve_file(string file, varargs mapping extra_headers)
{
  mapping headers;

  if (file_size(file) < 0)
    return text_response(404, "not found");

  headers = ([ "Content-Type" : file_content_type(file) ]);
  if (extra_headers != nil)
    headers += extra_headers;

  // the connection streams the file: it may be bigger than a string
  return ([ "status": 200, "headers": headers, "file": file ]);
}
