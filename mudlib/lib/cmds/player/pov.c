#include <mud/cmd.h>
#include <common/properties.h>
#include <language.h>
#include <translations/pov.h>
#include <translations/common.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_POV_ALIAS);
  set_usage(_LANG_CMD_POV_SYNTAX);
  set_help(_LANG_CMD_POV_HELP);
}

static int cmd(string str, object me, string verb) 
{
  object user;
  user = me->user();

  if (!strlen(str) || (member_array(str, POV_TYPES) == -1))
	{
		write(_LANG_SYNTAX + ": " + _LANG_CMD_POV_ALIAS[0] + " < "+implode( POV_TYPES, " | ")+" >\n" + 
						"    " + _LANG_CMD_POV_CURRENT + POV_TYPES[user->query_pov()] + "\n");
		return 1;
	}
	
  // set the pov as the index in the array
	user->set_pov(member_array(str, POV_TYPES));
	write(_LANG_CMD_POV_MSG);

	me->set_trivial_action();
  return 1;
}

