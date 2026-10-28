// Poisoned: venom in the blood that hurts a number of times. A living holds
// one; a worse poison added over it replaces it (see damage-over-time.c for
// the data).

#include <living/traits.h>
#include <language.h>

inherit "/lib/obj/traits/damage-over-time.c";

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
