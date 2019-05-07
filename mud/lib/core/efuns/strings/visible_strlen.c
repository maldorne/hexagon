
static int visible_strlen(string str)
{
  int i, result, length;

  result = 0;
  length = strlen(str);

  // if we are not using an ansi terminal... well...
  // visible_strlen won't work
  if (this_user() && this_user()->query_term_name() != "ansi")
    return length;

  for (i = 0; i < length; i++)
  {
    // take a look to /lib/handlers/terminal.c for more info
    // start of ansi control characters
    if (str[i] == 27)
    {
      // advance until the end of the control characters
      while ((str[i] != 'm') && (i < length))
        i++;

      continue;
    }

    result++;
  }

  return result;
}
