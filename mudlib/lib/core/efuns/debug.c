
#include <mud/secure.h>

static nomask void debug(string type, string message)
{
  object debugger;

  if (!message)
    return;

  if (!strlen(type))
    type = "general";

  ::find_object(DEBUGGER_OB)->log(type, message);

  return;  
}
