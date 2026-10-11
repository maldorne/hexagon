// System of oceans and seas for Ciudad Capital, neverbot 31/03/09
// 10/2026 - Ported to Hexagon, neverbot (from std/sea.c).
//
// A room where one swims instead of walking: a river, a lake, the sea. The
// rules are in /lib/room/water.c; converted to a location, the room becomes a
// water component with the same settings.
//
// A water room below the surface, an underground stream, calls
// set_underground_kind() and counts as underground as well.

inherit room  "/lib/underground.c";
inherit water "/lib/room/water.c";

private int below_surface;

int query_underground() { return below_surface; }

void set_underground_kind(string kind)
{
  below_surface = 1;
  room::set_underground_kind(kind);
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  room::event_enter(ob, msg, from, avoid);
  water::water_event_enter(ob);
}

int do_exit_command(string str, varargs mixed verb, object ob)
{
  string refusal;

  if (!verb)
    verb = query_verb();
  if (!ob)
    ob = this_player();

  refusal = query_walk_refusal(ob, query_verb_exit(verb));
  if (refusal)
  {
    notify_fail(refusal);
    return 0;
  }

  return room::do_exit_command(str, verb, ob);
}

string query_dirs_string() { return query_water_dirs_string(this_player()); }
string query_short_exit_string() { return query_water_short_exit_string(this_player()); }

mixed * stats()
{
  return room::stats() + water_stats();
}
