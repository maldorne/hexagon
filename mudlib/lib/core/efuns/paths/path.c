
/**
 * Returns the directory portion of a full path (the "dirname"), always
 * with a leading `/` and a trailing `/`.
 *
 *   "/a/b/c.c" -> "/a/b/"
 *   "/foo"     -> "/foo/" (no `/` separator inside, single segment)
 *   ""         -> ""
 *
 * Pair with `get_path_file_name` for the basename half.
 *
 * Note: the name `path` is generic and shadows any local `string path`
 * variable in code that uses it; bear that in mind when reading callers.
 */
static string path(string full_path)
{
  string * words;
  words = explode(full_path, "/");

  if (sizeof(words) == 0)
    return "";

  if (sizeof(words) == 1)
    return "/" + words[0] + "/";

  return "/" + implode(words[0..sizeof(words)-2], "/") + "/";
}
