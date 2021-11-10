
// games list cmd, neverbot 10/2021

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_help()
{
  return _LANG_CMD_GAMES_HELP;
}

static int cmd (string arg, object me, string verb)
{
  int i, is_coder;
  object user;
  object * games;

  is_coder = false;
  user = me->user();

  if (user->player() && user->player()->query_coder())
    is_coder = true;

  // if (!strlen(me->query_account_name()))
  // {
  //   write(_LANG_CMD_GAMES_NO_LOGIN);
  //   return 1;
  // }

  write(_LANG_CMD_GAMES_AVAILABLE);

  games = handler("games")->query_game_objects();

  for (i = 0; i < sizeof(games); i++)
  {
    string line;
    int available;

    available = games[i]->is_available(user);

    if (!is_coder && !available)
      continue;

    line = "   " + ((string)(i + 1)) + ") " + games[i]->query_game_name();

    if (is_coder)
      line += " (" + file_name(games[i]) + ")";

    if (!available)
      line += " (" + _LANG_CMD_GAMES_UNAVAILABLE_GAME + ")";

    line += "\n";

    write(line);
  }

  return 1;
}
