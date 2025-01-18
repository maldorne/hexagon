
#define _LANG_CALENDAR_BIRTHDAY_STRING day+" de "+month(month-1)

// postal

#define _LANG_POSTAL_GROUP_CODERS "programadores"
#define _LANG_POSTAL_GROUP_ADMINS "admins"
#define _LANG_POSTAL_GROUP_PATRONS "patrones"
#define _LANG_POSTAL_DOES_NOT_EXIST_USER_OR_GROUP "No existe el usuario o grupo: "+capitalize(who[i])+".\n"
#define _LANG_POSTAL_DOES_NOT_EXIST_USER "No existe el usuario: "+capitalize(g[i])+".\n"
#define _LANG_POSTAL_NEW_MAIL "  Nuevo mail ha llegado desde $N\n  Asunto: $S"
#define _LANG_POSTAL_OLD_MAIL_1 "Mail antiguo #"+low+"\n"
#define _LANG_POSTAL_OLD_MAIL_2 "Mails antiguos #"+low+" y "+"#"+high+"\n"
#define _LANG_POSTAL_OLD_MAIL_3 "Mails antiguos #"+low+" hasta"+" #"+high+"\n"

// weapons

#define _LANG_WEAPONS_SLASH_MSG_ME "Cortas"
#define _LANG_WEAPONS_SLASH_MSG_HIM " te corta"
#define _LANG_WEAPONS_SLASH_MSG_ENV " corta"
#define _LANG_WEAPONS_PIERCE_MSG_ME "Perforas"
#define _LANG_WEAPONS_PIERCE_MSG_HIM " te perfora"
#define _LANG_WEAPONS_PIERCE_MSG_ENV " perfora"
#define _LANG_WEAPONS_BLUNT_MSG_ME "Golpeas"
#define _LANG_WEAPONS_BLUNT_MSG_HIM " te golpea"
#define _LANG_WEAPONS_BLUNT_MSG_ENV " golpea"
#define _LANG_WEAPONS_FIRE_MSG_ME "Quemas"
#define _LANG_WEAPONS_FIRE_MSG_HIM " te quema"
#define _LANG_WEAPONS_FIRE_MSG_ENV " quema"
#define _LANG_WEAPONS_COLD_MSG_ME "Congelas"
#define _LANG_WEAPONS_COLD_MSG_HIM " te congela"
#define _LANG_WEAPONS_COLD_MSG_ENV " congela"

#define _LANG_WEAPONS_NO_EFFECT " sin efecto"
#define _LANG_WEAPONS_WEAKLY " débilmente"
#define _LANG_WEAPONS_WITH_LITTLE_FORCE " con poca fuerza"
#define _LANG_WEAPONS_NORMAL ""
#define _LANG_WEAPONS_WITH_FORCE " con fuerza"
#define _LANG_WEAPONS_WITH_MUCH_FORCE " con mucha fuerza"
#define _LANG_WEAPONS_VIOLENTLY " violentamente"
#define _LANG_WEAPONS_INCREDIBLE_FORCE " con una increíble fuerza"
#define _LANG_WEAPONS_SUPERHUMAN_FORCE " con una fuerza sobrehumana"

#define _LANG_WEAPONS_WHO_ME " a "+defender->query_cap_name()
#define _LANG_WEAPONS_WHO_HIM ""
#define _LANG_WEAPONS_WHO_ENV " a "+defender->query_cap_name()

#define _LANG_WEAPONS_WHERE_RELATIVE_ME " en su "+where->query_name()
#define _LANG_WEAPONS_WHERE_RELATIVE_HIM " en tu "+where->query_name()
#define _LANG_WEAPONS_WHERE_RELATIVE_ENV " en su "+where->query_name()

#define _LANG_WEAPONS_WHERE_ME " "+where->query_article()+" "+where->query_name()+" de "+defender->query_cap_name()
#define _LANG_WEAPONS_WHERE_HIM " "+where->query_article()+" "+where->query_name()
#define _LANG_WEAPONS_WHERE_ENV " "+where->query_article()+" "+where->query_name()+" de "+defender->query_cap_name()

#define _LANG_WEAPONS_LOC_ME " en "+localization
#define _LANG_WEAPONS_LOC_HIM " en "+localization
#define _LANG_WEAPONS_LOC_ENV " en "+localization

// #define _LANG_WEAPONS_NO_LOC_ME " a "+defender->query_cap_name()
// #define _LANG_WEAPONS_NO_LOC_ENV " a "+defender->query_cap_name()

#define _LANG_WEAPONS_SLASHING_MSG_1 ", haciendo graves cortes"
#define _LANG_WEAPONS_SLASHING_MSG_2 ", dejando heridas severas"
#define _LANG_WEAPONS_SLASHING_MSG_3 ", dejando profundas heridas"
#define _LANG_WEAPONS_SLASHING_MSG_4 ", dejando una sangrante herida"
#define _LANG_WEAPONS_SLASHING_MSG_5 ", dejando una ligera herida"
#define _LANG_WEAPONS_SLASHING_MSG_6 ", haciendo solo rasguños"
#define _LANG_WEAPONS_SLASHING_MSG_7 ", dejando una herida mortal"
#define _LANG_WEAPONS_NON_SLASHING_MSG_1 ", dejando graves heridas internas"
#define _LANG_WEAPONS_NON_SLASHING_MSG_2 ", haciendo graves heridas"
#define _LANG_WEAPONS_NON_SLASHING_MSG_3 ", provocando un terrible dolor"
#define _LANG_WEAPONS_NON_SLASHING_MSG_4 ", provocando grandes dolores"
#define _LANG_WEAPONS_NON_SLASHING_MSG_5 ", provocando serios dolores"
#define _LANG_WEAPONS_NON_SLASHING_MSG_6 ", provocando ligeros moratones"
#define _LANG_WEAPONS_NON_SLASHING_MSG_7 ", provocando mortales heridas"

// shutdown.c

#define _LANG_SHUTDOWN_NAME "heraldo"
#define _LANG_SHUTDOWN_SHORT "El Heraldo de los Dioses"
#define _LANG_SHUTDOWN_LONG "Un ángel celestial que está esperando " + \
           "a que los Dioses le indiquen cuando llegará una nueva Era.\n"
#define _LANG_SHUTDOWN_LONG2 "Un ángel celestial, de belleza inconmensurable, firmemente concentrado " + \
           "en la cuenta atrás.\n"
#define _LANG_SHUTDOWN_EXTRA_LONG "El destino del mundo se decidirá en "+(time_of_crash - time())+ \
      " segundos.\n"
#define _LANG_SHUTDOWN_IN_SECONDS "El destino del mundo se decidirá en "+time_to_crash+" segundos."
#define _LANG_SHUTDOWN_IN_MINUTES "El destino del mundo se decidirá en "+time_to_crash+" minutos."
#define _LANG_SHUTDOWN_IN_ONE_MINUTE "El destino del mundo se decidirá en 1 minuto."
#define _LANG_SHUTDOWN_WRONG_PARAM "Argumento incorrecto.\n"
#define _LANG_SHUTDOWN_ALREADY "Ya estaba programado el reinicio del mud, dentro de "+ \
      (time_of_crash - time()) + " segundos.\n"
#define _LANG_SHUTDOWN_ACCEPT "Reinicio del mundo dentro de "+ \
    (time_of_crash - time()) + " segundos.\n"
#define _LANG_SHUTDOWN_MUD_CLOSING "¡Mud cerrándose!\n"
#define _LANG_SHUTDOWN_AUTO_REBOOT "El fin del mundo está a punto de sobrevenir...\nLa totalidad de la existencia " + \
        "será destruida y un nuevo mundo se reconstruirá sobre sus cenizas...\n\n\t" + \
        "%^BOLD%^Dentro de diez minutos%^RESET%^.\n"

// souls.c

#define _LANG_SOULS_HELP_HEADER "Éstos son los comandos de emociones disponibles:\n"+ \
      "   # significa que admite argumentos.\n"+ \
      "   * significa que sólo puede utilizarse con seres vivos.\n"+ \
      "   + significa que puede usarse tanto con seres vivos como de modo independiente.\n\n"+ \
      " Número total de emociones: "+m_sizeof(soul_data)+".\n\n"
#define _LANG_SOULS_CANNOT_DO "No puedes hacer eso. Escribe \"ayuda <emoción>\" "+ \
            "para ver las opciones disponibles.\n"
#define _LANG_SOULS_BLOCKING_YOU ((string)target->query_cap_name()) + " te está bloqueando.\n"
#define _LANG_SOULS_STATUS_NOT_CONNECTED "Lo siento, pero '"+liv+"' no está conectado.\n"
#define _LANG_SOULS_STATUS_NOT_THAT_WAY "No puedes utilizar ese comando de emoción de esa forma.\n"
#define _LANG_SOULS_STATUS_CANNOT_DO "No tienes el valor de hacer eso.\n"
#define _LANG_SOULS_STATUS_CANNOT_FIND "No puedo encontrar a '"+livfail+"'.\n"
#define _LANG_SOULS_HELP_NO_PARAMETERS "El comando de emoción '"+str+"' no tiene parámetros opcionales.\n"
#define _LANG_SOULS_HELP_PERSON "<persona>"
#define _LANG_SOULS_HELP_PARAMETER "<argumento>"
#define _LANG_SOULS_HELP_SYNTAX "Sintaxis del comando de emoción %^BOLD%^'"+str+"'%^RESET%^:\n"
#define _LANG_SOULS_HELP_NO_LIVINGS "Sin aplicarlo a seres vivos:\n"
#define _LANG_SOULS_HELP_LIVINGS "Aplicándolo a seres vivos:\n"
#define _LANG_SOULS_HELP_ANY_TEXT "Puede utilizarse cualquier texto como argumento.\n"
#define _LANG_SOULS_HELP_WITHOUT_PARAMETERS "Puede utilizarse sin argumentos.\n"
#define _LANG_SOULS_HELP_AVAILABLE_PARAMETERS "Los argumentos posibles son: "+implode(bit,", ")+".\n"
#define _LANG_SOULS_HELP_HAS_NO_PARAMETERS "No tiene argumentos.\n"
