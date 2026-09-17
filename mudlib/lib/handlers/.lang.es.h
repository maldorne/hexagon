
#define _LANG_CALENDAR_BIRTHDAY_STRING day+" de "+month(month-1)

// Composition of a year name. References `num`, `gender` and `year`
// (a mixed * with POS_YEAR_NAME / POS_YEAR_ADJECTIVE indices) from
// the caller, which is calendar.c::query_year_name().
#define _LANG_CALENDAR_YEAR_STRING \
  "" + num + ", el año de" + ((gender == 1) ? "l " : " la ") + \
  capitalize(year[POS_YEAR_NAME]) + " " + year[POS_YEAR_ADJECTIVE]

// weather

#define _LANG_WEATHER_MONTHS ({ "enero", "febrero", "marzo", "abril", \
                                "mayo", "junio", "julio", "agosto", \
                                "septiembre", "octubre", "noviembre", \
                                "diciembre" })

#define _LANG_WEATHER_SEASONS ({ "invierno", "primavera", "verano", "otoño" })

#define _LANG_WEATHER_DAYNIGHT ({ "día", "noche" })

#define _LANG_WEATHER_MOON_PHASES ({ \
  "llena", "tres cuartos menguante", "media menguante", \
  "menguante", "menguante", "nueva", "creciente", "creciente", \
  "media creciente", "tres cuartos creciente" })

// Single moon name for the shared base handler. Per-game subclasses
// with multiple / coloured moons override moon_string() outright.
#define _LANG_WEATHER_MOON_NAME "luna"

// El objeto controlador del tiempo, tal como lo ve quien lo examina
#define _LANG_WEATHER_NO_CLONING "Este objeto no puede ser clonado.\n"
#define _LANG_WEATHER_OB_NAME    "tiempo"
#define _LANG_WEATHER_OB_SHORT   "Controlador extraordinario del tiempo"
#define _LANG_WEATHER_OB_LONG    "¡Éste es el controlador extraordinario del tiempo!\n"
#define _LANG_WEATHER_OB_ALIAS   "controlador"

// Full "moon <phase>" sentence. References `phase` from the caller.
// The name is capitalized here because it is the sentence start.
#define _LANG_WEATHER_MOON_STRING \
  "%^BOLD%^" + capitalize(_LANG_WEATHER_MOON_NAME) + "%^RESET%^ " + phase + ".\n"

#define _LANG_WEATHER_TEMP_SCORCHING  "un calor abrasador"
#define _LANG_WEATHER_TEMP_VERY_HOT   "mucho calor"
#define _LANG_WEATHER_TEMP_HOT        "un clima caluroso"
#define _LANG_WEATHER_TEMP_WARM       "un clima cálido"
#define _LANG_WEATHER_TEMP_PLEASANT   "buena temperatura"
#define _LANG_WEATHER_TEMP_COOL       "un clima frío"
#define _LANG_WEATHER_TEMP_COLD       "frío"
#define _LANG_WEATHER_TEMP_VERY_COLD  "mucho frío"
#define _LANG_WEATHER_TEMP_POLAR      "un frío polar"

#define _LANG_WEATHER_WIND_STRONG      "viento fuerte"
#define _LANG_WEATHER_WIND_VERY_STRONG "viento muy fuerte"
#define _LANG_WEATHER_WIND_BLIZZARD    "ventisca"
#define _LANG_WEATHER_WIND_HURRICANE   "viento huracanado"

// Rain buckets. The stormy ones use ({ rain, hail, snow }) indexed by
// the cold-mod computed in rain_string().
#define _LANG_WEATHER_RAIN_LIGHT   "lluvia suave"
#define _LANG_WEATHER_RAIN_NORMAL  "lluvia"
#define _LANG_WEATHER_RAIN_STORM         ({ "tormenta", "granizo", "nieve" })
#define _LANG_WEATHER_RAIN_STRONG_STORM  ({ "fuerte tormenta", "fuerte granizo", \
                                            "una fuerte nevada" })
#define _LANG_WEATHER_RAIN_HEAVY_STORM   ({ "tormenta muy fuerte", "granizo muy fuerte", \
                                            "una nevada muy densa" })

#define _LANG_WEATHER_STRING_OF   " de "
#define _LANG_WEATHER_STRING_WITH " con "
#define _LANG_WEATHER_STRING_SEP  ", "
#define _LANG_WEATHER_STRING_AND  " y "
#define _LANG_WEATHER_STRING_END  ".\n"

#define _LANG_WEATHER_DATE_STRING \
  ((data[0] == 1) ? "Es la una" : "Son las " + data[0]) + \
  " del " + capitalize(handler("calendar", this_object())->query_week_day_string(0, this_object())) + \
  " " + (day_of_month + 1) + " de " + \
  capitalize(month_string()) + \
  " del año " + handler("calendar")->query_year_name(data[4])

// postal

#define _LANG_POSTAL_NEW_MAIL "Te ha llegado una carta de " + capitalize(from) + \
  ": " + subject + ".\n"

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

// channels
//
// A channel is stored and asked for by its id, an English word. The words a
// player types are here, and so is the name the channel is shown under.
// `name` in these messages is the display name of the channel.

#define _LANG_CHANNEL_VERBS ([ \
  "emergencia" : "emergency", \
  "gremio"     : "guild", \
  "clan"       : "clan", \
  "raza"       : "race", \
  "aventurero" : "adventurer", \
])

#define _LANG_CHANNEL_NAMES ([ \
  "emergency" : "emergencia", \
])

#define _LANG_CHANNEL_ANY "<canal>"

#define _LANG_CHANNEL_OPENED "Ok, abres el canal %^BOLD%^WHITE%^" + name + "%^RESET%^.\n"
#define _LANG_CHANNEL_CLOSED "Ok, apagas el canal %^RED%^" + name + "%^RESET%^.\n"
#define _LANG_CHANNEL_ALREADY_ON "¡El canal '" + name + "' ya está activado!\n"
#define _LANG_CHANNEL_ALREADY_OFF "¡El canal '" + name + "' ya está desactivado!\n"
#define _LANG_CHANNEL_NOT_LISTENING "No tienes ese canal activado.\n"
#define _LANG_CHANNEL_DISABLED "Tu canal ha sido deshabilitado.\n"
#define _LANG_CHANNEL_NO_PERMISSION "No tienes permiso para usar el canal '" + name + "'.\n"
#define _LANG_CHANNEL_NEEDS_MESSAGE "Funciona mejor si introduces un mensaje.\n"

#define _LANG_CHANNEL_NO_GUILD "No eres miembro de ningún gremio.\n"
#define _LANG_CHANNEL_GUILD_NO_CHANNEL "Tu gremio no tiene esa habilidad.\n"
#define _LANG_CHANNEL_NO_CLAN "No eres miembro de ningún clan.\n"
#define _LANG_CHANNEL_CLAN_NO_CHANNEL "Tu clan no tiene esa habilidad.\n"
#define _LANG_CHANNEL_NO_RACE_GROUP "No eres miembro de ningún grupo racial.\n"
#define _LANG_CHANNEL_RACE_NO_CHANNEL "Tu grupo racial no tiene esa habilidad.\n"
#define _LANG_CHANNEL_NO_PARTY "Debes pertenecer a un grupo para utilizar este canal.\n"
#define _LANG_CHANNEL_PARTY_AUTOMATIC "No puedes abrir ni cerrar el canal de tu grupo, funciona automáticamente.\n"

#define _LANG_CHANNEL_HISTORY_HEADER "Historia del canal '" + name + "':\n"
#define _LANG_CHANNEL_HISTORY_EMPTY "El canal está vacío.\n"
#define _LANG_CHANNEL_HISTORY_NOT_ALLOWED "Intento de ver el histórico de canales a los que no tienes permiso.\n"
#define _LANG_CHANNEL_HELP_HEADER "Ayuda del canal '" + name + "':\n"
#define _LANG_CHANNEL_WHO_HEADER "Gente escuchando en el canal '" + name + "':"

#define _LANG_CHANNEL_HELP_MESSAGE "\t" + name + " <mensaje> : Envía un mensaje a la gente que escucha el canal.\n"
#define _LANG_CHANNEL_HELP_HELP "\t" + name + " ?     : Muestra este mensaje de ayuda.\n"
#define _LANG_CHANNEL_HELP_HISTORY "\t" + name + " !     : Ver los últimos " + number_as_string(MAX_CHANNEL_HISTORY) + " mensajes enviados al canal.\n"
#define _LANG_CHANNEL_HELP_WHO "\t" + name + " .     : Lista quienes están escuchando el canal.\n"
