
// character list cmd, neverbot 08/2020

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return _LANG_CMD_CHARACTERS;
}

string query_short_help()
{
  return _LANG_CMD_CHARACTERS_HELP;
}

static int cmd (string arg, object me, string verb)
{
  string * list;
  int i;

  if (!strlen(me->query_account_name())) {
    write(_LANG_CMD_CHARACTERS_NO_LOGIN);
    return 1;
  }

  list = me->query_player_list();

  write(_LANG_AVAILABLE_CHARACTERS_IN_ACCOUNT);

  for (i = 0; i < sizeof(list); i++)
    write("   " + ((string)(i + 1)) + ") " + capitalize(list[i]) + "\n");

  return 1;
}
