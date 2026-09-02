
// games handler
// common functions to handle information about available games

// void create()
// {
//   ::create();
// }

object * query_game_objects()
{
  string * dirs;
  int i;
  object * result;

  dirs = get_files("/games/*");
  result = ({ });

  for (i = 0; i < sizeof(dirs); i++)
  {
    object game;
    game = load_object(dirs[i] + "/master.c");

    if (game)
      result += ({ game });
  }

  return result;
}

// The games this mudlib holds, by name. A game is a directory under /games with
// a master in it; there is no register of them, so the tree is the list.
string * query_games()
{
  string * dirs, * result;
  string game;
  int i;

  dirs = get_files("/games/*");
  result = ({ });

  for (i = 0; i < sizeof(dirs); i++)
  {
    if (file_size(dirs[i] + "/master.c") < 0)
      continue;

    game = game_from_path(dirs[i] + "/master.c");
    if (strlen(game) && member_array(game, result) == -1)
      result += ({ game });
  }

  return result;
}
