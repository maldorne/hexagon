// Bleeding: an open wound on a body part that bleeds a number of times. A
// living holds one; a worse wound added over it replaces it (see
// damage-over-time.c for the rest of the data).
//
// data, besides the ones of damage-over-time.c:
//   "part"     the name of the body part it is on, in the mud's language
//   "article"  its article ("la", "los"...), for the messages; optional
//
// Whoever adds it names the part from the body of the living (see rend.c in a
// game for one that does).

#include <living/traits.h>
#include <language.h>

inherit "/lib/obj/traits/damage-over-time.c";

/**
 * The fixed state flag the wound raises, for the call sites that will ask for
 * it.
 */
mapping trait_effects(object living, mapping data)
{
  return ([ TRAIT_EFFECT_FLAGS : ({ "bleeding" }) ]);
}

/**
 * The name of the part and its article, with the fallback when the wound
 * names none.
 *
 * @return ({ part, article, possessive })
 */
private string * wound_words(mapping data)
{
  string part, article, possessive;

  part = data["part"] ? data["part"] : _LANG_TRAIT_BODY;
  article = data["article"] ? data["article"] : _LANG_TRAIT_BODY_ARTICLE;
  possessive = member_array(article, _LANG_TRAIT_PLURAL_ARTICLES) != -1 ?
    _LANG_TRAIT_POSSESSIVE_PLURAL : _LANG_TRAIT_POSSESSIVE;

  return ({ part, article, possessive });
}

/**
 * The wound bleeds: the living and the room are told which part.
 */
void trait_pulse(object living, mapping data)
{
  string * words;
  string part, article, possessive;

  words = wound_words(data);
  part = words[0];
  article = words[1];
  possessive = words[2];

  tell_object(living, _LANG_TRAIT_BLEEDING_ME);
  if (environment(living))
    tell_room(environment(living), _LANG_TRAIT_BLEEDING_ROOM, living);
}

/**
 * The wound closes by itself after its last bleeding.
 */
void trait_removed(object living, mapping data, string reason)
{
  string * words;
  string part, article, possessive;

  if (reason != TRAIT_END_EXPIRED || living->query_dead())
    return;

  words = wound_words(data);
  part = words[0];
  article = words[1];
  possessive = words[2];

  tell_object(living, _LANG_TRAIT_BLEEDING_ENDS);
}

string trait_look(object living, mapping data)
{
  string * words;
  string part, article, possessive;

  words = wound_words(data);
  part = words[0];
  article = words[1];
  possessive = words[2];

  return _LANG_TRAIT_BLEEDING_LOOK;
}
