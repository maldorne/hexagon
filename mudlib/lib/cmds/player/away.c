
#include <mud/cmd.h>
#include <common/properties.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_AWAY_ALIAS);
  set_usage(_LANG_CMD_AWAY_SYNTAX);
  set_help(_LANG_CMD_AWAY_HELP);
}

static int cmd(string str, object me, string verb) 
{
  if (!strlen(str))
  {
    if (!me->query_static_property(AWAY_PROP))
       write(_LANG_CMD_AWAY_NONE);
    else
    {   
       me->remove_static_property(AWAY_PROP);
       write(_LANG_CMD_AWAY_REMOVED);
    }
  }
  else
  {
    me->add_static_property(AWAY_PROP, str);
    write(_LANG_CMD_AWAY_OK);
  }

  me->set_trivial_action();
  return 1;
}

