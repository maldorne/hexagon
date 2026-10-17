// guild entrances and guild rooms. `me` is the player joining or leaving,
// `guild` the guild object, `wait` the seconds of play left to wait

#define _LANG_GUILD_ENTRANCE_CMDS_JOIN ({ "alistarse", "alistar", "alistarme" })
#define _LANG_GUILD_ENTRANCE_CMDS_LEAVE ({ "renunciar" })
#define _LANG_GUILD_ENTRANCE_YES ({ "s", "si", "sí" })
#define _LANG_GUILD_CMDS_RULES ({ "normas" })

#define _LANG_GUILD_ENTRANCE_SIGN_LONG "Un cartel cuelga de la pared, junto a la entrada.\n"
#define _LANG_GUILD_ENTRANCE_SIGN_NO_GUILD "Este gremio no admite a nadie ahora mismo."
#define _LANG_GUILD_ENTRANCE_SIGN_TITLE "Gremio: "
#define _LANG_GUILD_ENTRANCE_SIGN_RACES "Admite a: "
#define _LANG_GUILD_ENTRANCE_SIGN_CLASSES "Clases: "
#define _LANG_GUILD_ENTRANCE_SIGN_COMMANDS \
        "  alistarse    te conviertes en miembro del gremio\n" + \
        "  renunciar    abandonas el gremio al que perteneces\n" + \
        "  normas       quién puede entrar, qué se aprende y las normas del gremio\n\n" + \
        "Después de alistarte en un gremio tendrás que esperar " + \
        (GUILD_JOIN_WAIT / 3600) + " horas de juego antes de poder alistarte " + \
        "en otro."

#define _LANG_GUILD_ENTRANCE_NO_GUILD "Este gremio no admite a nadie ahora mismo.\n"
#define _LANG_GUILD_ENTRANCE_ALREADY_MEMBER "Ya eres miembro de este gremio.\n"
#define _LANG_GUILD_ENTRANCE_OTHER_GUILD "Ya perteneces a un gremio: tendrás que " + \
        "renunciar a él antes de alistarte en otro.\n"
#define _LANG_GUILD_ENTRANCE_NOT_LEGAL "No reúnes las condiciones para alistarte " + \
        "en este gremio.\n"
#define _LANG_GUILD_ENTRANCE_TOO_SOON "Hace muy poco que te alistaste en un " + \
        "gremio: tendrás que jugar " + seconds_to_time_string(wait) + " más antes " + \
        "de alistarte en otro.\n"
#define _LANG_GUILD_ENTRANCE_JOINED "Te alistas en el gremio. Durante las " + \
        "próximas " + (GUILD_JOIN_WAIT / 3600) + " horas de juego no podrás " + \
        "alistarte en ningún otro. Escribe 'normas' para leer las normas del " + \
        "gremio.\n"
#define _LANG_GUILD_ENTRANCE_JOINED_OTHERS me->query_cap_name() + " se alista " + \
        "en el gremio.\n"
#define _LANG_GUILD_ENTRANCE_NO_GUILD_TO_LEAVE "No perteneces a ningún gremio.\n"
#define _LANG_GUILD_ENTRANCE_CANNOT_LEAVE "Por alguna razón no te está permitido " + \
        "abandonar tu gremio.\n"
#define _LANG_GUILD_ENTRANCE_LEAVE_WAIT_WARNING "Te alistaste hace poco: si " + \
        "renuncias, no podrás alistarte en ningún gremio hasta que juegues " + \
        seconds_to_time_string(wait) + " más.\n"
#define _LANG_GUILD_ENTRANCE_LEAVE_CONFIRM "¿Seguro que quieres renunciar a tu " + \
        "gremio? (s/n) "
#define _LANG_GUILD_ENTRANCE_LEAVE_CANCELLED "De acuerdo, sigues en tu gremio.\n"
#define _LANG_GUILD_ENTRANCE_LEFT "Renuncias a tu gremio: a partir de ahora no " + \
        "perteneces a ninguno.\n"

#define _LANG_GUILD_ROOM_CMDS_COST ({ "coste" })
#define _LANG_GUILD_ROOM_CMDS_ADVANCE ({ "avanzar" })
#define _LANG_GUILD_ROOM_SIGN_LONG "Un cartel cuelga de la pared con lo que se puede hacer aquí.\n"
#define _LANG_GUILD_ROOM_SIGN_TEXT "Salón del gremio\n\n" + \
        "  coste      lo que te falta para tu siguiente nivel de gremio\n" + \
        "  avanzar    subes un nivel de gremio, si tienes esa experiencia\n" + \
        "  normas     las normas del gremio\n\n" + \
        "Solo los miembros del gremio pueden avanzar aquí."
#define _LANG_GUILD_ROOM_NO_GUILD "Este salón no pertenece a ningún gremio ahora mismo.\n"
#define _LANG_GUILD_ROOM_NOT_MEMBER "Solo los miembros de este gremio pueden avanzar aquí.\n"
#define _LANG_GUILD_ROOM_MAX_LEVEL "Ya has alcanzado el nivel más alto de tu gremio.\n"
#define _LANG_GUILD_ROOM_NOT_ENOUGH_XP "Aún no tienes experiencia suficiente para avanzar: " + \
        "tienes " + have + " y necesitas " + cost + ".\n"
#define _LANG_GUILD_ROOM_ADVANCED_OTHERS me->query_cap_name() + " avanza un nivel en el gremio.\n"
#define _LANG_GUILD_ROOM_COST "Para avanzar al nivel " + next + " de tu gremio " + \
        "necesitas " + cost + " puntos de experiencia (tienes " + have + ").\n"
