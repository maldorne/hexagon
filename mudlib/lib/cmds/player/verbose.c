
#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_VERBOSE_ALIAS);
  set_usage(_LANG_CMD_VERBOSE_SYNTAX);
  set_help(_LANG_CMD_VERBOSE_HELP);
}

static int cmd(string str, object me, string verb) 
{
  if (!strlen(str))
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_VERBOSE_SYNTAX + "\n");
    if (me->user()->query_verbose())
      write(_LANG_CMD_VERBOSE_CURRENT_ON);
    else
      write(_LANG_CMD_VERBOSE_CURRENT_OFF);
  }
  else if (member_array(str, _LANG_AFFIRMATIVE_OPTIONS) != -1)
  {
    write(_LANG_CMD_VERBOSE_ON);
    me->user()->set_verbose(1);
  }
  else if (member_array(str, _LANG_NEGATIVE_OPTIONS) != -1)
  {
    write(_LANG_CMD_VERBOSE_OFF);
    me->user()->set_verbose(0);
  }
  else if (str)
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_VERBOSE_SYNTAX + "\n");
  }

  me->set_trivial_action();
  return 1;
}

