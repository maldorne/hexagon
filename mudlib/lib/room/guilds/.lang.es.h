// guild entrances and guild rooms. `me` is the player joining or leaving,
// `guild` the guild object, `wait` the seconds of play left to wait

#define _LANG_GUILD_ENTRANCE_CMDS_JOIN ({ "alistarse", "alistar", "alistarme" })
#define _LANG_GUILD_ENTRANCE_CMDS_LEAVE ({ "renunciar" })
#define _LANG_GUILD_ENTRANCE_YES ({ "s", "si", "sí" })

#define _LANG_GUILD_ENTRANCE_SIGN_LONG "Un cartel cuelga de la pared, junto a la entrada.\n"
#define _LANG_GUILD_ENTRANCE_SIGN_NO_GUILD "Este gremio no admite a nadie ahora mismo."
#define _LANG_GUILD_ENTRANCE_SIGN_TITLE "Gremio: "
#define _LANG_GUILD_ENTRANCE_SIGN_RACES "Admite a: "
#define _LANG_GUILD_ENTRANCE_SIGN_CLASSES "Clases: "
#define _LANG_GUILD_ENTRANCE_SIGN_COMMANDS \
        "  alistarse    te conviertes en miembro del gremio\n" + \
        "  renunciar    abandonas el gremio al que perteneces\n\n" + \
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
        "alistarte en ningún otro.\n"
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

#define _LANG_GUILD_ROOM_CMDS_MEMBERS ({ "miembros" })
#define _LANG_GUILD_ROOM_SIGN_LONG "Un cartel cuelga de la pared con lo que se puede hacer aquí.\n"
#define _LANG_GUILD_ROOM_SIGN_TEXT "Salón del gremio\n\n" + \
        "  miembros    los miembros más famosos del gremio"
#define _LANG_GUILD_ROOM_NOW_FAMOUS "Ahora formas parte de la lista de los " + \
        "miembros más famosos de tu gremio.\n"
#define _LANG_GUILD_ROOM_NOW_FAMOUS_OVER "Ahora formas parte de la lista de los " + \
        "miembros más famosos de tu gremio: has superado a " + lowest + ".\n"
#define _LANG_GUILD_ROOM_NO_FAMOUS "Aún no hay nadie en la lista de los miembros " + \
        "más famosos de este gremio.\n"
#define _LANG_GUILD_ROOM_FAMOUS_HEADER "Los miembros más famosos del gremio:\n\n"
#define _LANG_GUILD_ROOM_LEVEL "nivel"
