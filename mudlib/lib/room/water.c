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
//   kind      river | sea | lake, the body of water it is part of
//   depth     1 swimming at the surface, 2 deep water, harder to cross
//   submerged 0 at the surface, 1 or more how far below it: whoever is here
//             without a way to breathe drowns (see breathe_step)
//   current   the exit the water flows out by, nil for still water
//   strength  0 still .. 3 rapids, how hard the current pulls
//   upstream  free | hard | block, swimming any way but the current's
//   swept_to  the exit a failed or idle swimmer is carried out by, the
//             current's own when not set

#include <living/skills.h>
#include <room/water.h>
#include <room/room.h>
#include <basic/move.h>
#include <common/properties.h>
#include <mud/mudos.h>
#include <language.h>

private int water_depth, water_strength, water_submerged;
private string water_current, water_upstream, water_swept_to, water_kind;

// heart_beats each living has spent in this cell, in moving water (never saved)
static mapping drifting;

object query_water_place() { return this_object(); }

int query_water_environment() { return 1; }
int query_water() { return 1; }

// Whether `who` is in the water here: everybody is, in a plain water place.
int query_in_water(object who) { return 1; }

string query_water_kind() { return water_kind ? water_kind : ""; }
void set_water_kind(string kind) { water_kind = kind; }

// How far below the surface the place is: 0 at the surface.
int query_water_submerged() { return water_submerged; }
void set_water_submerged(int i) { water_submerged = i; }

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
      "water_kind"     : water_kind,
      "water_depth"    : water_depth,
      "water_submerged" : water_submerged,
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
  water_kind = m["water_kind"];
  // saves written before a setting existed lack its key: nil is not an int
  water_depth = m["water_depth"] ? m["water_depth"] : 0;
  water_submerged = m["water_submerged"] ? m["water_submerged"] : 0;
  water_current = m["water_current"];
  water_strength = m["water_strength"] ? m["water_strength"] : 0;
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

// Where one can swim from here, every swum exit, shores included. Nothing when
// there are none.
string query_water_swim_ways(object who)
{
  string * dirs, * ways;
  int i;

  dirs = query_swim_directions(who);
  ways = ({ });
  for (i = 0; i < sizeof(dirs); i++)
    ways += ({ ROOM_HAND->query_exit_dir(dirs[i]) });

  if (!sizeof(ways))
    return "";

  return _LANG_WATER_SWIM_WAYS + query_multiple_short(ways) + ".\n";
}

// What one sees from the water: where the shore is and where one can swim,
// never the exits.
string query_water_look(object who)
{
  string shore;

  shore = query_water_shore(who);
  return (strlen(shore) ? shore : _LANG_WATER_NO_SHORE) +
         query_water_swim_ways(who);
}

// What is worth telling somebody who has just arrived in the water, each
// hint only while it applies. Places add theirs.
string * query_water_arrival_hints(object who)
{
  string * hints;
  string line;

  hints = ({ });
  line = query_water_shore(who);
  if (strlen(line))
    hints += ({ line[0 .. strlen(line) - 2] });
  line = query_water_swim_ways(who);
  if (strlen(line))
    hints += ({ line[0 .. strlen(line) - 2] });
  return hints;
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
  if (water_submerged > 0)
    return _LANG_WATER_UNDER_LOOK;
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

/**
 * One heart_beat of somebody in moving water who does not swim on: the beats
 * spent here are counted, and when they add up to WATER_DRIFT_TIME seconds the
 * current drags them out by the exit it sweeps to (and may hurt them).
 *
 * @param who  a living in the water of this place
 */
private void drift_step(object who)
{
  int beats;

  if (who->query_swims() || water_strength <= 0 || !query_water_swept_to())
    return;

  if (!drifting)
    drifting = ([ ]);

  beats = drifting[who] ? drifting[who] + 1 : 1;
  drifting[who] = beats;

  if (beats < WATER_DRIFT_TIME / HEART_BEAT_TIME)
    return;

  // a stroke already on its way decides where they go: try again next beat
  if (who->query_active_effect("effect"))
    return;

  map_delete(drifting, who);

  // a death happens here, in the water
  if (hurt_by_current(who))
    return;

  tell_object(who, _LANG_WATER_DRIFT_ME);
  move_swimmer(who, query_water_swept_to(), _LANG_WATER_SWEPT_MESS);
}

/**
 * Whether the living can breathe under water: by a property of its own (a
 * spell, a nature), of something it wears or holds (a mask), or because it
 * swims by nature.
 *
 * @param who  the living
 * @return 1 if it never drowns
 */
int breathes_under_water(object who)
{
  object * equipment;
  int i;

  if (who->query_swims() || who->query_property(WATER_BREATHING_PROP))
    return 1;

  equipment = who->query_worn_ob() + who->query_held_ob();
  for (i = 0; i < sizeof(equipment); i++)
    if (equipment[i] && equipment[i]->query_property(WATER_BREATHING_PROP))
      return 1;

  return 0;
}

/**
 * One heart_beat of a living under the surface. Whoever breathes there has a
 * full lungful. The others spend one beat of breath (kept in WATER_BREATH_PROP
 * with the time of the beat; a gap of more than three beats means they
 * surfaced, left, or their heart_beat stopped, and the breath starts over) and,
 * with none left, lose a share of their life each beat.
 *
 * @param who  a living in the submerged water of this place
 */
private void breathe_step(object who)
{
  mixed breath;
  int max, left;

  if (who->query_dead())
    return;

  if (breathes_under_water(who))
  {
    who->remove_static_property(WATER_BREATH_PROP);
    return;
  }

  max = WATER_BREATH_BASE + WATER_BREATH_PER_CON * who->query_con();
  breath = who->query_static_property(WATER_BREATH_PROP);

  // the first beat without a way to breathe: the living holds its breath
  // a missing property reads as 0, not as an array
  if (!arrayp(breath) || time() - breath[1] > 3 * HEART_BEAT_TIME)
  {
    left = max;
    tell_object(who, _LANG_WATER_DROWN_START);
  }
  else
    left = breath[0];

  if (left > 0)
  {
    left--;
    if (left == max / 2)
      tell_object(who, _LANG_WATER_DROWN_HALF);
    else if (left == 2)
      tell_object(who, _LANG_WATER_DROWN_LAST);
    who->add_static_property(WATER_BREATH_PROP, ({ left, time() }));
    return;
  }

  who->add_static_property(WATER_BREATH_PROP, ({ 0, time() }));

  tell_object(who, _LANG_WATER_DROWN_ME);
  tell_room(query_water_place(), _LANG_WATER_DROWN_ROOM, who);

  who->adjust_hp(-(who->query_max_hp() * WATER_DROWN_HP_PERCENT / 100 + 1));
}

/**
 * One heart_beat of a living in a cell that is not submerged (the surface, an
 * air pocket). Whoever had spent breath under the water is told it breathes
 * again, once, and gets its full lungful back: the property is removed, so the
 * next dive starts a fresh count. A record older than three beats is stale and
 * is dropped without a word.
 *
 * @param who  a living in the water of this place, above the surface
 */
private void surface_step(object who)
{
  mixed breath;

  breath = who->query_static_property(WATER_BREATH_PROP);
  if (!arrayp(breath))
    return;

  who->remove_static_property(WATER_BREATH_PROP);
  if (!who->query_dead() && time() - breath[1] <= 3 * HEART_BEAT_TIME)
    tell_object(who, _LANG_WATER_SURFACE);
}

/**
 * A living in the water of this place has had another heart_beat: below the
 * surface it holds its breath or drowns, and in moving water it is dragged.
 * Above it (the surface, an air pocket) it breathes again.
 * The place passes it on from its own event_heart_beat. Whoever is not in the
 * water (on a bridge above it) feels nothing.
 *
 * @param who  the living
 */
void water_event_heart_beat(object who)
{
  if (!who || environment(who) != query_water_place() || !query_in_water(who))
    return;

  if (water_submerged > 0)
  {
    breathe_step(who);
    // the water may have killed it
    if (!who)
      return;
  }
  else
    surface_step(who);

  drift_step(who);
}

/**
 * Somebody leaves the place: the beats of the current they had spent are
 * forgotten.
 *
 * @param ob  who or what leaves
 */
void water_event_exit(object ob)
{
  if (drifting && ob)
    map_delete(drifting, ob);
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

/**
 * A thing that is not a living is in moving water: the current carries it on
 * at once. Onto the coast, out of the water, if this cell has one; otherwise
 * to the next cell downstream, whose own event_enter carries it on again, until
 * a coast takes it out or the river ends. Still water does nothing. The cells
 * crossed are counted in WATER_CARRY_PROP, so a current that loops stops after
 * WATER_CARRY_MAX_CELLS.
 *
 * @param ob  the thing in the water of this place
 */
void water_carry(object ob)
{
  object place, dest;
  string shore, dir, name;
  mixed cells;
  int shored, top;

  if (!ob || living(ob) || ob->get() || !query_water_flows())
    return;

  place = query_water_place();
  if (environment(ob) != place)
    return;

  cells = ob->query_static_property(WATER_CARRY_PROP);
  if (!cells)
    cells = 0;
  if (cells >= WATER_CARRY_MAX_CELLS)
    return;

  shore = query_shore_exit();
  shored = shore != nil;
  dir = shored ? shore : water_current;

  dest = place->query_dest_object(dir);
  if (!dest)
    return;

  name = ob->query_short();
  if (!name)
    name = "";

  // only the first cell of a carry clears the count, once the whole chain ends
  top = !cells;
  ob->add_static_property(WATER_CARRY_PROP, cells + 1);

  tell_room(place, replace_string(replace_string(shored ?
            _LANG_WATER_OBJECT_SHORE_FROM : _LANG_WATER_OBJECT_DRIFT_FROM, "$O",
            name), "$T", ROOM_HAND->query_exit_dir(dir)) + "\n");

  if (ob->move(dest) == MOVE_OK)
    tell_room(dest, replace_string(shored ? _LANG_WATER_OBJECT_SHORE_TO :
              _LANG_WATER_OBJECT_DRIFT_TO, "$O", name) + "\n");

  if (top && ob)
    ob->remove_static_property(WATER_CARRY_PROP);
}

// Anybody who reaches water knows from then on how to swim. A thing that comes
// in is carried off at once by a current; the strokes, the drift and the
// breath of a living are worked out on its heart_beats.
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
}

mixed * water_stats()
{
  return ({
      ({ "Water Kind", query_water_kind(), }),
      ({ "Water Depth", query_water_depth(), }),
      ({ "Water Submerged", water_submerged, }),
      ({ "Water Current", water_current, }),
      ({ "Water Strength", water_strength, }),
      ({ "Water Upstream", query_water_upstream(), }),
      ({ "Water Swept To", query_water_swept_to(), }),
    });
}
