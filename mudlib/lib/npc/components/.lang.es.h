// Strings for the NPC components under this directory.

// raise.c -- the service of returning a ghost to its body

#define _LANG_RAISE_INFO_VERBS ({ "preguntar" })
#define _LANG_RAISE_VERBS ({ "resucitar", "resucitarme" })

#define _LANG_RAISE_INFO "Uhmm, si de verdad deseas volver a tu forma mortal, " + \
    "solo tienes que pedírmelo: escribe 'resucitar'."
#define _LANG_RAISE_NOT_DEAD "¿Por qué quieres resucitar si no lo necesitas?\n"
#define _LANG_RAISE_REFUSED "Tu alma aún no es lo bastante pura para que pueda devolverte a la vida.\n"
#define _LANG_RAISE_TOO_SOON "Resucitaste hace poco, tendrás que esperar un poco más.\n"
#define _LANG_RAISE_GESTURE "Unas manos se alzan invocando a los poderes que " + \
    "atan el alma al cuerpo.\n"
// price_text is the price in words, set by raise.c
#define _LANG_RAISE_INFO_PRICE "Mis servicios cuestan " + price_text + "."
#define _LANG_RAISE_CANNOT_PAY "Devolverte a la vida cuesta " + price_text + \
    ", y no veo esa cantidad aquí, ni contigo ni en tu cuerpo.\n"

// heal-service.c -- the service of healing whoever asks, for a price
// price_text is a price in words and who the player, set by heal-service.c

#define _LANG_HEAL_SERVICE_INFO_VERBS ({ "preguntar" })
#define _LANG_HEAL_SERVICE_VERBS ({ "curar", "curarme" })

#define _LANG_HEAL_SERVICE_INFO "Puedo atender tus heridas: escribe 'curar' y me " + \
    "ocuparé de ellas."
#define _LANG_HEAL_SERVICE_INFO_PRICE "Cobro " + price_text + " por atenderte."
#define _LANG_HEAL_SERVICE_DEAD "Atiendo a los vivos, no a los fantasmas. Haz que te resuciten primero.\n"
#define _LANG_HEAL_SERVICE_FIGHTING "¡No mientras estás luchando! Termina eso primero.\n"
#define _LANG_HEAL_SERVICE_REFUSED "No puedo ayudarte.\n"
#define _LANG_HEAL_SERVICE_TOO_SOON "Te atendí hace un momento, dale tiempo a tu cuerpo.\n"
#define _LANG_HEAL_SERVICE_NOT_NEEDED "Te veo bien, no tengo nada que curar.\n"
#define _LANG_HEAL_SERVICE_CANNOT_PAY "Serían " + price_text + ", y no llevas tanto dinero. " + \
    "Vuelve cuando lo tengas.\n"
#define _LANG_HEAL_SERVICE_BUSY "Ahora no puedo atenderte, espera un momento.\n"
#define _LANG_HEAL_SERVICE_START "Quédate quieto, que empiezo."

// toll
#define _LANG_TOLL_VERBS ({ "pagar" })
#define _LANG_TOLL_BLOCKED "Eh, " + who->query_cap_name() + ", si quieres pasar, ¡paga!"
#define _LANG_TOLL_EXEMPT "Tú no tienes que pagar nada, pasa cuando quieras."
#define _LANG_TOLL_ALREADY_PAID "Gracias, pero tú ya has pagado."
#define _LANG_TOLL_NO_MONEY "¡Eh, tú! ¿Acaso quieres tomarme el pelo? No tienes dinero suficiente."
#define _LANG_TOLL_PAID "Muchas gracias, el camino está abierto para ti."

// guard
#define _LANG_GUARD_WANTED_BLOCKED "¡Alto! La ley te busca, no pasarás."
#define _LANG_GUARD_WANTED_SHOUT "¡Alto en nombre de la ley!"
#define _LANG_GUARD_RACE_BLOCKED "No dejamos pasar a los de tu pueblo."
#define _LANG_GUARD_NIGHT_BLOCKED "De noche las puertas no se abren a los forasteros."

// gift
#define _LANG_GIFT_BUSY "$npc$ está en mitad de un combate y ahora no puede atenderte."

// verb-response
#define _LANG_VERB_RESPONSE_BUSY "$npc$ está en mitad de un combate y ahora no puede atenderte."
