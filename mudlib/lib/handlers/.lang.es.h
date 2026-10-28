
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
#define _LANG_WEAPONS_ELEMENTAL_MSG_ME "Dañas"
#define _LANG_WEAPONS_ELEMENTAL_MSG_HIM " te daña"
#define _LANG_WEAPONS_ELEMENTAL_MSG_ENV " daña"

// daño elemental extra que un arma añade a un golpe (element es el nombre del elemento)
// Elemental damage of a weapon, one line per hit. The weapon is named by its short:
// "Tu <short>" for its owner, "<article> <short> de <owner>" for the rest.
// One entry per element, in the order of RESISTANCES: the text for the others
// and the text for the defender. The tokens are the ones weapons can use in
// their own messages: $weapon$, $attacker$ and $defender$.
#define _LANG_WEAPONS_EXTRA_DAMAGE_WEAPON_ME "Tu "+weapon->query_short()
#define _LANG_WEAPONS_EXTRA_DAMAGE_WEAPON_OTHER capitalize(weapon->query_article())+" "+weapon->query_short()+" de "+attacker->query_cap_name()
#define _LANG_WEAPONS_EXTRA_DAMAGE_VERBS ({ \
  ({ "corroe a $defender$", "te corroe" }), \
  ({ "azota con un golpe de viento a $defender$", "te azota con un golpe de viento" }), \
  ({ "sacude con una descarga a $defender$", "te sacude con una descarga" }), \
  ({ "hiela a $defender$", "te hiela" }), \
  ({ "golpea con su magia a $defender$", "te golpea con su magia" }), \
  ({ "hiere la mente de $defender$", "te hiere la mente" }), \
  ({ "descarga una lluvia de proyectiles sobre $defender$", "descarga una lluvia de proyectiles sobre ti" }), \
  ({ "envenena a $defender$", "te envenena" }), \
  ({ "castiga con la fuerza de la ley a $defender$", "te castiga con la fuerza de la ley" }), \
  ({ "azota con una oleada de agua a $defender$", "te azota con una oleada de agua" }), \
  ({ "abrasa con luz sagrada a $defender$", "te abrasa con luz sagrada" }), \
  ({ "infecta a $defender$", "te infecta" }), \
  ({ "quema a $defender$", "te quema" }), \
  ({ "corrompe con su maldad a $defender$", "te corrompe con su maldad" }), \
  ({ "drena la vida de $defender$", "te drena la vida" }), \
  ({ "aplasta con su peso de roca a $defender$", "te aplasta con su peso de roca" }), \
  ({ "desgarra con el caos a $defender$", "te desgarra con el caos" }), })

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

// titles

#define _LANG_TITLE_DEMO_FINISHED "Recién Llegado"
#define _LANG_TITLE_GRANTED "%^BOLD%^Has ganado el título '" + text + "'.%^RESET%^\n"

// quests

#define _LANG_QUEST_ACCEPTED "Aceptas la misión: " + quest->query_title() + ".\n"
#define _LANG_QUEST_ABANDONED "Abandonas la misión: " + quest->query_title() + ".\n"
#define _LANG_QUEST_COMPLETED "%^BOLD%^Has terminado " + quest->query_title() + ".%^RESET%^\n"
#define _LANG_QUEST_OBJECTIVE_DONE "Misión %^BOLD%^" + quest->query_title() + \
      "%^RESET%^\n  " + objectives[j][OBJ_TEXT] + ": %^BOLD%^hecho%^RESET%^."
#define _LANG_QUEST_OBJECTIVE_PROGRESS "Misión %^BOLD%^" + quest->query_title() + \
      "%^RESET%^\n  " + objectives[j][OBJ_TEXT] + ": " + after + " de " + \
      objectives[j][OBJ_COUNT] + "."
#define _LANG_QUEST_OBJECTIVES_MET "\n  Ya puedes entregarla ('misiones entregar')."

// books. `page` and `pages` are the page shown and how many there are, `book`
// the title of the book and `reader` who is reading it

#define _LANG_BOOKS_FOOTER "[Página " + (page + 1) + " de " + pages + \
        ": Intro sigue, - vuelve, <número> va a una página, h ayuda, q cierra]"
#define _LANG_BOOKS_OPEN "Abres el libro.\n"
#define _LANG_BOOKS_OPEN_OTHERS reader->query_cap_name() + " abre un libro y empieza a leer.\n"
#define _LANG_BOOKS_CLOSE "Cierras el libro.\n"
#define _LANG_BOOKS_UNREADABLE "Las páginas de este libro están en blanco, o tan estropeadas que no se pueden leer.\n"
#define _LANG_BOOKS_NO_SUCH_PAGE "El libro no tiene esa página.\n"
#define _LANG_BOOKS_HELP_KEYS ({ "h", "ayuda", "?" })
#define _LANG_BOOKS_HELP "Mientras lees un libro:\n" + \
        "  Intro o +  : pasas a la página siguiente.\n" + \
        "  -          : vuelves a la anterior.\n" + \
        "  <número>   : vas a esa página.\n" + \
        "  h          : muestra esta ayuda.\n" + \
        "  q          : cierras el libro (cualquier otra cosa también).\n"
#define _LANG_BOOKS_FILED "%^BOLD%^Anotas este libro en la enciclopedia de tu diario " + \
        "('diario enciclopedia' para volver a leerlo).%^RESET%^\n"

// crimes

#define _LANG_CRIMES_SEEN_CRIMINAL "Alguien ha visto lo que has hecho, y la ley de este lugar lo recordará.\n"
#define _LANG_CRIMES_SEEN_VICTIM "La ley de este lugar sabrá que $criminal$ ha sido el culpable.\n"

// crimes: justice

#define _LANG_CRIMES_PASSED_OUT_PROP "Estás inconsciente tras la paliza de los guardias.\n"
#define _LANG_CRIMES_BEATEN "Los guardias te golpean hasta dejarte tendido en el suelo, sin sentido.\n"
#define _LANG_CRIMES_BEATEN_ROOM "$who$ cae al suelo, vencido por los guardias.\n"
#define _LANG_CRIMES_ARRESTED "Los guardias te arrastran hasta la prisión.\n"
#define _LANG_CRIMES_ARRESTED_ROOM "Los guardias se llevan a rastras a $who$.\n"
#define _LANG_CRIMES_FINED_ON_SPOT "Los guardias te registran la bolsa y se cobran la multa: $amount$ monedas.\n"
#define _LANG_CRIMES_LET_GO "Los guardias te dejan tirado donde has caído.\n"
#define _LANG_CRIMES_RELEASED "Has cumplido tu condena. Ya puedes marcharte.\n"
#define _LANG_CRIMES_BAILED "Alguien ha pagado tu fianza: eres libre.\n"
#define _LANG_CRIMES_PUT_BACK "Los guardias te devuelven a tu celda: aún no has cumplido tu condena.\n"

// crimes: capture of a beaten target

#define _LANG_CRIMES_CAPTURE_NO_LAW "Aquí no manda la ley que te encargó esta captura: nadie se hará cargo de $who$, que acabará por recobrar el sentido, y esta victoria no cuenta.\n"
#define _LANG_CRIMES_CAPTURE_NO_CELL "La ley de este lugar no tiene celda donde meter a $who$, que acabará por recobrar el sentido, y esta victoria no cuenta.\n"
#define _LANG_CRIMES_CAPTURE_NO_GUARD "No hay ningún guardia libre lo bastante cerca para llevarse a $who$. Recobrará el sentido y esta victoria no cuenta.\n"
#define _LANG_CRIMES_CAPTURE_COMING "Un guardia acude a hacerse cargo de $who$.\n"
#define _LANG_CRIMES_CAPTURE_HELD "Estás inconsciente, a merced de los guardias.\n"
#define _LANG_CRIMES_CAPTURE_FAILED "La detención de $who$ ha fracasado.\n"
#define _LANG_CRIMES_CAPTURE_JAILED "Los guardias han encerrado a $who$.\n"
#define _LANG_CRIMES_CAPTURE_JAILED_ROOM "El guardia arroja a $who$ a la celda y cierra tras de sí.\n"
#define _LANG_CRIMES_CAPTIVE_GONE "$who$ cumple su condena y se marcha de la celda para no volver.\n"

// rules (the normas of a guild or a job). `level` is the guild level a skill
// is taught from; `next`, `cost` and `have` the reader's next level, what it
// costs and the experience they have

#define _LANG_RULES_TITLE_GUILD "Normas del gremio: "
#define _LANG_RULES_TITLE_JOB "Normas del oficio: "
#define _LANG_RULES_WHO_HEADER "%^BOLD%^Quién puede entrar%^RESET%^\n"
#define _LANG_RULES_RACES "  Razas: "
#define _LANG_RULES_CLASSES "  Clases: "
#define _LANG_RULES_ANY "cualquiera"
#define _LANG_RULES_ALIGNMENT "  Alineamiento: "
#define _LANG_RULES_ALIGN_NEUTRAL "neutral"
#define _LANG_RULES_ALIGN_GOOD "bueno"
#define _LANG_RULES_ALIGN_EVIL "malvado"
#define _LANG_RULES_ALSO "  Además: "
#define _LANG_RULES_YOU_MEET "  Tú reúnes estas condiciones.\n"
#define _LANG_RULES_YOU_DO_NOT_MEET "  Tú no reúnes estas condiciones.\n"
#define _LANG_RULES_SKILLS_HEADER "%^BOLD%^Habilidades que enseña%^RESET%^\n"
#define _LANG_RULES_SKILL_FROM_LEVEL "desde el nivel " + level + " de gremio"
#define _LANG_RULES_SKILL_FROM_JOB_LEVEL "desde el nivel " + level + " de oficio"
#define _LANG_RULES_SPELLS_HEADER "%^BOLD%^Hechizos que enseña (se aprenden con «aprender»)%^RESET%^\n"
#define _LANG_RULES_RELATED_SKILLS_HEADER "%^BOLD%^Habilidades ligadas al gremio (se aprenden fuera de él)%^RESET%^\n"
#define _LANG_RULES_RELATED_SPELLS_HEADER "%^BOLD%^Hechizos ligados al gremio (se aprenden fuera de él)%^RESET%^\n"
#define _LANG_RULES_RELATED_JOB_SKILLS_HEADER "%^BOLD%^Habilidades ligadas al oficio (se aprenden fuera de él)%^RESET%^\n"
#define _LANG_RULES_RELATED_JOB_SPELLS_HEADER "%^BOLD%^Hechizos ligados al oficio (se aprenden fuera de él)%^RESET%^\n"
#define _LANG_RULES_LEVEL_HEADER "%^BOLD%^Tu siguiente nivel%^RESET%^\n"
#define _LANG_RULES_NOT_MEMBER_GUILD "  No eres miembro de este gremio.\n"
#define _LANG_RULES_NOT_MEMBER_JOB "  Este no es tu oficio.\n"
#define _LANG_RULES_MAX_LEVEL "  Ya has alcanzado el nivel más alto.\n"
#define _LANG_RULES_GUILD_COST "  Para avanzar al nivel " + next + " necesitas " + \
        cost + " puntos de experiencia (tienes " + have + ").\n"
#define _LANG_RULES_JOB_COST "  Llegarás al nivel " + next + " de oficio al pasar de " + \
        cost + " puntos de experiencia de oficio (llevas " + have + ").\n"
#define _LANG_RULES_NO_BOOK "No hay más normas escritas.\n"
