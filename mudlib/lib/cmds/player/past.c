
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
	position = 0;
}

string query_usage()
{
	return "past";
}

string query_short_help()
{
	return "Te muestra lo que te hayan dicho últimamente.";
}

static int cmd(string name, object me, string verb)
{
  me->print_past_g();
  me->set_trivial_action();
  return 1;
}
