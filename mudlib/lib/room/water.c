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
#include <basic/move.h>
#include <language.h>

private int water_depth, water_strength;
private string water_current, water_upstream, water_swept_to;

// who has a drift pending, and the things the current is about to carry
// (never saved)
static mapping drifting;
static object * drift_objects;
static int drift_objects_due;

object query_water_place() { return this_object(); }

int query_water_environment() { return 1; }
int query_water() { return 1; }

// Whether `who` is in the water here: everybody is, in a plain water place.
int query_in_water(object who) { return 1; }

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
    tell_object(who, _LANG_WATER_SWIM_ME);
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

// Where the shore is, from the water: nothing when no swum exit leads to one.
string query_water_shore(object who)
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
    return "";

  return _LANG_WATER_SHORE + query_multiple_short(shores) + ".\n";
}

// What one sees from the water: where the shore is, never the exits.
string query_water_look(object who)
{
  string shore;

  shore = query_water_shore(who);
  return strlen(shore) ? shore : _LANG_WATER_NO_SHORE;
}

// What is worth telling somebody who has just arrived in the water, each
// hint only while it applies. Places add theirs.
string * query_water_arrival_hints(object who)
{
  string shore;

  shore = query_water_shore(who);
  if (!strlen(shore))
    return ({ });
  return ({ shore[0 .. strlen(shore) - 2] });
}

// Bold, between blank lines, so it is not lost among the movement messages.
void tell_water_arrival_hints(object who)
{
  string * hints;

  if (!interactive(who))
    return;

  hints = query_water_arrival_hints(who);
  if (sizeof(hints))
    tell_object(who, "\n%^BOLD%^" + implode(hints, " ") + "%^RESET%^\n\n");
}

// A line under the description: that one is in the water, and how to swim.
string query_water_hint(object who)
{
  return _LANG_WATER_LOOK;
}

// Whether the current here carries things off: it moves, and the exit it
// flows out by exists.
int query_water_flows()
{
  string * dirs;

  if (!water_current || water_strength <= 0 || !query_water_place())
    return 0;

  dirs = query_water_place()->query_direc();
  return dirs && member_array(water_current, dirs) != -1;
}

// Whether the current would drag `who` if they were here and in the water.
int query_water_drags(object who)
{
  return who && query_in_water(who) && query_water_flows();
}

// One drift pending per living: coming back in before it falls due does not
// add another.
private void queue_drift(object ob)
{
  if (!drifting)
    drifting = ([ ]);
  if (drifting[ob])
    return;

  drifting[ob] = 1;
  call_out("water_drift", WATER_DRIFT_TIME, ob);
}

// Whether `ob`, or anything inside it, is a unique object.
private int holds_unique(object ob)
{
  object * inv;
  int i;

  if (ob->query_unique_object())
    return 1;

  inv = all_inventory(ob);
  for (i = 0; i < sizeof(inv); i++)
    if (holds_unique(inv[i]))
      return 1;

  return 0;
}

// Put a thing that is not a living in the queue of what the current carries
// off. Things that arrive in a cell with a current queue themselves; a place
// that does not tell them apart (a bridge cell) is asked for the ones in the
// water.
void water_carry(object ob)
{
  if (!ob || living(ob) || ob->get() || !query_water_flows())
    return;

  if (!drift_objects)
    drift_objects = ({ });
  if (member_array(ob, drift_objects) != -1)
    return;

  drift_objects += ({ ob });

  // a single call_out for every thing waiting here
  if (!drift_objects_due)
  {
    drift_objects_due = 1;
    call_out("water_drift_objects", WATER_DRIFT_TIME);
  }
}

// Whoever comes into moving water and does not swim on is carried off by it
// after a while. Anybody who reaches water knows from then on how to swim.
// Anything else that comes in is carried off too.
void water_event_enter(object ob)
{
  if (!ob)
    return;

  if (!living(ob))
  {
    water_carry(ob);
    return;
  }

  if (interactive(ob) && !ob->query_known_skill(SKILL_SWIM))
    ob->add_known_skill(SKILL_SWIM);

  tell_water_arrival_hints(ob);

  if (ob->query_swims() || water_strength <= 0 || !query_water_swept_to())
    return;

  queue_drift(ob);
}

// The current drags somebody who is not swimming: it may hurt them, more the
// stronger it is and less the better they swim. Returns whether it killed.
private int hurt_by_current(object who)
{
  int chance, damage, i;

  if (who->query_dead())
    return 0;

  chance = WATER_HURT_BASE_CHANCE + WATER_HURT_STRENGTH_CHANCE * water_strength -
           who->query_skill_ability(SKILL_SWIM) / WATER_HURT_SKILL_DIVISOR;
  if (random(100) + 1 > chance)
    return 0;

  damage = who->query_max_hp() * water_strength * WATER_HURT_HP_PERCENT / 100;
  for (i = 0; i < water_strength; i++)
    damage += 1 + random(WATER_HURT_DIE);

  tell_object(who, _LANG_WATER_HURT_ME);
  tell_room(query_water_place(), replace_string(_LANG_WATER_HURT_ROOM, "$N",
            who->query_cap_name()) + "\n", who);

  who->adjust_hp(-damage);
  return who->query_hp() < 0;
}

void water_drift(object ob)
{
  if (ob && drifting)
    map_delete(drifting, ob);

  // only whoever is in the water drifts (not who is on a bridge above it)
  if (!ob || environment(ob) != query_water_place() || !query_in_water(ob))
    return;

  // a stroke already on its way decides where they go
  if (ob->query_active_effect("effect"))
  {
    queue_drift(ob);
    return;
  }

  // a death happens here, in the water
  if (hurt_by_current(ob))
    return;

  tell_object(ob, _LANG_WATER_DRIFT_ME);
  move_swimmer(ob, query_water_swept_to(), _LANG_WATER_SWEPT_MESS);
}

// The exit of this place that leads to a coast by swimming, one of them at
// random, or nil.
private string query_shore_exit()
{
  object place, dest;
  string * dirs, * shores;
  int i;

  place = query_water_place();
  dirs = place->query_direc();
  shores = ({ });
  for (i = 0; i < sizeof(dirs); i++)
  {
    if (!place->query_swim_exit(dirs[i]) || place->query_sealed_exit(dirs[i]))
      continue;
    dest = place->query_dest_object(dirs[i]);
    if (dest && dest->query_coast())
      shores += ({ dirs[i] });
  }

  return sizeof(shores) ? shores[random(sizeof(shores))] : nil;
}

// One drag of the current on a thing: lost for good, washed onto the shore of
// this place if it has one, or carried on downstream.
private void carry_object(object ob)
{
  object place, dest;
  string shore, dir, name;
  int roll, shored;

  place = query_water_place();
  name = ob->query_short();
  if (!name)
    name = "";

  shore = query_shore_exit();
  roll = random(100);

  if (roll < WATER_OBJECT_LOST_CHANCE && !holds_unique(ob))
  {
    tell_room(place, replace_string(_LANG_WATER_OBJECT_LOST, "$O", name) + "\n");
    ob->dest_me();
    return;
  }

  shored = shore && roll < WATER_OBJECT_LOST_CHANCE + WATER_OBJECT_SHORE_CHANCE;
  dir = shored ? shore : water_current;

  dest = place->query_dest_object(dir);
  if (!dest)
    return;

  tell_room(place, replace_string(replace_string(shored ?
            _LANG_WATER_OBJECT_SHORE_FROM : _LANG_WATER_OBJECT_DRIFT_FROM, "$O",
            name), "$T", ROOM_HAND->query_exit_dir(dir)) + "\n");

  if (ob->move(dest) != MOVE_OK)
    return;

  tell_room(dest, replace_string(shored ? _LANG_WATER_OBJECT_SHORE_TO :
            _LANG_WATER_OBJECT_DRIFT_TO, "$O", name) + "\n");
}

// Every thing waiting in this place is dragged once. What the current carries
// into the next cell is queued there, with its own time.
void water_drift_objects()
{
  object * obs;
  int i;

  obs = drift_objects ? drift_objects : ({ });
  drift_objects = ({ });
  drift_objects_due = 0;

  for (i = 0; i < sizeof(obs); i++)
    if (obs[i] && environment(obs[i]) == query_water_place() && query_water_flows())
      carry_object(obs[i]);
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
