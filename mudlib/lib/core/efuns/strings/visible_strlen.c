
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
      while ((i < length) && (str[i] != 'm'))
        i++;

      continue;
    }
    // international characters áÁöû, etc
    else if (str[i] == 195)
    {
      // advance just one character, á == 195 161, é == 195 169, etc
      continue;
    }
    else if (str[i] == 226)
    {
      // advance two characters, ╒ = 226 149 146
      i++;
      continue;
    }

    result++;
  }

  return result;
}

static string char_codes(string str)
{
  int i, length;
  string result;

  result = "";
  length = strlen(str);

  for (i = 0; i < length; i++)
  {
    result += "" + str[i] + " ";
  }

  return result;
}
