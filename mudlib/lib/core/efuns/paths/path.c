
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
