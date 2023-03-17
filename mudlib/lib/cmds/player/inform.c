
#include <mud/cmd.h>
#include <translations/inform.h>
#include <translations/common.h>
#include <common/properties.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_INFORM_ALIAS);
  set_usage(_LANG_CMD_INFORM_SYNTAX);
  set_help(_LANG_CMD_INFORM_HELP);
}

static int cmd(string str, object me, string verb) 
{
  string * types, * frog;
  string ret;
  object user;
  int i;

  ret = "";
  user = me->user();

  if (user->query_admin())
    types = INFORM_EVENTS_ADMINS;
  else if (user->query_coder())
    types = INFORM_EVENTS_CODERS;
  else
    types = INFORM_EVENTS_PLAYERS;

  stderr(to_string(types));

  if (!strlen(str))
    str = "";

  frog = explode(str, " ");
  if (!frog)
    frog = ({ });
  else
    frog = frog - ({ "", nil });

  // inform
  if (!sizeof(frog))
  {
    ret += _LANG_INFORM_HEADER;

    if (user->query_property(NO_INFORM))
      ret += _LANG_INFORM_BLOCKED;

    ret += "\n";

    for (i = 0; i < sizeof(types); i++)
      ret += sprintf("  %-15s %|7s\n", types[i], user->query_inform(types[i]) ? _LANG_YES : _LANG_NO);

    ret = handler("frames")->frame(ret);
    write(ret);
    return 1;
  }

  // inform yes, no, all, none
  if (sizeof(frog) == 1)
  {
    ret = "";

    if (member_array(frog[0], _LANG_AFFIRMATIVE_OPTIONS) != -1)
    {
      if (user->query_property(NO_INFORM))
      {
        user->remove_property(NO_INFORM);
        write(_LANG_INFORM_EVENTS_UNBLOCKED);
        return 1;
      }

      write(_LANG_SYNTAX + ": " +_LANG_CMD_INFORM_SYNTAX + "\n");
      return 1;        
    }
    else if (member_array(frog[0], _LANG_NEGATIVE_OPTIONS) != -1)
    {
      user->add_property(NO_INFORM, 1);
      write(_LANG_INFORM_EVENTS_BLOCKED);
      return 1;
    }
    else if (member_array(frog[0], _LANG_ALL_OPTIONS) != -1)
    {
      for (i = 0; i < sizeof(types); i++)
        user->set_inform(types[i], 1);

      write(_LANG_INFORM_ALL_ON);
      return 1;
    }
    else if (member_array(frog[0], _LANG_NONE_OPTIONS) != -1)
    {
      for (i = 0; i < sizeof(types); i++)
        user->set_inform(types[i], 0);

      write(_LANG_INFORM_ALL_OFF);
      return 1;
    }

    write(_LANG_SYNTAX + ": " +_LANG_CMD_INFORM_SYNTAX + "\n");
    return 1;      
  }

  // inform <something> yes, no
  if (sizeof(frog) == 2)
  {
    if (member_array(frog[0], types) == -1)
    {
      write(_LANG_INFORM_UNKNOWN_EVENT);
      return 1;
    }
    
    if (member_array(frog[1], _LANG_AFFIRMATIVE_OPTIONS) != -1)
    {
      user->set_inform(frog[0], 1);
      write(_LANG_INFORM_ON);
      return 1;
    }
    else if (member_array(frog[1], _LANG_NEGATIVE_OPTIONS) != -1)
    {
      user->set_inform(frog[0], 0);
      write(_LANG_INFORM_OFF);
      return 1;
    }
  }

  write(_LANG_SYNTAX + ": " +_LANG_CMD_INFORM_SYNTAX + "\n");
  return 1;   
}

