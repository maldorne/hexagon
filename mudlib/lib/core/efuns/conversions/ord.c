
// string to char
static int ord(string c)
{
  int tmp;

  if (!c || (strlen(c) == 0))
    return 0;

  if (strlen(c) > 1)
    c = c[0..0];

  sscanf(c, "%c", tmp);
  return tmp;
}
