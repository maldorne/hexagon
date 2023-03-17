
static nomask string game_name(object ob)
{
  string fname;
  string * words;

  if (!ob)
    return "";

  // if it is a player, we use the room they are in to know
  // what game are they playing
  if (ob->query_player())
    return game_name(environment(ob));

  fname = file_name(ob);
  words = explode(fname, "/");

  // if (member_array(dom, get_dir("/games/")) != -1)
  //    return dom;
  if ((sizeof(words) > 2) && (words[0] == "games"))
    return words[1];
 
  return "";
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

static nomask int is_in_game(object ob)
{
  if (explode(base_name(ob), "/")[0] == "games")
    return 1;
  return 0;
}
