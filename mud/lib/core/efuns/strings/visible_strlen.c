
static int visible_strlen(string str)
{
  int i, result;

  result = 0;

  for (i = 0; i < strlen(str); i++)
  {
    // if (((str[i] <= 97) && (str[i] < 122)) ||
    //     ((str[i] <= 65) && (str[i] < 90)) )
    if (str[i] >= 32)
      result++;
  }

  return result;
}
