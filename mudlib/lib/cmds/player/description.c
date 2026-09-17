// description - the text others read when they look at your character.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_DESCRIPTION_ALIAS);
  set_usage(_LANG_CMD_DESCRIPTION_SYNTAX);
  set_help(_LANG_CMD_DESCRIPTION_HELP);
}

// The text comes back from the editor, or nil when it was not kept.
void description_written(string text)
{
  object me;

  me = this_player();

  if (!strlen(trim(text ? text : "")))
  {
    write(_LANG_CMD_DESCRIPTION_UNCHANGED);
    return;
  }

  if (!me->set_description(text))
  {
    write(_LANG_CMD_DESCRIPTION_TOO_LONG);
    return;
  }

  write(_LANG_CMD_DESCRIPTION_SET);
}

static int cmd(string str, object me, string verb)
{
  str = trim(str ? str : "");

  if (!strlen(str))
  {
    if (strlen(me->query_description()))
      write(_LANG_CMD_DESCRIPTION_CURRENT);
    else
      write(_LANG_CMD_DESCRIPTION_DEFAULT);
    return 1;
  }

  if (member_array(str, _LANG_CMD_DESCRIPTION_CLEAR) != -1)
  {
    me->set_description("");
    write(_LANG_CMD_DESCRIPTION_CLEARED);
    return 1;
  }

  if (member_array(str, _LANG_CMD_DESCRIPTION_EDIT) != -1)
  {
    me->user()->do_edit(me->query_description(), "description_written", this_object());
    return 1;
  }

  if (!me->set_description(str))
  {
    write(_LANG_CMD_DESCRIPTION_TOO_LONG);
    return 1;
  }

  write(_LANG_CMD_DESCRIPTION_SET);
  return 1;
}
