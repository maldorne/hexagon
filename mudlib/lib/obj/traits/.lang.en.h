
// bleeding.c
// the body part when the wound names none, and its article
#define _LANG_TRAIT_BODY "body"
#define _LANG_TRAIT_BODY_ARTICLE "the"
// plural articles, which take "tus" instead of "tu" in Spanish; none here
#define _LANG_TRAIT_PLURAL_ARTICLES ({ })
#define _LANG_TRAIT_BLEEDING_ME "The wound on your " + part + " bleeds.\n"
#define _LANG_TRAIT_BLEEDING_ROOM "The wound on " + living->query_cap_name() + "'s " + \
      part + " bleeds.\n"
#define _LANG_TRAIT_BLEEDING_ENDS "The wound on your " + part + " finally closes.\n"
#define _LANG_TRAIT_BLEEDING_CURED "The wound on your " + part + " closes.\n"
#define _LANG_TRAIT_BLEEDING_LOOK "A wound bleeds on " + article + " " + part + ".\n"
#define _LANG_TRAIT_POSSESSIVE "your"
#define _LANG_TRAIT_POSSESSIVE_PLURAL "your"

// poisoned.c
#define _LANG_TRAIT_POISONED_ME "The poison burns you from inside, causing you great pain.\n"
#define _LANG_TRAIT_POISONED_ROOM living->query_cap_name() + " shudders in pain.\n"
#define _LANG_TRAIT_POISONED_RESISTED "You resist the poison.\n"
#define _LANG_TRAIT_POISONED_ENDS "The poison stops working.\n"
#define _LANG_TRAIT_POISONED_LOOK "Shows signs of poisoning.\n"

// deaf.c
#define _LANG_TRAIT_DEAF_STARTS "A deathly silence surrounds you: you cannot hear anything.\n"
#define _LANG_TRAIT_DEAF_ENDS "The deathly silence finally ends. Sounds from the outside world at last begin to filter through your ears.\n"
#define _LANG_TRAIT_DEAF_LOOK "Does not seem to hear anything.\n"

// mute.c
#define _LANG_TRAIT_MUTE_STARTS "Your voice fades: you cannot utter a word.\n"
#define _LANG_TRAIT_MUTE_ENDS "Your voice returns, and you can speak again.\n"
#define _LANG_TRAIT_MUTE_LOOK "Moves the mouth, but cannot speak.\n"
