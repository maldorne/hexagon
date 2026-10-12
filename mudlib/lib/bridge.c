// A room that is a bridge and water at once. The rules are in
// /lib/room/bridge.c; converted to a location, the room becomes a bridge
// component with the same settings. Its exits of type "bridge" are walked,
// every other exit is swum.
//
// A bridge below the surface calls set_underground_kind() and counts as
// underground as well.

inherit room   "/lib/underground.c";
inherit bridge "/lib/room/bridge.c";

#include <room/bridge.h>

private int below_surface;

int query_underground() { return below_surface; }

void set_underground_kind(string kind)
{
  below_surface = 1;
  room::set_underground_kind(kind);
}

// Every exit is swum but the bridge ones.
mixed add_exit(string direc, mixed dest, string type,
               varargs string material, mapping options)
{
  if (type == BRIDGE_EXIT_TYPE)
    return room::add_exit(direc, dest, type, material, options);

  return room::add_exit(direc, dest, type, material,
                        (options ? options : ([ ])) + ([ "swim": 1 ]));
}

// The room base answers it with no refusal; the bridge's rules replace that.
string query_exit_refusal(string dir, object who)
{
  return bridge::query_exit_refusal(dir, who);
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  room::event_enter(ob, msg, from, avoid);
  bridge::bridge_event_enter(ob, from);
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  room::event_exit(ob, msg, dest, avoid);
  bridge::bridge_event_exit(ob);
}

// From the water there are no exit lines: in their place, where the shore is.
// Worked out only for somebody looking from here, never while the exits are
// being set up.
string query_dirs_string()
{
  string look;

  if (!this_player() || environment(this_player()) != this_object() ||
      !query_in_water(this_player()))
    return room::query_dirs_string();

  look = query_water_look(this_player());
  return look[0 .. strlen(look) - 2];
}

string query_short_exit_string()
{
  if (this_player() && environment(this_player()) == this_object() &&
      query_in_water(this_player()))
    return "";
  return room::query_short_exit_string();
}

string query_glance_string() { return query_bridge_glance(this_player()); }

// Above what is here, where one is: a look asks with "", a glance with
// nothing, and a glance has its own line.
string query_contents(varargs string str, object * obs)
{
  string ret;

  ret = room::query_contents(str, obs);
  if (!stringp(str) || strlen(str))
    return ret;
  return query_bridge_hint(this_player()) + ret;
}

mixed * stats()
{
  return room::stats() + bridge_stats();
}
