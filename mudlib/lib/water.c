// System of oceans and seas for Ciudad Capital, neverbot 31/03/09
// 10/2026 - Ported to Hexagon, neverbot (from std/sea.c).
//
// A room where one swims instead of walking, open to the sky: a river, a lake,
// the sea. The rules are in /lib/room/water.c; converted to a location, the
// room becomes a water component with the same settings. Water below the
// surface is a location with both the water and the underground components.

inherit room  "/lib/outside.c";
inherit water "/lib/room/water.c";

// Every exit of water is swum.
mixed add_exit(string direc, mixed dest, string type,
               varargs string material, mapping options)
{
  return room::add_exit(direc, dest, type, material,
                        (options ? options : ([ ])) + ([ "swim": 1 ]));
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  room::event_enter(ob, msg, from, avoid);
  water::water_event_enter(ob);
}

// No exit lines in water: in their place, where the shore is. Worked out only
// for somebody looking from here, never while the exits are being set up.
string query_dirs_string()
{
  string look;

  if (!this_player() || environment(this_player()) != this_object())
    return "";

  look = query_water_look(this_player());
  return look[0 .. strlen(look) - 2];
}

string query_short_exit_string() { return ""; }
string query_glance_string() { return query_water_look(this_player()); }

// Above what is here, how to swim on: a look asks with "", a glance with
// nothing, and a glance has its own line.
string query_contents(varargs string str, object * obs)
{
  string ret;

  ret = room::query_contents(str, obs);
  if (!stringp(str) || strlen(str))
    return ret;
  return query_water_hint(this_player()) + ret;
}

mixed * stats()
{
  return room::stats() + water_stats();
}
