
// games handler
// common functions to handle information about available games
//
// Also where a character leaves a demo: which games it may go on to, and the
// move itself. The demo only tells the story and asks which one.

#include <mud/games.h>

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

// The games somebody is shown, in the order they are numbered: the demos first,
// since those are the ones a new account can always play. Players see the open
// ones; coders see every game.
object * query_listed_games(object user)
{
  object * games, * result;
  int i, coder;

  coder = user && user->player() && user->player()->query_coder();
  games = query_game_objects();
  result = ({ });

  for (i = 0; i < sizeof(games); i++)
    if (games[i]->query_demo() && (coder || games[i]->query_open()))
      result += ({ games[i] });

  for (i = 0; i < sizeof(games); i++)
    if (!games[i]->query_demo() && (coder || games[i]->query_open()))
      result += ({ games[i] });

  return result;
}

// A character goes into a demo: the place is its account's until it finishes.
void start_demo(object player, object game)
{
  object user;

  if (!game->query_demo() || !(user = player->user()))
    return;

  user->set_demo_player(game_name(game), player->query_name());
}

// The playable race of a game with this id, as its file, or "". Lineages live
// in subdirectories and are not looked at: they are chosen after the race.
string query_race_file(string game, string id)
{
  string * files;
  string dir;
  object race;
  int i;

  if (!strlen(id))
    return "";

  dir = "/games/" + game + "/obj/races/";
  files = get_dir(dir + "*.c");

  for (i = 0; i < sizeof(files); i++)
    if ((race = load_object(dir + files[i])) && race->query_is_race_ob() &&
        race->query_playable() && race->query_race_id() == id)
      return dir + files[i];

  return "";
}

// The class of a game with this id, as its file, or "".
string query_class_file(string game, string id)
{
  string * files;
  string dir;
  object class_ob;
  int i;

  if (!strlen(id))
    return "";

  dir = "/games/" + game + "/obj/classes/";
  files = get_dir(dir + "*.c");

  for (i = 0; i < sizeof(files); i++)
    if ((class_ob = load_object(dir + files[i])) &&
        class_ob->query_class_id() == id)
      return dir + files[i];

  return "";
}

// The race a character takes in another game: the one with the id of its own,
// or of its base race when it belongs to a lineage.
string query_equivalent_race(object player, string game)
{
  string file;

  file = query_race_file(game, player->query_race_id());

  if (!strlen(file))
    file = query_race_file(game, player->query_base_race_id());

  return file;
}

// Where a character finishing the demo it is in may go: the open games that
// are not demos, of the same genre, and where its race exists.
object * query_transfer_destinations(object player)
{
  object * games, * result;
  object source;
  int i;

  source = game_master_object(player);
  result = ({ });

  if (!source || !source->query_demo())
    return result;

  games = query_game_objects();

  for (i = 0; i < sizeof(games); i++)
    if (games[i] != source && games[i]->query_open() &&
        !games[i]->query_demo() &&
        games[i]->query_genre() == source->query_genre() &&
        strlen(query_equivalent_race(player, game_name(games[i]))))
      result += ({ games[i] });

  return result;
}

// Takes a character out of its demo and into another game, for good. What it
// carries, its class and its race are settled by the start rooms of the
// destination, as they would be for a new character.
int transfer(object player, object game)
{
  object user;
  string source, start;

  if (member_array(game, query_transfer_destinations(player)) == -1)
    return 0;

  start = game_root(game) + GAME_START_ROOM;

  if (!load_object(start))
    return 0;

  source = game_name(player);

  if (user = player->user())
    user->add_finished_demo(source, player->query_name());

  log_file("transfers", player->query_cap_name() + ": " + source + " -> " +
    game_name(game) + ", " + ctime(time(), 4) + "\n");

  player->move(start);
  player->do_look();
  return 1;
}
