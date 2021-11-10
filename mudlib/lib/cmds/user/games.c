
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
  string * list;
  int i, is_coder;
  object user;
  string * dirs, * pieces;

  is_coder = false;

  user = me->user();

  if (user->player() && user->player()->query_coder())
    is_coder = true;

  // if (!strlen(me->query_account_name()))
  // {
  //   write(_LANG_CMD_GAMES_NO_LOGIN);
  //   return 1;
  // }

  write(_LANG_AVAILABLE_GAMES);

  dirs = get_files("/games/*");

  for (i = 0; i < sizeof(dirs); i++)
  {
    object game;
    string line;
    
    line = "   ";

    pieces = explode(dirs[i], "/");
    line += ((string)(i + 1)) + ") " + pieces[1];

    game = load_object(dirs[i] + "/master.c");
    
    if (is_coder)
    {
      if (game)
        line += " (" + file_name(game) + ")";
      else
        line += " (error with master file)";
    }

    line += "\n";

    write(line);
  }

  return 1;
}
