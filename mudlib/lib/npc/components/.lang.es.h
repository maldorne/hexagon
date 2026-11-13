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
#define _LANG_RAISE_PAY_VERBS ({ "pagar" })
#define _LANG_RAISE_BUSY "Ahora no puedo invocar a los poderes, inténtalo en un momento.\n"
// price_text is the price in words, set by raise.c
#define _LANG_RAISE_INFO_CREDIT "Te devolveré a la vida, y lo que cuesta, " + price_text + \
    ", me lo deberás: pagarás cuando puedas con 'pagar'."
#define _LANG_RAISE_DEBT_TOLD "Serás devuelto a tu forma mortal, pero me deberás " + price_text + \
    ". No olvides tu deuda: escribe 'pagar' cuando tengas dinero."
#define _LANG_RAISE_DEBT_REMINDER "Aún me debes " + price_text + ". Escribe 'pagar' para saldarlo."
#define _LANG_RAISE_DEBT_REFUSED "No te devolveré a la vida mientras no me pagues lo que me debes: " + \
    price_text + ". Busca a otro que te devuelva la vida, y vuelve a pagarme.\n"
#define _LANG_RAISE_NO_DEBT "No me debes nada.\n"
#define _LANG_RAISE_DEBT_NO_MONEY "No llevas dinero con el que pagarme lo que me debes: " + price_text + ".\n"
#define _LANG_RAISE_DEBT_SETTLED "Tu deuda queda saldada. Si me necesitas de nuevo, aquí estaré."
// price_text is what was paid, left_text what is still owed
#define _LANG_RAISE_DEBT_PART_PAID "Recibo " + price_text + ". Aún me debes " + left_text + "."

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

// gatekeeper: the commands that close a door and lock it with the carried key
#define _LANG_GATEKEEPER_LOCK_COMMANDS(close, dir) ({ close + " " + dir + " con llave" })

// gift
#define _LANG_GIFT_BUSY "$npc$ está en mitad de un combate y ahora no puede atenderte."

// verb-response
#define _LANG_VERB_RESPONSE_BUSY "$npc$ está en mitad de un combate y ahora no puede atenderte."

// escort.c -- a guard taking a beaten criminal to prison

#define _LANG_ESCORT_ARRESTS "$guard$ se planta ante $who$, le echa los grilletes y declara: '¡En nombre de la ley, quedas detenido!'\n"
#define _LANG_ESCORT_ARRESTS_AWAY "Un guardia ha llegado hasta $who$ y lo ha detenido.\n"
#define _LANG_ESCORT_DRAG_FROM "$guard$ se lleva a rastras a $who$.\n"
#define _LANG_ESCORT_DRAG_TO "$guard$ llega arrastrando a $who$, inconsciente.\n"

// trainer.c -- the service of teaching skills for money
// skill_name, points, price_text, now, to and name/row/price (list) are set by trainer.c

#define _LANG_TRAINER_LIST_VERBS ({ "lista" })
#define _LANG_TRAINER_COST_VERBS ({ "coste" })
#define _LANG_TRAINER_TRAIN_VERBS ({ "entrenar" })
#define _LANG_TRAINER_UP_TO "hasta"

#define _LANG_TRAINER_BUSY "Ahora no puedo atenderte, estoy ocupado.\n"
#define _LANG_TRAINER_REFUSED "No tengo nada que enseñarte, todavía.\n"
#define _LANG_TRAINER_WHICH "¿Qué quieres aprender? Escribe 'lista' para ver lo que enseño."
#define _LANG_TRAINER_NOT_TAUGHT "Eso no lo enseño yo. Escribe 'lista' para ver lo que sé."
#define _LANG_TRAINER_UNKNOWN_SKILL "No conoces esa habilidad, no puedo mejorártela."
#define _LANG_TRAINER_TOO_LOW "Aún no sabes lo bastante de eso para que yo pueda ayudarte: practícalo tú primero."
#define _LANG_TRAINER_NOTHING_MORE "Ya sabes de eso todo lo que yo puedo enseñarte."
#define _LANG_TRAINER_COST "Mejorar " + skill_name + " " + points + " punto" + (points == 1 ? "" : "s") + \
    " hasta el " + ask[2] + "% te costará " + price_text + "."
#define _LANG_TRAINER_CANNOT_PAY "Serían " + price_text + ", y no llevas tanto dinero. Vuelve cuando lo tengas."
#define _LANG_TRAINER_DONE "Que así sea."
#define _LANG_TRAINER_TRAINED "Entrenas " + skill_name + " hasta el " + ask[2] + "% por " + price_text + ".\n"
#define _LANG_TRAINER_LIST_HEADER "Esto es lo que enseño:\n\n"
#define _LANG_TRAINER_LIST_LINE "  " + capitalize(name) + ": entre el " + \
    (mappingp(row) && intp(row["min"]) ? row["min"] : 0) + "% y el " + \
    (mappingp(row) && intp(row["max"]) ? row["max"] : 100) + "%, " + \
    (price > 0 ? price_words(price) + " cada punto" : "gratis") + ".\n"

// weapon masteries and levels (rows with "mastery"; min, max, now, value, left_text, cooldown_text
// and status are set by trainer.c)
#define _LANG_TRAINER_LEVEL "Aún no tienes experiencia suficiente: tienes que ser al menos de nivel " + \
    trainer_gate["min_level"] + " para que te enseñe nada.\n"
#define _LANG_TRAINER_MASTERY_ILLEGAL "Tu gremio no permite empuñar ninguna de esas armas, y no voy a enseñarte lo que no podrías usar."
#define _LANG_TRAINER_MASTERY_NEVER "Primero tienes que pelear con esa arma: no puedo enseñarte a manejar lo que nunca has empuñado."
#define _LANG_TRAINER_MASTERY_TOO_LOW "Solo enseño lo que luchar ya no puede darte, a partir del " + min + "% en " + \
    skill_name + ". Tú vas por el " + now + "%: sigue peleando con esas armas."
#define _LANG_TRAINER_MASTERY_FULL "Con " + skill_name + " has llegado al " + now + "%, y el " + max + "% es lo máximo que sé enseñar."
#define _LANG_TRAINER_COOLDOWN "Ya has aprendido de mí hace poco. Vuelve dentro de " + left_text + "."
#define _LANG_TRAINER_MASTERY_COST "Un punto de " + skill_name + " (del " + ask[1] + "% al " + ask[2] + "%) te costará " + \
    price_text + ". Enseño un punto cada vez" + (trainer_cooldown > 0 ? ", y no te daré otro hasta dentro de " + \
    cooldown_text : "") + "."
#define _LANG_TRAINER_MASTERY_TRAINED "Entrenas " + skill_name + ": ahora dominas el " + value + "% por " + price_text + \
    ". No podré darte otro punto hasta dentro de " + cooldown_text + ".\n"
#define _LANG_TRAINER_MASTERY_TRAINED_NOW "Entrenas " + skill_name + ": ahora dominas el " + value + "% por " + price_text + ".\n"
#define _LANG_TRAINER_VALUE_NONE "nada, nunca la has usado"
#define _LANG_TRAINER_MASTERY_LINE "  " + capitalize(name) + ": dominas " + value_text + "; enseño del " + min + "% al " + max + \
    "%, " + (price > 0 ? price_words(price) + " cada punto" : "gratis") + ", un punto por sesión. " + status + "\n"
#define _LANG_TRAINER_STATUS_ILLEGAL "Tu gremio no permite ninguna de esas armas."
#define _LANG_TRAINER_STATUS_NEVER "Primero tienes que pelear con esa arma."
#define _LANG_TRAINER_STATUS_TOO_LOW "Aún no llegas al " + min + "%: sigue peleando."
#define _LANG_TRAINER_STATUS_FULL "Ya no tengo más que enseñarte."
#define _LANG_TRAINER_STATUS_WAIT "Podrás entrenar otro punto dentro de " + left_text + "."
#define _LANG_TRAINER_STATUS_READY "Puedes entrenar un punto ahora."
