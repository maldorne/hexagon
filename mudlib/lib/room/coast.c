// System of oceans and seas for Ciudad Capital, neverbot 31/03/09
// 10/2026 - Ported to Hexagon, neverbot (from std/coast.c).
//
// Coast: the edge of water, where one goes in from dry land, a beach, a cliff
// over the sea, a port, a river bank. Shared by the coast room base
// (/lib/coast.c) and the coast location component, which answer
// query_coast_place() with the place itself.
//
// Exits into water are not walked: one swims into the water from here, with
// the swim skill, and whoever reaches a coast knows from then on how to.
//   kind   beach | cliff | port | harbor | bank

#include <living/skills.h>
#include <room/room.h>
#include <language.h>

private string coast_kind;

object query_coast_place() { return this_object(); }

int query_coast() { return 1; }

string query_coast_kind() { return coast_kind ? coast_kind : "beach"; }
void set_coast_kind(string kind) { coast_kind = kind; }

// The exits that go into water.
string * query_swim_directions(object who)
{
  object place, dest;
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
  {
    if (place->query_sealed_exit(dirs[i]))
      continue;
    dest = place->query_dest_object(dirs[i]);
    if (dest && dest->query_water_environment())
      ret += ({ dirs[i] });
  }

  return ret;
}

// Going into the water is not rolled: the strokes that count come after.
int swim(object who, string dir)
{
  object dest;

  dest = query_coast_place()->query_dest_object(dir);
  if (!dest)
    return 0;

  return who->move_living(dir, dest, _LANG_COAST_SWIM_MESS);
}

// Nobody walks into the water but those who swim by nature.
string query_walk_refusal(object who, string dir)
{
  if (!who || who->query_swims() || !dir)
    return nil;
  if (member_array(dir, query_swim_directions(who)) == -1)
    return nil;
  return _LANG_COAST_MUST_SWIM;
}

// A line under the description: where one can swim from here.
string query_coast_look(object who)
{
  string * dirs, * names;
  int i;

  dirs = query_swim_directions(who);
  if (!sizeof(dirs))
    return "";

  names = ({ });
  for (i = 0; i < sizeof(dirs); i++)
    names += ({ ROOM_HAND->query_exit_dir(dirs[i]) });

  return _LANG_COAST_CAN_SWIM + query_multiple_short(names) + ".\n";
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
