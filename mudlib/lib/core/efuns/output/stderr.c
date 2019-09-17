
static nomask void stderr(string str)
{
  if (!str)
    return;

  ::find_object(DRIVER)->_stderr(str);

  return;  
}
