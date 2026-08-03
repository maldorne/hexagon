
// hearthstone.c

#define _LANG_HEARTHSTONE_NAME "piedra de hogar"
#define _LANG_HEARTHSTONE_SHORT "%^MAGENTA%^Piedra de Hogar%^RESET%^"
#define _LANG_HEARTHSTONE_ALIAS ({ "piedra", "hogar" })
#define _LANG_HEARTHSTONE_PLURAL "%^MAGENTA%^Piedras de Hogar%^RESET%^"
#define _LANG_HEARTHSTONE_PLURALS ({ "piedras", "piedras de hogar" })
#define _LANG_HEARTHSTONE_LONG "Es una piedra pequeña, de un color púrpura, con una " + \
    "extraña runa que no sabes descifrar tallada en uno de sus lados. Un aura " + \
    "blanquecina la rodea sin llegar a emitir luz, dándote la sensación de " + \
    "que es un objeto hermoso. Sin duda parece que posee poderes más " + \
    "allá de tu comprensión. Escribe 'ayuda piedra' para ver lo que puedes " + \
    "hacer con ella.\n"
#define _LANG_HEARTHSTONE_MARK_INFO (strlen(destination_name) ? "Tu piedra está marcada en: " + \
    destination_name + ".\n" : "Tu piedra no está marcada aún.\n")
#define _LANG_HEARTHSTONE_TRANSPORT_VERBS ({ "transportar" })
#define _LANG_HEARTHSTONE_MARK_VERBS ({ "marcar" })
#define _LANG_HEARTHSTONE_INVOKE_VERBS ({ "invocar" })

#define _LANG_HEARTHSTONE_ACTING "La piedra está actuando, espera un poco.\n"
#define _LANG_HEARTHSTONE_DEAD "Estando muerto no puedes hacer eso.\n"
#define _LANG_HEARTHSTONE_COMBATS "Debes terminar primero tus combates.\nEscribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n"
#define _LANG_HEARTHSTONE_LOCKED "La piedra aún no ha acumulado suficiente energía desde la última vez.\n"
#define _LANG_HEARTHSTONE_FORGOTTEN "Tu piedra ha olvidado el camino a casa... tendrás que marcar un nuevo lugar antes de intentarlo.\n"
#define _LANG_HEARTHSTONE_ORIGIN "Tu piedra ha olvidado el camino a casa... pero aún recuerda su lugar de origen inicial.\n"
#define _LANG_HEARTHSTONE_MSG_ME "La piedra de hogar comprende tus deseos y sus poderes se activan.\n"
#define _LANG_HEARTHSTONE_MSG_ROOM "La piedra de hogar de "+this_player()->query_cap_name()+" comienza a brillar con tonos púrpura.\n"

#define _LANG_HEARTHSTONE_MOVE "Si no paras de moverte será imposible que la piedra " + \
    "acumule energía suficiente para transportarte.\n"
#define _LANG_HEARTHSTONE_DEAD2 "Al morir tu piedra deja escapar la energía acumulada...\n"
#define _LANG_HEARTHSTONE_ERROR "Algo ha ido mal.\n"
#define _LANG_HEARTHSTONE_TRANSPORT_ME "Quedas cegado por un potente haz de luz " + \
    "púrpura que desprende la piedra, mientras notas cómo se calienta al " + \
    "tacto.\n\nNotas como el calor de la piedra desaparece y al difuminarse " + \
    "el haz de luz puedes distinguir dónde te encuentras.\n\n"
#define _LANG_HEARTHSTONE_TRANSPORT_ROOM " aparece repentinamente de entre " + \
    "un extraño humo púrpura.\n"

#define _LANG_HEARTHSTONE_MARK_FAIL "¿Marcar el qué? Quizá quieras 'marcar piedra'.\n"
#define _LANG_HEARTHSTONE_NOT_INN_FAIL "Sólo puedes hacer eso en una taberna.\n"
#define _LANG_HEARTHSTONE_DESTINATION "De acuerdo, a partir de ahora el destino de tu piedra de hogar será: "
#define _LANG_HEARTHSTONE_HELP "Puedes utilizar los siguientes comandos:\n\n" + \
          "\tmarcar          - en una taberna para que tu piedra de hogar recuerde el lugar.\n" + \
          "\ttransportar     - para que la piedra te transporte de vuelta al lugar marcado.\n" + \
          "\tinvocar montura - para traer a tu montura a tu lado (sólo al aire libre).\n"

#define _LANG_HEARTHSTONE_INVOKE_NAMES ({ "montura" })
#define _LANG_HEARTHSTONE_INVOKE_WHAT "¿Invocar el qué? Quizá quieras 'invocar montura'.\n"
#define _LANG_HEARTHSTONE_INVOKE_NO_MOUNT "Debes tener una montura para eso.\n"
#define _LANG_HEARTHSTONE_INVOKE_OUTSIDE "Sólo puedes hacer eso al aire libre.\n"
#define _LANG_HEARTHSTONE_INVOKE_NO_WATER "Hacer eso en el agua no es muy recomendable.\n"
#define _LANG_HEARTHSTONE_INVOKE_MOUNT_HERE "Tu montura ya se encuentra aquí.\n"
#define _LANG_HEARTHSTONE_INVOKE_MSG_ME "Tu montura aparece a tu lado.\n"
#define _LANG_HEARTHSTONE_INVOKE_MSG_ROOM "La montura de "+this_player()->query_cap_name()+" aparece a su lado.\n"

// diary.c

#define _LANG_DIARY_NAME "diario"
#define _LANG_DIARY_SHORT "Diario"
#define _LANG_DIARY_ALIAS ({ "diario", "libro" })
#define _LANG_DIARY_PLURAL "Diarios"
#define _LANG_DIARY_PLURALS ({ "diarios" })
#define _LANG_DIARY_LONG "Un libro gastado donde anotas tus aventuras y viajes. " + \
    "Escribe '%^BOLD%^diario explorador%^RESET%^' para ver los lugares que has descubierto.\n"
#define _LANG_DIARY_VERBS ({ "diario" })
#define _LANG_DIARY_EXPLORER_SUBVERBS ({ "explorador", "exploracion", "exploración", "lugares" })
#define _LANG_DIARY_PENDING_SUBVERBS ({ "viajes", "enciclopedia" })
#define _LANG_DIARY_HELP "El diario recoge tus andanzas. Comandos:\n" + \
    "\tdiario explorador  - lista los lugares que has descubierto.\n"
#define _LANG_DIARY_DEAD "Estando muerto no puedes hacer eso.\n"
#define _LANG_DIARY_WHAT "¿Qué quieres consultar en el diario? Prueba 'diario explorador'.\n"
#define _LANG_DIARY_NOT_YET "Esa sección del diario aún está en blanco.\n"
#define _LANG_DIARY_EXPLORER_EMPTY "Aún no has descubierto ningún lugar digno de mención.\n"
#define _LANG_DIARY_EXPLORER_HEADER "A lo largo de tus viajes has estado en:\n\n"
#define _LANG_DIARY_EXPLORER_COUNT_PRE "\nEn total, has visitado "
#define _LANG_DIARY_PLACE_SG "lugar"
#define _LANG_DIARY_PLACE_PL "lugares"
