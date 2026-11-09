// Mute: the living cannot speak. It cannot say, whisper, shout or tell, nor
// cast a spell that is chanted; emotes and skills are not speech and stay
// possible. What it hears is not changed (see deaf.c). Permanent when added
// without "expires", temporary with it. Raises the flag "mute".

#include <living/traits.h>
#include <language.h>

inherit TRAIT_BASE;

/**
 * The state flag the speech code asks for (query_trait_flag("mute")).
 */
mapping trait_effects(object living, mapping data)
{
  return ([ TRAIT_EFFECT_FLAGS : ({ "mute" }) ]);
}

/**
 * The living loses its voice.
 */
void trait_added(object living, mapping data)
{
  tell_object(living, _LANG_TRAIT_MUTE_STARTS);
}

/**
 * The voice comes back, when it was not lost to death or replaced.
 */
void trait_removed(object living, mapping data, string reason)
{
  if (reason == TRAIT_END_REPLACED || reason == TRAIT_END_DEATH ||
      living->query_dead())
    return;

  tell_object(living, _LANG_TRAIT_MUTE_ENDS);
}

string trait_look(object living, mapping data)
{
  return _LANG_TRAIT_MUTE_LOOK;
}

/**
 * A permanent muteness stays; between temporary ones the longer lasts.
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
