
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
