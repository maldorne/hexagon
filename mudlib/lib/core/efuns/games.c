

static string game_name(object ob)
{
  string fname;
  string * words;

  // if it is a player, we use the room they are in to know
  // what game are they playing
  if (ob->query_player())
    ob = environment(ob);

  fname = file_name(ob);
  words = explode(fname, "/");

  // if (member_array(dom, get_dir("/games/")) != -1)
  //    return dom;
  if ((sizeof(words) > 2) && (words[0] == "games"))
    return words[1];
 
  return "";
}

static string game_root(varargs object ob)
{
  string ret;

  if (!ob)
    ob = this_object();

  ret = game_name(ob);

  if (!strlen(ret))
    return "/";

  return "/games/" + ret + "/";
}

static int is_in_game(object ob)
{
  if (explode(base_name(ob), "/")[0] == "games")
    return 1;
  return 0;
}
