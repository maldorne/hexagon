// Bridge: one cell that is a bridge and water at once. Shared by the bridge
// room base (/lib/bridge.c) and the bridge location component, which answer
// query_water_place() with the place itself.
//
// The cell is water (current, upstream mode, drift: see /lib/room/water.c)
// plus exits of type "bridge", which are walked; every other exit is swum.
// Each living in the cell is either on the bridge or in the water:
//   - it arrives on the bridge unless it comes through a swum exit (swum or
//     carried by the current), or is put in the water by a jump or a fall;
//   - on the bridge nothing drifts and only the bridge exits work;
//   - in the water it drifts, swims through the swum exits, and may climb
//     back (the climb skill) when the water is reachable from the bridge.
// What a place declares:
//   reachable  1 (default) the water can be reached from the bridge: one
//              climbs back up and fights across; 0 a bridge too high for it

#include <living/skills.h>
#include <room/water.h>
#include <room/bridge.h>
#include <room/room.h>
#include <language.h>

inherit water "/lib/room/water.c";

private int bridge_unreachable;

// who is in the water here (never saved)
static mapping swimmers;

int query_bridge() { return 1; }

int query_bridge_reachable() { return !bridge_unreachable; }
void set_bridge_reachable(int i) { bridge_unreachable = !i; }

// The water settings plus the bridge's own, for the conversion of a room into
// a location and for the component's own save.
mapping query_bridge_settings()
{
  return query_water_settings() + ([ "bridge_unreachable": bridge_unreachable ]);
}

void set_bridge_settings(mapping m)
{
  if (!m)
    return;
  set_water_settings(m);
  bridge_unreachable = m["bridge_unreachable"];
}

// Whether `who` is in the water below the bridge, not on it.
int query_in_water(object who)
{
  if (!who || !swimmers || !swimmers[who])
    return 0;
  return environment(who) == query_water_place();
}

// Put `who` on the bridge, silently.
void put_on_bridge(object who)
{
  if (swimmers)
    map_delete(swimmers, who);
}

// Put `who` in the water, silently; the drift starts and the strokes and the
// climb back are learned, as for anybody who reaches water.
void put_in_water(object who)
{
  if (!swimmers)
    swimmers = ([ ]);
  swimmers[who] = 1;

  if (interactive(who) && !who->query_known_skill(SKILL_CLIMB))
    who->add_known_skill(SKILL_CLIMB);

  water_event_enter(who);
}

// Something puts `who` in the water below the bridge (a plank breaking): they
// read `message` (a default when none), the others `room_message`, where $N is
// the name of who falls.
void drop_into_water(object who, varargs string message, string room_message)
{
  object place;

  place = query_water_place();
  if (!who || environment(who) != place || query_in_water(who))
    return;

  tell_object(who, message ? message : _LANG_BRIDGE_FALL_ME);
  tell_room(place, replace_string(room_message ? room_message :
            _LANG_BRIDGE_FALL_ROOM, "$N", who->query_cap_name()) + "\n", who);
  put_in_water(who);
}

// Jump from the bridge into the water: what 'jump' does.
int jump_into_water(object who)
{
  object place;

  place = query_water_place();
  if (!who || environment(who) != place)
    return 0;

  if (query_in_water(who))
  {
    tell_object(who, _LANG_BRIDGE_ALREADY_IN_WATER);
    return 0;
  }

  drop_into_water(who, _LANG_BRIDGE_JUMP_ME, _LANG_BRIDGE_JUMP_ROOM);
  return 1;
}

// Whether `who` came into the cell through a swum exit of `from`, the way
// into water, and not through a walked one or by no exit at all.
private int arrived_by_water(object who, object from)
{
  string dir;

  dir = from->query_exit_taken_by(who);
  if (!dir || from->query_dest_object(dir) != query_water_place())
    return 0;

  return from->query_swim_exit(dir) ? 1 : 0;
}

// A living arrives on the bridge unless it came by a swum exit; any other way
// in (login, teleport, a room load) leaves it on the bridge.
void bridge_event_enter(object ob, object from)
{
  if (!ob || !living(ob))
    return;

  if (from && arrived_by_water(ob, from))
    put_in_water(ob);
  else
    put_on_bridge(ob);
}

void bridge_event_exit(object ob)
{
  put_on_bridge(ob);
}

// Whether an exit of this type is walked on the bridge: the bridge's own, and
// a door or a gate across it. Every other exit of the cell is swum.
int query_bridge_exit_type(string type)
{
  return type == BRIDGE_EXIT_TYPE || type == "door" || type == "gate";
}

// The bridge exits are walked, so the swum ones are the rest; and only from
// the water.
string * query_swim_directions(object who)
{
  string * dirs, * ret;
  int i;

  if (!query_in_water(who))
    return ({ });

  dirs = water::query_swim_directions(who);
  ret = ({ });
  for (i = 0; i < sizeof(dirs); i++)
    if (!query_bridge_exit_type(query_water_place()->query_ex_type(dirs[i])))
      ret += ({ dirs[i] });

  return ret;
}

// Why `who` cannot take this exit by typing it, or nil: a swum one from the
// bridge, a bridge one from the water.
string query_exit_refusal(string dir, object who)
{
  object place;

  place = query_water_place();
  if (query_in_water(who))
  {
    if (query_bridge_exit_type(place->query_ex_type(dir)))
      return _LANG_BRIDGE_FROM_WATER;
    return nil;
  }

  if (place->query_swim_exit(dir))
    return _LANG_BRIDGE_ON_BRIDGE_NO_SWIM;

  return nil;
}

// Whoever is on the bridge and whoever is in the water reach each other only
// when the water is reachable. Returns why not, or nil.
string query_out_of_reach(object attacker, object victim)
{
  if (query_bridge_reachable() || !attacker || !victim)
    return nil;

  if (query_in_water(attacker) == query_in_water(victim))
    return nil;

  return query_in_water(attacker) ? _LANG_BRIDGE_OUT_OF_REACH_FROM_WATER :
                                    _LANG_BRIDGE_OUT_OF_REACH_FROM_BRIDGE;
}

// The climb back onto the bridge, as the climb skill asks of whatever is
// climbed.
int query_climbable() { return 1; }

string query_climb_refusal(object who)
{
  if (!query_in_water(who))
    return _LANG_BRIDGE_ALREADY_ON;
  if (!query_bridge_reachable())
    return _LANG_BRIDGE_TOO_HIGH;
  return nil;
}

// The current pulls at the climber too.
int query_climb_modifier(object who)
{
  return BRIDGE_CLIMB_MODIFIER - WATER_ACROSS_MALUS * query_water_strength();
}

void event_climbed(object who)
{
  put_on_bridge(who);
  tell_object(who, _LANG_BRIDGE_CLIMB_ME);
  tell_room(query_water_place(), replace_string(_LANG_BRIDGE_CLIMB_ROOM, "$N",
            who->query_cap_name()) + "\n", who);
}

void event_climb_failed(object who)
{
  lose_breath(who);
  tell_object(who, _LANG_BRIDGE_CLIMB_FAIL);
}

// The drift only takes whoever is in the water.
void water_drift(object ob)
{
  if (!query_in_water(ob))
    return;
  water::water_drift(ob);
}

// A line under the description: where one is.
string query_bridge_hint(object who)
{
  if (!query_in_water(who))
    return _LANG_BRIDGE_LOOK_ON;
  return query_bridge_reachable() ? _LANG_BRIDGE_LOOK_WATER_REACH :
                                    _LANG_BRIDGE_LOOK_WATER;
}

string query_bridge_glance(object who)
{
  if (!query_in_water(who))
    return _LANG_BRIDGE_GLANCE_ON;
  return query_water_look(who);
}

// Told on arriving in the water: the climb back, only while it is possible.
string * query_water_arrival_hints(object who)
{
  string * hints;

  hints = water::query_water_arrival_hints(who);
  if (query_bridge_reachable())
    hints += ({ _LANG_BRIDGE_HINT_CLIMB });
  return hints;
}

mixed * bridge_stats()
{
  return water_stats() + ({ ({ "Bridge Reachable", query_bridge_reachable(), }), });
}
