// Deaf: the living hears nothing. Speech, shouts, tells, whispers and the
// sounds of places do not reach it; it only sees that somebody speaks. What it
// says itself is not changed (see mute.c). Permanent when added without
// "expires", temporary with it. Raises the flag "deaf".

#include <living/traits.h>
#include <language.h>

inherit TRAIT_BASE;

/**
 * The state flag the hearing code asks for (query_trait_flag("deaf")).
 */
mapping trait_effects(object living, mapping data)
{
  return ([ TRAIT_EFFECT_FLAGS : ({ "deaf" }) ]);
}

/**
 * The living stops hearing.
 */
void trait_added(object living, mapping data)
{
  tell_object(living, _LANG_TRAIT_DEAF_STARTS);
}

/**
 * The hearing comes back, when it was not lost to death or replaced.
 */
void trait_removed(object living, mapping data, string reason)
{
  if (reason == TRAIT_END_REPLACED || reason == TRAIT_END_DEATH ||
      living->query_dead())
    return;

  tell_object(living, _LANG_TRAIT_DEAF_ENDS);
}

string trait_look(object living, mapping data)
{
  return _LANG_TRAIT_DEAF_LOOK;
}

/**
 * A permanent deafness stays; between temporary ones the longer lasts.
 */
mapping trait_merge(object living, mapping old, mapping incoming)
{
  if (!old[TRAIT_DATA_EXPIRES])
    return old;

  if (!incoming[TRAIT_DATA_EXPIRES] ||
      (int)incoming[TRAIT_DATA_EXPIRES] > (int)old[TRAIT_DATA_EXPIRES])
    return incoming;

  return old;
}
