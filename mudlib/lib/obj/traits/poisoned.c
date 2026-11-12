// Poisoned: venom in the blood that hurts a number of times. A living holds
// one; a worse poison added over it replaces it (see damage-over-time.c for
// the data).

#include <living/traits.h>
#include <living/damage-types.h>
#include <language.h>

inherit "/lib/obj/traits/damage-over-time.c";

/**
 * Applies the target's poison resistance (percent) to a new poison: it may be
 * resisted altogether (same percent chance), and what is not resisted has its
 * damage and pulses scaled down by that percent, never under 1. A negative
 * resistance (weakness) scales them up and is never resisted. Runs once per
 * poison ("resisted" marks the data).
 * @return 1 if the poison is resisted altogether
 */
private int resist_poison(object living, mapping data)
{
  int res;

  if (data["resisted"])
    return 0;
  data["resisted"] = 1;

  res = living->query_resistance(DAMAGE_POISON);
  if (res == 0)
    return 0;

  if (res >= 100 || (res > 0 && random(100) < res))
    return 1;

  // rounded up, so a weak poison is not scaled to nothing
  data["damage"] = (data["damage"] * (100 - res) + 99) / 100;
  data["left"] = (data["left"] * (100 - res) + 99) / 100;
  if (data["spread"])
    data["spread"] = (data["spread"] * (100 - res) + 99) / 100;
  if (data["damage"] < 1)
    data["damage"] = 1;
  if (data["left"] < 1)
    data["left"] = 1;

  return 0;
}

/**
 * A new poison: resisted ones are dropped, the rest scaled by the resistance.
 */
void trait_added(object living, mapping data)
{
  ::trait_added(living, data);

  if (resist_poison(living, data))
  {
    tell_object(living, _LANG_TRAIT_POISONED_RESISTED);
    living->remove_trait("poisoned", TRAIT_END_REMOVED);
  }
}

/**
 * A poison over another: resisted, it leaves the carried one alone; otherwise
 * the scaled one competes as usual.
 */
mapping trait_merge(object living, mapping old, mapping incoming)
{
  if (resist_poison(living, incoming))
  {
    tell_object(living, _LANG_TRAIT_POISONED_RESISTED);
    return old;
  }

  return ::trait_merge(living, old, incoming);
}

/**
 * The fixed state flag the poison raises, for the call sites that will ask for
 * it.
 */
mapping trait_effects(object living, mapping data)
{
  return ([ TRAIT_EFFECT_FLAGS : ({ "poisoned" }) ]);
}

/**
 * The poison burns: the living and the room are told.
 */
void trait_pulse(object living, mapping data)
{
  tell_object(living, _LANG_TRAIT_POISONED_ME);
  if (environment(living))
    tell_room(environment(living), _LANG_TRAIT_POISONED_ROOM, living);
}

/**
 * The poison wears off after its last pulse.
 */
void trait_removed(object living, mapping data, string reason)
{
  if (reason == TRAIT_END_EXPIRED && !living->query_dead())
    tell_object(living, _LANG_TRAIT_POISONED_ENDS);
}

string trait_look(object living, mapping data)
{
  return _LANG_TRAIT_POISONED_LOOK;
}
