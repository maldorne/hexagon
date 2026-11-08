// Fire component. Attach to a location that is burning: whoever comes in is
// burned at once, and whoever stays is burned again every FIRE_PULSE seconds.
// The damage is of type fire, so the fire resistance of the living lowers it
// (100 is immune). Same structure as the acid component.
//
// A fire is permanent or timed. A timed one keeps the absolute time it ends
// at, saved with the location, and ends at the first check after that time:
// whenever a living enters or has a heart_beat here. No call_out is kept, and
// the fire survives a reboot. A fire that is `quenchable` can be doused (water, cold
// spells) with douse(), which takes seconds off the time left; a permanent
// fire cannot be doused.
//
// While it burns, a decorative fixture (/lib/obj/flames.c) stands in the
// location; it does nothing and goes with the fire.
//
// To set a location on fire from a spell or any code:
//   location->add_component(LOCATION_COMPONENT_FIRE,
//                           ([ "duration": 120, "quenchable": 1 ]));
// "duration" is in seconds (0 or missing: permanent). Adding it to a location
// that already burns reseeds the fire with the new settings.
//
// The stay is counted on the heart_beats of the livings in the fire (the
// location tells its components of each one).

#include <room/location.h>
#include <translations/combat.h>
#include <language.h>

inherit component "/lib/location/component.c";

// seconds between two burns of whoever stays
#define FIRE_PULSE 10
// damage when stepping in: FIRE_ENTER_BASE + random(FIRE_ENTER_SPREAD)
#define FIRE_ENTER_BASE 6
#define FIRE_ENTER_SPREAD 10
// damage of each burn while staying
#define FIRE_STAY_BASE 6
#define FIRE_STAY_SPREAD 10

// the decorative fixture of a burning location
#define FIRE_FLAMES "/lib/obj/flames"

// when the fire ends (time()), 0 for a permanent one
private int fire_end;
// whether douse() takes effect
private int fire_quenchable;
// when each living was last burned here (never saved)
static mapping burned;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FIRE);
  burned = ([ ]);
}

void init() {}

/**
 * The fire goes: its flames go with it. Called when the component is dropped,
 * by the end of the fire or by the location unloading.
 */
void dest_me()
{
  if (query_entity())
    query_entity()->remove_fixture(FIRE_FLAMES);
}

void initialize(object loc)
{
  component::initialize(loc);

  // a reloaded location keeps the flames in its fixtures: only a new fire
  // adds them
  if (!loc->query_fixtures()[FIRE_FLAMES])
    loc->add_fixture(FIRE_FLAMES);
}

/**
 * Whether the fire is permanent (it has no end time).
 */
int query_permanent() { return !fire_end; }

/**
 * Whether douse() can take time off this fire.
 */
int query_quenchable() { return fire_quenchable; }

/**
 * Seconds before a timed fire ends; 0 for a permanent one.
 */
int query_fire_time_left()
{
  int left;

  if (!fire_end)
    return 0;
  left = fire_end - time();
  return left > 0 ? left : 0;
}

/**
 * Puts the fire out: a message to the room and the component is dropped, which
 * takes the flames with it. Nothing of the component is used after this.
 */
void end_fire()
{
  object place;

  place = query_entity();
  if (!place)
    return;

  tell_room(place, _LANG_FIRE_OUT);
  place->remove_component(LOCATION_COMPONENT_FIRE);
}

/**
 * Ends a timed fire whose time is over. Called whenever somebody enters or has
 * a heart_beat here.
 *
 * @return 1 if the fire has ended
 */
int check_over()
{
  if (fire_end && time() >= fire_end)
  {
    end_fire();
    return 1;
  }
  return 0;
}

/**
 * Takes `seconds` off the time left of a timed, quenchable fire (a water or a
 * cold spell). A permanent fire, or one that cannot be quenched, is not
 * affected. When the time left reaches zero the fire ends.
 *
 * @param seconds  how much time to take off
 * @return 1 if the fire was doused, 0 if it did not respond
 */
int douse(int seconds)
{
  object place;

  if (!fire_end || !fire_quenchable || seconds < 1)
    return 0;

  place = query_entity();
  fire_end -= seconds;

  if (time() >= fire_end)
  {
    end_fire();
    return 1;
  }

  tell_room(place, _LANG_FIRE_DOUSED);
  // the shorter fire is saved with the location
  place->components_changed();
  return 1;
}

/**
 * Burns a living with damage of type fire, so its resistance counts. Whoever
 * is immune is not told anything.
 */
private void burn(object who, int damage, string message)
{
  burned[who] = time();
  if ((int)who->query_resistance(DAMAGE_FIRE) >= 100)
    return;

  tell_object(who, message);
  who->spell_damage(damage, DAMAGE_FIRE);
}

/**
 * Whether the fire burns this object: a living, alive.
 */
private int burns(object ob)
{
  return ob && living(ob) && !ob->query_dead();
}

/**
 * Whoever steps in is burned at once.
 */
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (!burns(ob) || check_over())
    return;

  burn(ob, FIRE_ENTER_BASE + random(FIRE_ENTER_SPREAD), _LANG_FIRE_ENTER);
}

/**
 * Whoever leaves is forgotten: coming back is a new step in.
 */
void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  map_delete(burned, ob);
}

/**
 * A living in the fire has had another heart_beat: it is burned again when
 * FIRE_PULSE seconds have passed since its last burn.
 *
 * @param who  the living
 */
void event_heart_beat(object who)
{
  if (!burns(who) || check_over())
    return;

  // already here when the component was loaded: its stay starts now
  if (!burned[who])
  {
    burned[who] = time();
    return;
  }

  if (time() - burned[who] < FIRE_PULSE)
    return;

  burn(who, FIRE_STAY_BASE + random(FIRE_STAY_SPREAD), _LANG_FIRE_BURN);
}

string query_info()
{
  return (fire_end ? "ends in " + query_fire_time_left() + "s" : "permanent") +
         (fire_quenchable ? ", quenchable" : ", not quenchable");
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
      "fire_end"       : fire_end,
      "fire_quenchable" : fire_quenchable,
    ]);
}

/**
 * Restores the saved fire (`fire_end`, `fire_quenchable`) or reads the
 * settings a caller gives: `duration` in seconds (0 or missing: permanent) and
 * `quenchable` (0 or 1).
 */
void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (args["fire_end"] != nil)
    fire_end = args["fire_end"];
  else if (args["duration"] != nil && args["duration"] > 0)
    fire_end = time() + args["duration"];
  else
    fire_end = 0;

  if (args["fire_quenchable"] != nil)
    fire_quenchable = args["fire_quenchable"];
  else
    fire_quenchable = args["quenchable"] ? 1 : 0;
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Fire time left", query_fire_time_left() }),
    ({ "Fire permanent", query_permanent() }),
    ({ "Fire quenchable", query_quenchable() }),
  });
}
