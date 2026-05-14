
// json_decode — parse a JSON string into LPC values.
//
// Inverse mapping of json_encode (see encode.c):
//   null          -> nil
//   true / false  -> 1 / 0   (LPC has no native boolean)
//   number        -> int if no fraction or exponent, otherwise float
//   string        -> string (with JSON unescape, \uXXXX limited to BMP)
//   array         -> mixed *
//   object        -> mapping with string keys
//
// Parser style is a straightforward recursive descent. State (the
// current offset) is threaded through the call chain as ({ value, pos })
// tuples to keep the parser reentrant.

#include <type.h>

private mixed * _json_parse_value(string str, int pos);

private int _json_skip_ws(string str, int pos)
{
  int len, c;

  len = ::strlen(str);

  while (pos < len)
  {
    c = str[pos];
    if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
      break;
    pos++;
  }

  return pos;
}

private int _json_hex(int c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  error("json_decode: invalid hex digit in \\u escape");
}

private string _json_encode_utf8(int cp)
{
  // encode a single code point as UTF-8 (1-3 bytes for BMP)
  if (cp < 0x80)
    return chr(cp);                    // chr lives in conversions.c
  if (cp < 0x800)
    return chr(0xC0 | (cp >> 6)) +
           chr(0x80 | (cp & 0x3F));
  return chr(0xE0 | (cp >> 12)) +
         chr(0x80 | ((cp >> 6) & 0x3F)) +
         chr(0x80 | (cp & 0x3F));
}

private mixed * _json_parse_string(string str, int pos)
{
  int len, c, cp, start;
  string result;

  len = ::strlen(str);

  if (pos >= len || str[pos] != '"')
    error("json_decode: expected '\"'");
  pos++;

  result = "";

  while (pos < len)
  {
    c = str[pos];

    if (c == '"')
      return ({ result, pos + 1 });

    if (c == '\\')
    {
      pos++;
      if (pos >= len)
        error("json_decode: unterminated escape");
      c = str[pos];

      switch (c)
      {
        case '"':  result += "\""; pos++; break;
        case '\\': result += "\\"; pos++; break;
        case '/':  result += "/";  pos++; break;
        case 'b':  result += "\b"; pos++; break;
        case 'f':  result += "\f"; pos++; break;
        case 'n':  result += "\n"; pos++; break;
        case 'r':  result += "\r"; pos++; break;
        case 't':  result += "\t"; pos++; break;
        case 'u':
          if (pos + 4 >= len)
            error("json_decode: truncated \\u escape");
          cp = (_json_hex(str[pos + 1]) << 12) |
               (_json_hex(str[pos + 2]) << 8) |
               (_json_hex(str[pos + 3]) << 4) |
               (_json_hex(str[pos + 4]));
          result += _json_encode_utf8(cp);
          pos += 5;
          break;
        default:
          error("json_decode: unknown escape \\" + chr(c));
      }
    }
    else
    {
      // copy a run of literal bytes for speed
      start = pos;
      while (pos < len && str[pos] != '"' && str[pos] != '\\')
        pos++;
      result += str[start..pos - 1];
    }
  }

  error("json_decode: unterminated string");
}

private mixed * _json_parse_number(string str, int pos)
{
  int len, start, c, is_float, val_i;
  float val_f;
  string text;

  len = ::strlen(str);
  start = pos;
  is_float = 0;

  if (pos < len && str[pos] == '-')
    pos++;

  while (pos < len)
  {
    c = str[pos];
    if (c >= '0' && c <= '9')
    {
      pos++;
    }
    else if (c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')
    {
      is_float = 1;
      pos++;
    }
    else
      break;
  }

  if (pos == start)
    error("json_decode: expected number");

  text = str[start..pos - 1];

  if (is_float)
  {
    if (sscanf(text, "%f", val_f) != 1)
      error("json_decode: invalid number '" + text + "'");
    return ({ val_f, pos });
  }
  else
  {
    if (sscanf(text, "%d", val_i) != 1)
      error("json_decode: invalid number '" + text + "'");
    return ({ val_i, pos });
  }
}

private mixed * _json_parse_array(string str, int pos)
{
  mixed * result;
  mixed * tmp;
  int len;

  len = ::strlen(str);
  result = ({ });

  pos++;  // consume '['
  pos = _json_skip_ws(str, pos);

  if (pos < len && str[pos] == ']')
    return ({ result, pos + 1 });

  while (1)
  {
    tmp = _json_parse_value(str, pos);
    result += ({ tmp[0] });
    pos = _json_skip_ws(str, tmp[1]);

    if (pos >= len)
      error("json_decode: unterminated array");

    if (str[pos] == ',')
    {
      pos = _json_skip_ws(str, pos + 1);
      continue;
    }

    if (str[pos] == ']')
      return ({ result, pos + 1 });

    error("json_decode: expected ',' or ']' in array");
  }
}

private mixed * _json_parse_object(string str, int pos)
{
  mapping result;
  mixed * tmp;
  string key;
  int len;

  len = ::strlen(str);
  result = ([ ]);

  pos++;  // consume '{'
  pos = _json_skip_ws(str, pos);

  if (pos < len && str[pos] == '}')
    return ({ result, pos + 1 });

  while (1)
  {
    tmp = _json_parse_string(str, pos);
    key = tmp[0];
    pos = _json_skip_ws(str, tmp[1]);

    if (pos >= len || str[pos] != ':')
      error("json_decode: expected ':' after object key");
    pos = _json_skip_ws(str, pos + 1);

    tmp = _json_parse_value(str, pos);
    result[key] = tmp[0];
    pos = _json_skip_ws(str, tmp[1]);

    if (pos >= len)
      error("json_decode: unterminated object");

    if (str[pos] == ',')
    {
      pos = _json_skip_ws(str, pos + 1);
      continue;
    }

    if (str[pos] == '}')
      return ({ result, pos + 1 });

    error("json_decode: expected ',' or '}' in object");
  }
}

private mixed * _json_parse_literal(string str, int pos, string lit, mixed value)
{
  int litlen;

  litlen = ::strlen(lit);

  if (pos + litlen > ::strlen(str) || str[pos..pos + litlen - 1] != lit)
    error("json_decode: expected '" + lit + "'");

  return ({ value, pos + litlen });
}

private mixed * _json_parse_value(string str, int pos)
{
  int c;

  pos = _json_skip_ws(str, pos);

  if (pos >= ::strlen(str))
    error("json_decode: unexpected end of input");

  c = str[pos];

  switch (c)
  {
    case '{': return _json_parse_object(str, pos);
    case '[': return _json_parse_array(str, pos);
    case '"': return _json_parse_string(str, pos);
    case 't': return _json_parse_literal(str, pos, "true", 1);
    case 'f': return _json_parse_literal(str, pos, "false", 0);
    case 'n': return _json_parse_literal(str, pos, "null", nil);
  }

  if (c == '-' || (c >= '0' && c <= '9'))
    return _json_parse_number(str, pos);

  error("json_decode: unexpected character '" + chr(c) + "'");
}

mixed decode(string str)
{
  mixed * tmp;
  int pos;

  if (str == nil)
    error("json_decode: nil input");

  tmp = _json_parse_value(str, 0);
  pos = _json_skip_ws(str, tmp[1]);

  if (pos != ::strlen(str))
    error("json_decode: trailing garbage at offset " + pos);

  return tmp[0];
}
