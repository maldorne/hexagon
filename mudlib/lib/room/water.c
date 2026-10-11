// System of oceans and seas for Ciudad Capital, neverbot 31/03/09
// 10/2026 - Ported to Hexagon, neverbot (from std/sea.c).
//
// Water: a place where one swims instead of walking, a river, a lake, the
// sea, an underground stream. Shared by the water room base (/lib/water.c)
// and the water location component, which answer query_water_place() with
// the place itself.
//
// Every exit of water is swum (the "swim" exit option): it is there for the
// map and the paths, but it is not listed and its direction does nothing. One
// crosses it with the swim skill, and what one sees from the water is where
// the shore is. What a place declares:
//   depth     1 swimming at the surface, 2 deep water, harder to cross
//   current   the exit the water flows out by, nil for still water
//   strength  0 still .. 3 rapids, how hard the current pulls
//   upstream  free | hard | block, swimming any way but the current's
//   swept_to  the exit a failed or idle swimmer is carried out by, the
//             current's own when not set

#include <living/skills.h>
#include <room/water.h>
#include <room/room.h>
#include <language.h>

private int water_depth, water_strength;
private string water_current, water_upstream, water_swept_to;

object query_water_place() { return this_object(); }

int query_water_environment() { return 1; }
int query_water() { return 1; }

int query_water_depth() { return water_depth ? water_depth : 1; }
void set_water_depth(int i) { water_depth = i; }

string query_water_current() { return water_current; }
void set_water_current(string dir) { water_current = dir; }

int query_water_strength() { return water_strength; }
void set_water_strength(int i) { water_strength = i; }

string query_water_upstream()
{
  return water_upstream ? water_upstream : WATER_UPSTREAM_FREE;
}
void set_water_upstream(string how) { water_upstream = how; }

// Where the current carries whoever does not make it, or stays still.
string query_water_swept_to()
{
  return water_swept_to ? water_swept_to : water_current;
}
void set_water_swept_to(string dir) { water_swept_to = dir; }

// The water settings as a mapping, for the conversion of a room into a
// location and for the component's own save.
mapping query_water_settings()
{
  return ([
      "water_depth"    : water_depth,
      "water_current"  : water_current,
      "water_strength" : water_strength,
      "water_upstream" : water_upstream,
      "water_swept_to" : water_swept_to,
    ]);
}

void set_water_settings(mapping m)
{
  if (!m)
    return;
  water_depth = m["water_depth"];
  water_current = m["water_current"];
  water_strength = m["water_strength"];
  water_upstream = m["water_upstream"];
  water_swept_to = m["water_swept_to"];
}

// The exits one can swim out by: every one that is not sealed, but only the
// current's own when the water does not let anybody swim against it.
string * query_swim_directions(object who)
{
  object place;
  string * dirs, * ret;
  int i;

  place = query_water_place();
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
    if (water_current && dirs[i] != water_current &&
        query_water_upstream() == WATER_UPSTREAM_BLOCK)
      continue;
    ret += ({ dirs[i] });
  }

  return ret;
}

// The chance of one stroke: a base plus the swimmer's ability, easy with the
// current, harder across it and harder still against it (straight back where
// it comes from) the stronger it pulls, harder in deep water and with a heavy
// load. A chance of 100 or more is not rolled.
int query_swim_chance(object who, string dir)
{
  int chance, max, load;

  chance = WATER_BASE_CHANCE + who->query_skill_ability(SKILL_SWIM);

  if (water_current)
  {
    if (dir == water_current)
      chance += WATER_DOWNSTREAM_BONUS;
    else if (dir == ROOM_HAND->query_opposite(water_current))
      chance -= (query_water_upstream() == WATER_UPSTREAM_HARD) ?
                WATER_UPSTREAM_MALUS * (water_strength + 1) :
                WATER_ACROSS_MALUS * water_strength;
    else
      chance -= WATER_ACROSS_MALUS * water_strength;
  }

  if (query_water_depth() >= 2)
    chance -= WATER_UPSTREAM_MALUS;

  // what is carried beyond half of what one can carry drags one down
  max = who->query_max_weight();
  if (max > 0)
  {
    load = who->query_contents_weight() * 100 / max;
    if (load > 50)
      chance -= load - 50;
  }

  return chance + who->query_skill_malus() * 3;
}

// Water in the lungs: a bite of life that never takes the last of it.
void lose_breath(object who)
{
  int damage;

  damage = who->query_max_hp() / 10 + 1;
  if (who->query_hp() - damage < 1)
    damage = who->query_hp() - 1;
  if (damage > 0)
    who->adjust_hp(-damage);
}

// Take somebody out of the water place through a swum exit.
int move_swimmer(object who, string dir, string leave)
{
  object place;

  place = query_water_place();
  if (!place || !dir)
    return 0;

  return place->force_exit(dir, who, leave, _LANG_WATER_ARRIVE_MESS);
}

// One stroke towards `dir`, rolled. Whoever fails is carried off by the
// current, or stays where they are in still water, and swallows water either
// way. Returns whether they got where they meant to.
int swim(object who, string dir)
{
  string swept;
  int chance;

  who->update_skill_used_times(SKILL_SWIM);
  chance = query_swim_chance(who, dir);

  if (chance >= 100 || random(100) + 1 <= chance)
  {
    move_swimmer(who, dir, _LANG_WATER_SWIM_MESS);
    return 1;
  }

  lose_breath(who);
  swept = query_water_swept_to();

  if (swept && member_array(swept, query_water_place()->query_direc()) != -1)
  {
    tell_object(who, _LANG_WATER_SWEPT_ME);
    move_swimmer(who, swept, _LANG_WATER_SWEPT_MESS);
  }
  else
    tell_object(who, _LANG_WATER_SWIM_FAILED);

  return 0;
}

// What one sees from the water: where the shore is, never the exits.
string query_water_look(object who)
{
  string * dirs, * shores;
  object dest;
  int i;

  dirs = query_swim_directions(who);
  shores = ({ });
  for (i = 0; i < sizeof(dirs); i++)
  {
    dest = query_water_place()->query_dest_object(dirs[i]);
    if (dest && dest->query_coast())
      shores += ({ ROOM_HAND->query_exit_dir(dirs[i]) });
  }

  if (!sizeof(shores))
    return _LANG_WATER_NO_SHORE;

  return _LANG_WATER_SHORE + query_multiple_short(shores) + ".\n";
}

// Whoever comes into moving water and does not swim on is carried off by it
// after a while. Anybody who reaches water knows from then on how to swim.
void water_event_enter(object ob)
{
  if (!ob || !living(ob))
    return;

  if (interactive(ob) && !ob->query_known_skill(SKILL_SWIM))
    ob->add_known_skill(SKILL_SWIM);

  if (ob->query_swims() || water_strength <= 0 || !query_water_swept_to())
    return;

  call_out("water_drift", WATER_DRIFT_TIME, ob);
}

void water_drift(object ob)
{
  if (!ob || environment(ob) != query_water_place())
    return;

  // a stroke already on its way decides where they go
  if (ob->query_active_effect("effect"))
  {
    call_out("water_drift", WATER_DRIFT_TIME, ob);
    return;
  }

  tell_object(ob, _LANG_WATER_DRIFT_ME);
  move_swimmer(ob, query_water_swept_to(), _LANG_WATER_SWEPT_MESS);
}

mixed * water_stats()
{
  return ({
      ({ "Water Depth", query_water_depth(), }),
      ({ "Water Current", water_current, }),
      ({ "Water Strength", water_strength, }),
      ({ "Water Upstream", query_water_upstream(), }),
      ({ "Water Swept To", query_water_swept_to(), }),
    });
}
