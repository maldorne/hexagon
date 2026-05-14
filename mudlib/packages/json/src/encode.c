
// json_encode — serialize an LPC value to a JSON string.
//
// Type mapping:
//   nil           -> null
//   int           -> number (integer)
//   float         -> number
//   string        -> string (with JSON escape rules)
//   mixed *       -> array
//   mapping       -> object   (keys MUST be strings)
//   object        -> not supported, raises an error
//
// LPC has no native boolean: integers 0/1 are encoded as numbers, not
// as JSON false/true. If a caller wants real JSON booleans on the wire
// it must build the structure with literal "true"/"false" strings and
// pass them in already serialized.

#include <type.h>

private string _json_encode_string(string str);
private string _json_encode_value(mixed value);

private string _json_encode_string(string str)
{
  int i, len, c;
  string result;

  result = "\"";
  len = ::strlen(str);

  for (i = 0; i < len; i++)
  {
    c = str[i];

    switch (c)
    {
      case '"':  result += "\\\""; break;
      case '\\': result += "\\\\"; break;
      case '\b': result += "\\b"; break;
      case '\f': result += "\\f"; break;
      case '\n': result += "\\n"; break;
      case '\r': result += "\\r"; break;
      case '\t': result += "\\t"; break;
      default:
        if (c < 0x20)
          result += sprintf("\\u%04x", c);
        else
          // pass UTF-8 bytes through verbatim — they are already
          // valid in a JSON string
          result += str[i..i];
    }
  }

  return result + "\"";
}

private string _json_encode_value(mixed value)
{
  int t;

  t = typeof(value);

  switch (t)
  {
    case T_NIL:
      return "null";

    case T_INT:
      return (string)value;

    case T_FLOAT:
      // %g gives the shortest accurate representation
      return sprintf("%g", value);

    case T_STRING:
      return _json_encode_string(value);

    case T_ARRAY:
    {
      string * parts;
      int i, sz;

      sz = sizeof(value);
      parts = allocate(sz);

      for (i = 0; i < sz; i++)
        parts[i] = _json_encode_value(value[i]);

      return "[" + implode(parts, ",") + "]";
    }

    case T_MAPPING:
    {
      mixed * keys;
      string * parts;
      int i, sz;

      keys = map_indices(value);
      sz = sizeof(keys);
      parts = allocate(sz);

      for (i = 0; i < sz; i++)
      {
        if (typeof(keys[i]) != T_STRING)
          error("json_encode: mapping keys must be strings");

        parts[i] = _json_encode_string(keys[i]) + ":" +
                   _json_encode_value(value[keys[i]]);
      }

      return "{" + implode(parts, ",") + "}";
    }

    case T_OBJECT:
      // matches the documented limitation in
      // notes/lpc/dgd/writing-dgd-extensions.md §7.2: the mudlib must
      // flatten objects to a public identifier before encoding.
      error("json_encode: cannot encode object value; " +
            "flatten to a string id first");

    default:
      error("json_encode: unsupported value type " + t);
  }
}

string encode(mixed value)
{
  return _json_encode_value(value);
}
