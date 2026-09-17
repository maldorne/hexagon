
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

// mailer.c

#define _LANG_MAILER_PROMPT "correo> "
#define _LANG_MAILER_EMPTY "No tienes cartas.\n"
#define _LANG_MAILER_HEADERS_TITLE "Tus cartas ('?' para ver las órdenes):\n\n"
#define _LANG_MAILER_MARK_DELETED "B"
#define _LANG_MAILER_MARK_UNREAD "N"
#define _LANG_MAILER_NO_SUCH_LETTER "No tienes ninguna carta con ese número.\n"
#define _LANG_MAILER_WHICH_LETTERS "¿Qué cartas? Indica sus números: 3, 1-4 o 2,5.\n"
#define _LANG_MAILER_MARKED "Marcadas para borrar al salir.\n"
#define _LANG_MAILER_UNMARKED "Ya no se borrarán.\n"
#define _LANG_MAILER_LETTER_FROM "De: " + capitalize(letter["from"]) + "\n"
#define _LANG_MAILER_LETTER_TO "Para: " + list_names(letter["to"]) + "\n"
#define _LANG_MAILER_LETTER_CC "Copia: " + list_names(letter["cc"]) + "\n"
#define _LANG_MAILER_LETTER_DATE "Fecha: " + ctime(letter["date"], 4) + "\n"
#define _LANG_MAILER_LETTER_SUBJECT "Asunto: " + letter["subject"] + "\n"
#define _LANG_MAILER_NO_GROUPS "No tienes ningún grupo.\n"
#define _LANG_MAILER_GROUPS_TITLE "Tus grupos:\n\n"
#define _LANG_MAILER_GROUP_SYNTAX "Escribe g <grupo> <nombres> para añadir, y -<nombre> para quitar.\n"
#define _LANG_MAILER_GROUP_ADDED "Añadidos a " + group + ": " + list_names(done) + ".\n"
#define _LANG_MAILER_GROUP_NOT_ADDED "No se han añadido a " + group + " (no existen o ya estaban): " + \
  list_names(adding - done) + ".\n"
#define _LANG_MAILER_GROUP_REMOVED "Quitados de " + group + ": " + list_names(done) + ".\n"
#define _LANG_MAILER_GROUP_NOT_REMOVED "Nadie de esos está en " + group + ".\n"
#define _LANG_MAILER_HELP \
  "  <número>                  lee esa carta\n" + \
  "  l                         lista tus cartas\n" + \
  "  e <nombres>               escribe una carta\n" + \
  "  r <número>                responde a quien te la envió\n" + \
  "  v <número> <nombres>      reenvía una carta\n" + \
  "  b <números>               marca cartas para borrarlas al salir\n" + \
  "  d <números>               deja de marcarlas\n" + \
  "  g                         muestra tus grupos\n" + \
  "  g <grupo> <nombres>       añade nombres a un grupo (-<nombre> lo quita)\n" + \
  "  s                         sale, borrando las cartas marcadas\n" + \
  "  ?                         muestra esta ayuda\n"
#define _LANG_MAILER_CMD_LIST ({ "l" })
#define _LANG_MAILER_CMD_WRITE ({ "e" })
#define _LANG_MAILER_CMD_REPLY ({ "r" })
#define _LANG_MAILER_CMD_FORWARD ({ "v" })
#define _LANG_MAILER_CMD_DELETE ({ "b" })
#define _LANG_MAILER_CMD_UNDELETE ({ "d" })
#define _LANG_MAILER_CMD_GROUPS ({ "g" })
#define _LANG_MAILER_CMD_QUIT ({ "s" })
#define _LANG_MAILER_CMD_HELP ({ "?" })
#define _LANG_MAILER_WRITE_TO_WHOM "¿A quién? Escribe e <nombres>.\n"
#define _LANG_MAILER_REPLY_PREFIX "Re: "
#define _LANG_MAILER_FORWARD_SYNTAX "Escribe v <número> <nombres>.\n"
#define _LANG_MAILER_FORWARD_PREFIX "Rv: "
#define _LANG_MAILER_FORWARD_BODY "Carta reenviada por " + capitalize(owner_name) + ", escrita por " + \
  capitalize(letter["from"]) + " el " + ctime(letter["date"], 4) + ":\n\n"
#define _LANG_MAILER_UNKNOWN_RECIPIENTS "No hay nadie a quien escribir con esos nombres: " + \
  list_names(recipients["unknown"]) + ".\n"
#define _LANG_MAILER_NOBODY_TO_WRITE "La carta no tiene a quién ir.\n"
#define _LANG_MAILER_SENT "Carta enviada a " + list_names(delivered) + ".\n"
#define _LANG_MAILER_NOT_SENT "La carta no ha llegado a nadie.\n"
#define _LANG_MAILER_DELETED (number == 1 ? "Borrada una carta.\n" : "Borradas " + number + " cartas.\n")
#define _LANG_MAILER_CLOSED "Cierras tu buzón.\n"
#define _LANG_MAILER_UNKNOWN_COMMAND "No entiendo esa orden. Escribe '?' para verlas.\n"
#define _LANG_MAILER_ASK_SUBJECT "Asunto: "
#define _LANG_MAILER_NO_SUBJECT "(sin asunto)"
#define _LANG_MAILER_DISCARDED "Carta descartada.\n"
#define _LANG_MAILER_ASK_CC "Copia para (Intro para nadie): "

// editor.c

#define _LANG_EDITOR_START_MENU "Escribe el texto. %^BOLD%^**%^RESET%^ en una línea para ir al menú.\n"
#define _LANG_EDITOR_START_COMMAND "Escribe el texto. %^BOLD%^**%^RESET%^ en una línea para terminar, " + \
  "~h para ver las órdenes.\n"
#define _LANG_EDITOR_MENU_PROMPT "" + sizeof(lines) + (sizeof(lines) == 1 ? " línea" : " líneas") + \
  " - Elige entre IDLMC" + (can_use_ed() ? "E" : "") + "SQ, H para ayuda: "
#define _LANG_EDITOR_ASK_INSERT "Insertar antes de la línea: "
#define _LANG_EDITOR_ASK_DELETE "Borrar la línea o las líneas (ejemplo: 3 o 1..3): "
#define _LANG_EDITOR_ASK_MODIFY "Modificar la línea o las líneas (ejemplo: 3 o 1..3): "
#define _LANG_EDITOR_ASK_MODIFY_FROM "Texto a cambiar: "
#define _LANG_EDITOR_ASK_MODIFY_TO "Cambiarlo por: "
#define _LANG_EDITOR_ASK_QUIT "¿Seguro que quieres salir sin guardar el texto? (s/n): "
#define _LANG_EDITOR_YES ({ "s" })
#define _LANG_EDITOR_NO_LINES "Aún no hay ninguna línea.\n"
#define _LANG_EDITOR_CONTINUE "Sigues escribiendo al final. %^BOLD%^**%^RESET%^ en una línea para parar.\n"
#define _LANG_EDITOR_INSERTING "Escribes antes de la línea " + number + ". %^BOLD%^**%^RESET%^ en una " + \
  "línea para parar.\n"
#define _LANG_EDITOR_OUT_OF_RANGE "No hay líneas con esos números.\n"
#define _LANG_EDITOR_DELETED "Borrado.\n"
#define _LANG_EDITOR_ABORTED "No cambias nada.\n"
#define _LANG_EDITOR_DONE "Hecho.\n"
#define _LANG_EDITOR_BACK "Vuelves al editor.\n"
#define _LANG_EDITOR_SAVING "Guardas el texto.\n"
#define _LANG_EDITOR_QUITTING "Sales sin guardar.\n"
#define _LANG_EDITOR_UNKNOWN_COMMAND "No entiendo esa orden. " + \
  (mode == EDITOR_MODE_COMMAND ? "~h" : "H") + " muestra la ayuda.\n"
#define _LANG_EDITOR_ED_START "Entras en ed. %^BOLD%^x%^RESET%^ guarda y sale, " + \
  "%^BOLD%^q%^RESET%^ sale, %^BOLD%^h%^RESET%^ muestra la ayuda.\n"
#define _LANG_EDITOR_HELP \
  "  I - insertar líneas antes de un número de línea\n" + \
  "  D - borrar líneas: un número o un rango como 1..3\n" + \
  "  L - mostrar las líneas escritas\n" + \
  "  M - cambiar un texto por otro dentro de unas líneas\n" + \
  "  C - seguir escribiendo al final\n" + \
  (can_use_ed() ? "  E - pasar el texto al editor ed\n" : "") + \
  "  S - salir y guardar el texto\n" + \
  "  Q - salir sin guardar el texto\n" + \
  "  H - mostrar esta ayuda\n"
