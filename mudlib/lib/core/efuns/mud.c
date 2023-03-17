
#include <mud/config.h>

static nomask string mud_name() 
{
  return MUD_NAME;
}

static nomask string version() 
{ 
  return status()[0];
}
