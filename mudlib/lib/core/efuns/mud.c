
#include <mud/config.h>

static nomask string mud_name() 
{
  return MUD_NAME;
}

static nomask string version() 
{ 
  return status()[0];
}

static nomask void shutdown(varargs int hotboot)
{
  stderr(" - shutdown(" + (hotboot ? hotboot : "") + ")\n");
  ::shutdown(hotboot);
}