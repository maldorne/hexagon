
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
#define _LANG_TRAIT_POISONED_ENDS "The poison stops working.\n"
#define _LANG_TRAIT_POISONED_LOOK "Shows signs of poisoning.\n"
