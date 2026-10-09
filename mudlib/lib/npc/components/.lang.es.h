// Strings for the NPC components under this directory.

// raise.c -- the service of returning a ghost to its body

#define _LANG_RAISE_INFO_VERBS ({ "info", "información", "informacion" })
#define _LANG_RAISE_VERBS ({ "resucitar", "resucitarme" })

#define _LANG_RAISE_INFO "Uhmm, si de verdad deseas volver a tu forma mortal, " + \
    "solo tienes que pedírmelo: escribe 'resucitar'."
#define _LANG_RAISE_NOT_DEAD "¿Por qué quieres resucitar si no lo necesitas?\n"
#define _LANG_RAISE_TOO_SOON "Resucitaste hace poco, tendrás que esperar un poco más.\n"
#define _LANG_RAISE_GESTURE "Unas manos se alzan invocando a los poderes que " + \
    "atan el alma al cuerpo.\n"

// toll
#define _LANG_TOLL_VERBS ({ "pagar" })
#define _LANG_TOLL_BLOCKED "Eh, " + who->query_cap_name() + ", si quieres pasar, ¡paga!"
#define _LANG_TOLL_EXEMPT "Tú no tienes que pagar nada, pasa cuando quieras."
#define _LANG_TOLL_ALREADY_PAID "Gracias, pero tú ya has pagado."
#define _LANG_TOLL_NO_MONEY "¡Eh, tú! ¿Acaso quieres tomarme el pelo? No tienes dinero suficiente."
#define _LANG_TOLL_PAID "Muchas gracias, el camino está abierto para ti."
