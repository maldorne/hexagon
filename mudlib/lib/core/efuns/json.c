
// Thin sefun wrappers around the JSON package.
//
// The actual encoder/decoder lives in /packages/json/. We keep only
// the two entry points here so that the auto object inherits exactly
// two functions, no matter how many private helpers the package
// implementation has.

#define JSON_PACKAGE "/packages/json/master"

static string json_encode(mixed value)
{
  return JSON_PACKAGE->encode(value);
}

static mixed json_decode(string text)
{
  return JSON_PACKAGE->decode(text);
}
