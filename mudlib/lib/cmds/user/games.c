// games list cmd, neverbot 10/2021
//
// Runs before a login too, from the option list of the welcome screen, so it is
// the first thing a new player reads about how Hexagon is played.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_GAMES_ALIAS);
}

string query_usage()
{
  return _LANG_CMD_GAMES_SYNTAX;
}

string query_help()
{
  return _LANG_CMD_GAMES_HELP;
}

// The demos first: those are the ones a new account can always play.
private object * demos_first(object * games)
{
  object * out;
  int i;

  out = ({ });

  for (i = 0; i < sizeof(games); i++)
    if (games[i]->query_demo())
      out += ({ games[i] });

  for (i = 0; i < sizeof(games); i++)
    if (!games[i]->query_demo())
      out += ({ games[i] });

  return out;
}

static int cmd (string arg, object me, string verb)
{
  int i, shown, is_coder, width;
  object user;
  object * games;
  string ret;

  is_coder = false;
  user = me->user();

  if (user->player() && user->player()->query_coder())
    is_coder = true;

  // the width left for a description once its indentation is taken out: a
  // sprintf field wraps to it and lines up what follows under the first line.
  // Without the '=' flag, which would capitalize the text and mangle an
  // accented first letter
  width = (user->query_cols() ? user->query_cols() : 80) - 16;

  // the frame opens and closes with a blank row of its own, so the text adds
  // none at either end
  ret = _LANG_CMD_GAMES_AVAILABLE;

  games = demos_first(handler("games")->query_game_objects());
  shown = 0;

  for (i = 0; i < sizeof(games); i++)
  {
    string line;
    int available;

    available = games[i]->is_available(user);

    if (!is_coder && !available)
      continue;

    // numbered as they are listed: a game nobody can see leaves no gap
    shown++;
    // a blank line over each one, so they are told apart at a glance
    line = "\n   %^BOLD%^" + shown + ") %^CYAN%^" +
           games[i]->query_game_name() + "%^RESET%^";

    if (games[i]->query_demo())
      line += " (" + _LANG_CMD_GAMES_DEMO_GAME + ")";

    if (!available)
      line += " (" + _LANG_CMD_GAMES_UNAVAILABLE_GAME + ")";

    line += "\n";

    if (is_coder)
      line += "      " + file_name(games[i]) + "\n";

    if (games[i]->query_game_short_description())
      line += sprintf("      %-" + width + "s",
                      games[i]->query_game_short_description()) + "\n";

    ret += line;
  }

  // how playing here works, which is what somebody reading this list needs
  ret += "\n" + _LANG_CMD_GAMES_START_TITLE + "\n" +
         sprintf("   %-" + width + "s", _LANG_CMD_GAMES_START_DEMO) + "\n\n" +
         sprintf("   %-" + width + "s", _LANG_CMD_GAMES_START_CARRY) + "\n";

  write(handler("frames")->frame(ret));
  return 1;
}
