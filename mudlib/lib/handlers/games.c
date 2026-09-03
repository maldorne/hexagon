
// games handler
// common functions to handle information about available games

// void create()
// {
//   ::create();
// }

// Read the tree rather than get_files: this is asked from cron as well as from
// a player's command, and get_files resolves the path against a logged-in user,
// so from a timed call it would hand back nothing.
object * query_game_objects()
{
  mixed * entries;
  object * result;
  object game;
  string path;
  int i;

  entries = get_dir("/games/*", -1);
  result = ({ });

  for (i = 0; i < sizeof(entries); i++)
  {
    if (entries[i][1] != -2)   // size -2 marks a directory
      continue;

    path = "/games/" + entries[i][0] + "/master.c";
    if (file_size(path) < 0)
      continue;

    game = load_object(path);
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
