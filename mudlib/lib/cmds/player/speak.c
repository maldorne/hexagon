// speak / hablar — choose the language you speak and write in.
//
// This used to be an add_private_action ("set_language") registered on the
// living in communicate.c. The verb now lives here as a proper command. The
// living owns only the data: set_language(id) is a pure setter (resolves the
// input to an id, checks the player knows it, assigns it, returns a bool),
// and query_languages / query_current_language expose the state. All the
// user-facing presentation lives here in the command. Internal callers of
// set_language (monster setup, social race assignment) are unaffected.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_SPEAK_ALIAS);
  set_usage(_LANG_CMD_SPEAK_SYNTAX);
  set_help(_LANG_CMD_SPEAK_HELP);
}

static int cmd(string arg, object me, string verb)
{
  object lh;
  string res, name, str;
  string * langs;
  int i;

  lh = handler("languages");
  langs = me->query_languages();

  // no argument: show the current language and the ones the player knows
  if (!arg || !strlen(arg))
  {
    if (!strlen(me->query_current_language()))
      res = _LANG_CMD_SPEAK_SELECT;
    else
    {
      name = lh->query_language_display(me->query_current_language());
      res = _LANG_CMD_SPEAK_CURRENT;
    }

    if (sizeof(langs) > 0)
    {
      res += _LANG_CMD_SPEAK_KNOWN_HEADER;
      for (i = 0; i < sizeof(langs); i++)
        res += "\t" + lh->query_language_display(langs[i]) + "\n";
    }
    else
      res += _LANG_CMD_SPEAK_NONE;

    notify_fail(res);
    return 0;
  }

  // set_language resolves the alias / id / display name and checks the player
  // actually knows it; str keeps the raw input for the error message.
  str = arg;
  if (!me->set_language(arg))
  {
    notify_fail(_LANG_CMD_SPEAK_UNKNOWN);
    return 0;
  }

  name = lh->query_language_display(me->query_current_language());
  tell_object(me, _LANG_CMD_SPEAK_NOW_USING);
  return 1;
}
