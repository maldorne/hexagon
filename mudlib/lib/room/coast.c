// System of oceans and seas for Ciudad Capital, neverbot 31/03/09
// 10/2026 - Ported to Hexagon, neverbot (from std/coast.c).
//
// Coast: the edge of water, where one goes in from dry land, a beach, a cliff
// over the sea, a port, a river bank. Shared by the coast room base
// (/lib/coast.c) and the coast location component, which answer
// query_coast_place() with the place itself.
//
// Its way into the water is a swum exit (the "swim" exit option): the map
// shows it, but it is not listed and its direction does nothing. 'swim' alone
// takes one in, and whoever reaches a coast knows from then on how to.
//   kind   beach | cliff | port | harbor | bank

#include <living/skills.h>
#include <room/room.h>
#include <language.h>

private string coast_kind;

object query_coast_place() { return this_object(); }

int query_coast() { return 1; }

string query_coast_kind() { return coast_kind ? coast_kind : "beach"; }
void set_coast_kind(string kind) { coast_kind = kind; }

// The swum exits, the ways into the water.
string * query_swim_directions(object who)
{
  object place;
  string * dirs, * ret;
  int i;

  place = query_coast_place();
  if (!place)
    return ({ });

  dirs = place->query_direc();
  if (!dirs)
    return ({ });

  ret = ({ });
  for (i = 0; i < sizeof(dirs); i++)
    if (place->query_swim_exit(dirs[i]))
      ret += ({ dirs[i] });

  return ret;
}

// Going into the water is not rolled: the strokes that count come after.
int swim(object who, string dir)
{
  return query_coast_place()->force_exit(dir, who, _LANG_COAST_SWIM_MESS,
                                         _LANG_WATER_ARRIVE_MESS);
}

// A line under the description, when there is water to go into.
string query_coast_look(object who)
{
  if (!sizeof(query_swim_directions(who)))
    return "";
  return _LANG_COAST_LOOK;
}

// The same, shorter, under the short line of a glance.
string query_coast_glance(object who)
{
  if (!sizeof(query_swim_directions(who)))
    return "";
  return _LANG_COAST_GLANCE;
}

void coast_event_enter(object ob)
{
  if (ob && interactive(ob) && !ob->query_known_skill(SKILL_SWIM))
    ob->add_known_skill(SKILL_SWIM);
}

mixed * coast_stats()
{
  return ({ ({ "Coast Kind", query_coast_kind(), }), });
}
