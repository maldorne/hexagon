// Base of the traits that hurt every so often (a wound that bleeds, a poison).
//
// data:
//   "damage"  hit points taken on each pulse
//   "spread"  a random 0 to this is added to "damage" on each pulse (optional)
//   "left"    pulses remaining; the trait ends when it reaches 0
//   "every"   heart_beats between two pulses (1 if missing)
//   "first"   heart_beats to the first pulse (optional, "every" if missing)
//   "source"  object name of whoever caused it, credited with the damage while
//             it is still around (optional)
//
// A trait built on this one gives its messages in trait_pulse, and its look
// line and ending in the usual hooks.

#include <living/traits.h>

inherit TRAIT_BASE;

/**
 * Tells the living and the room that the trait hurts now. Called on each pulse,
 * before the damage.
 */
void trait_pulse(object living, mapping data)
{
}

/**
 * Completes the data: the countdown to the first pulse.
 */
void trait_added(object living, mapping data)
{
  if (!data["every"])
    data["every"] = 1;

  if (undefinedp(data["wait"]))
    data["wait"] = data["first"] ? data["first"] : data["every"];
}

/**
 * Counts down and, on a pulse, gives the messages and takes the damage through
 * adjust_hp, crediting the source if it is still there.
 */
int trait_tick(object living, mapping data)
{
  object source;
  int damage;

  if (living->query_dead() || living->query_pacify_effect())
    return 1;

  data["wait"] -= 1;
  if (data["wait"] > 0)
    return 0;

  data["wait"] = data["every"];
  data["left"] -= 1;

  trait_pulse(living, data);

  source = data[TRAIT_DATA_SOURCE] ? find_object(data[TRAIT_DATA_SOURCE]) : nil;
  damage = data["damage"];
  if (data["spread"])
    damage += random(data["spread"] + 1);

  living->adjust_hp(-damage, source);

  return data["left"] <= 0;
}

/**
 * The worse of two keeps: the one with more damage still to take.
 */
mapping trait_merge(object living, mapping old, mapping incoming)
{
  if (incoming["damage"] * incoming["left"] > old["damage"] * old["left"])
    return incoming;

  return old;
}
