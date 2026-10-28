
// bleeding.c
// the body part when the wound names none, and its article
#define _LANG_TRAIT_BODY "cuerpo"
#define _LANG_TRAIT_BODY_ARTICLE "el"
// plural articles, which take "tus" instead of "tu"
#define _LANG_TRAIT_PLURAL_ARTICLES ({ "los", "las" })
#define _LANG_TRAIT_BLEEDING_ME "La herida de " + possessive + " " + part + " sangra.\n"
#define _LANG_TRAIT_BLEEDING_ROOM "La herida de " + article + " " + part + " de " + \
      living->query_cap_name() + " sangra.\n"
#define _LANG_TRAIT_BLEEDING_ENDS "La herida de " + possessive + " " + part + " se cierra por fin.\n"
#define _LANG_TRAIT_BLEEDING_LOOK "Sangra por una herida en " + article + " " + part + ".\n"
#define _LANG_TRAIT_POSSESSIVE "tu"
#define _LANG_TRAIT_POSSESSIVE_PLURAL "tus"

// poisoned.c
#define _LANG_TRAIT_POISONED_ME "El veneno te quema por dentro causándote un gran dolor.\n"
#define _LANG_TRAIT_POISONED_ROOM living->query_cap_name() + " se estremece de dolor.\n"
#define _LANG_TRAIT_POISONED_ENDS "El veneno deja de hacer efecto.\n"
#define _LANG_TRAIT_POISONED_LOOK "Muestra signos de envenenamiento.\n"
