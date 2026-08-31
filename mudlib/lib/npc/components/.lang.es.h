// Strings for the NPC components under this directory.

// raise.c -- the service of returning a ghost to its body

#define _LANG_RAISE_INFO_VERBS ({ "info", "información", "informacion" })
#define _LANG_RAISE_VERBS ({ "resucitar", "resucitarme" })

#define _LANG_RAISE_INFO "Uhmm, si de verdad deseas volver a tu forma mortal, " + \
    "solo tienes que pedírmelo: escribe 'resucitar'."
#define _LANG_RAISE_NOT_DEAD "¿Por qué quieres resucitar si no lo necesitas?\n"
#define _LANG_RAISE_TOO_SOON "Resucitaste hace poco, tendrás que esperar un poco más.\n"
#define _LANG_RAISE_DONE_ME "Unas manos se alzan invocando a los poderes que atan " + \
    "el alma al cuerpo, y apareces de nuevo en tu forma mortal.\n"
#define _LANG_RAISE_DONE_ROOM this_player()->query_cap_name() + \
    " aparece de nuevo en su forma mortal.\n"
