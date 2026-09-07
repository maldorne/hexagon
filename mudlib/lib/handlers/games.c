
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

// The same games as above, named rather than as objects, for callers that work
// in names -- the areas each one holds, the file its state is kept in.
string * query_games()
{
  object * masters;
  string * result;
  string game;
  int i;

  masters = query_game_objects();
  result = ({ });

  for (i = 0; i < sizeof(masters); i++)
  {
    game = game_from_path(file_name(masters[i]));
    if (strlen(game) && member_array(game, result) == -1)
      result += ({ game });
  }

  return result;
}
