
// query_idle - determine how many seconds an interactive player has been idle
// int query_idle( object ob );
// Query how many seconds a player object (ob) has been idling.

static nomask int query_idle(object ob)
{
  if (!ob)
    ob = this_object();

  if (!ob->query_player())
    return 0;

  return time() - ob->query_timestamp();
}
