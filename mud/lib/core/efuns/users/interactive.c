
// interactive - detects whether or not a given object is an interactive
// int interactive( object ob );
// Return non-zero if 'ob' is an interactive player. 0 will be returned
// if he is link dead.

static nomask void stderr(string str);


static int interactive(object ob)
{
  if (!ob)
    return 0;

  if (ob->query_link() || ob->query_player())
  {
    ob = ob->user();
  }

  if (!ob->query_user())
    return 0;

  if (query_ip_number(ob) == nil)
    return 0;

  return 1;
}
