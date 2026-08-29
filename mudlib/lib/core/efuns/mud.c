
#include <mud/config.h>

static nomask string mud_name() 
{
  return MUD_NAME;
}

static nomask string version() 
{ 
  return status()[0];
}

// The language this mudlib was compiled in. The driver picks every
// .lang.<lang>.h include from it; a data file that carries text and cannot be
// preprocessed (a template .json) names the language in its own filename
// instead, and whoever opens it reads which sibling to ask for from here.
static nomask string mud_language()
{
  return GLOBAL_COMPILE_LANG;
}

static nomask void shutdown(varargs int hotboot)
{
  stderr(" - shutdown(" + (hotboot ? hotboot : "") + ")\n");
  ::shutdown(hotboot);
}