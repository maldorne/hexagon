
static nomask void debug(string type, string message)
{
  if (!message)
    return;

  if (!type)
    type = "general";

  ::find_object(DRIVER)->debugger()->log(type, message);

  // TODO remove this when debugger is finished
  ::find_object(DRIVER)->_stderr(message);

  return;  
}
