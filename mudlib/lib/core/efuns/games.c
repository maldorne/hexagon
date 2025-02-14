
#include <mud/config.h>

static nomask string game_from_path(string path)
{
  string * words;
  words = explode(path, "/");

  if ((sizeof(words) > 2) && (words[0] == "games"))
    return words[1];
  if ((sizeof(words) > 3) && (words[0] == "save") && (words[1] == "games"))
    return words[2];

  return "";
}

static nomask string game_name(varargs object ob)
{
  string fname;
  string * words;

  if (!ob)
    ob = this_object();

  // if it is a player, we use the room they are in to know
  // what game are they playing
  if (ob->query_player() && environment(ob))
    return game_name(environment(ob));

  return game_from_path(file_name(ob));
}

static nomask string game_root(varargs object ob)
{
  string ret;

  if (!ob)
    ob = this_object();

  ret = game_name(ob);

  if (!strlen(ret))
    return "/";

  return "/games/" + ret + "/";
}

static nomask string game_save_dir(varargs object ob)
{
  string ret;

  if (!ob)
    ob = this_object();

  ret = game_name(ob);

  if (!strlen(ret))
    return "/save/";

  return "/save/games/" + ret + "/";
}

static nomask object game_master_object(object ob)
{
  object master;
  string path;

  path = game_root(ob);
  master = load_object(path + "master.c");

  if (!master)
    return nil;

  return master;
}

static nomask string game_pretty_name(varargs object ob)
{
  object master;

  if (!ob)
    ob = this_object();

  master = game_master_object(ob);

  if (master)
    return master->query_game_name();
  else
    return MUD_NAME;
}

static nomask int is_in_game(object ob)
{
  if (explode(base_name(ob), "/")[0] == "games")
    return 1;
  return 0;
}
