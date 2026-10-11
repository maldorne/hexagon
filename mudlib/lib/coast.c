// System of oceans and seas for Ciudad Capital, neverbot 31/03/09
// 10/2026 - Ported to Hexagon, neverbot (from std/coast.c).
//
// A room at the edge of water: a beach, a cliff over the sea, a port, a river
// bank. The rules are in /lib/room/coast.c; converted to a location, the room
// becomes a coast component of the same kind.
//
// A coast below the surface, the bank of an underground stream, calls
// set_underground_kind() and counts as underground as well.

inherit room  "/lib/underground.c";
inherit coast "/lib/room/coast.c";

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
  coast::coast_event_enter(ob);
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

string long(string str, int dark)
{
  string ret;

  ret = room::long(str, dark);
  if ((str && strlen(str)) || dark)
    return ret;
  return ret + query_coast_look(this_player());
}

mixed * stats()
{
  return room::stats() + coast_stats();
}
