
// common

#define _LANG_CMD_NOTHING_HERE "No hay nada con el nombre '"+arg+"' aquí.\n"
#define _LANG_CMD_DEAD "Estás en forma espiritual, no necesitas conocer eso.\n"

// calendar.c

#define _LANG_CMD_CALENDAR_ALIAS       ({ "calendario", "calendar" })
#define _LANG_CMD_CALENDAR_SYNTAX      "calendario [años]"
#define _LANG_CMD_CALENDAR_HELP        "Muestra el calendario del mes actual. Con el argumento " + \
                                       "'años' lista los nombres de los años transcurridos."
#define _LANG_CMD_CALENDAR_ARG_YEARS   ({ "años", "anyos", "years" })
#define _LANG_CMD_CALENDAR_ERA_HEADER  "Calendario de la presente era:\n\n"
#define _LANG_CMD_CALENDAR_TITLE       "Calendario"
#define _LANG_CMD_CALENDAR_USAGE       "Sintaxis: calendario [años]\n"
#define _LANG_CMD_CALENDAR_WEEK_HEADER "  L   M   X   J   V   S   D\n"

// kill.c

#define _LANG_CMD_KILL_ALIAS ({ "matar", "atacar" })
#define _LANG_CMD_KILL_SYNTAX "atacar <objetivos>"
#define _LANG_CMD_KILL_HELP "Comienza a atacar a los objetivos especificados."
#define _LANG_CMD_KILL_NOT_ALLOWED "No está permitido.\n"
#define _LANG_CMD_KILL_NOT_ALLOWED_ARRAY ({ "todo", "todos", "all", "everyone", "*" })
#define _LANG_CMD_KILL_NO_NEED_WHEN_DEAD "Tu estado etéreo tiene poco efecto en el mundo material.\n"
#define _LANG_CMD_KILL_NOT_FOUND "No consigues encontrar a "+me->expand_nickname(str)+".\n"
#define _LANG_CMD_KILL_NOT_SUICIDE "¿Te quieres suicidar?\n"
#define _LANG_CMD_KILL_NOT_ALIVE "Intentas golpear a " + str + ", pero " + str + " no " + \
                                 "parece darse cuenta.\n"
#define _LANG_CMD_KILL_START "Ok. Comienzas a pelear con " + query_multiple_short(res, 0) + ".\n"
#define _LANG_CMD_KILL_NO_FIGHT "No puedes atacar a nadie en este lugar.\n"

// look.c

#define _LANG_CMD_LOOK_ALIAS ({ "mirar" })
#define _LANG_CMD_LOOK_SYNTAX "mirar [a] [<objeto>]"
#define _LANG_CMD_LOOK_HELP "Devuelve la descripción de un objeto o (por defecto) " + \
                    "la localización en la que te encuentras."
#define _LANG_CMD_LOOK_NO_ENVIRONMENT "No estás en ningún sitio... no puedes ver nada.\n"
#define _LANG_CMD_LOOK_BLINDED "No puedes ver nada. ¡Estás cegado!\n"

// speak (set the language you speak / write). `name` is the language's
// real-world display name; `str` the raw input the player typed.
#define _LANG_CMD_SPEAK_ALIAS ({ "hablar" })
#define _LANG_CMD_SPEAK_SYNTAX "hablar [<idioma>]"
#define _LANG_CMD_SPEAK_HELP "Elige el idioma en el que hablas y escribes. Sin " + \
                    "argumentos muestra tu idioma actual y los que conoces."
#define _LANG_CMD_SPEAK_SELECT "Debes seleccionar un idioma para hablar.\n"
#define _LANG_CMD_SPEAK_CURRENT "Ahora hablas en " + name + ".\n"
#define _LANG_CMD_SPEAK_KNOWN_HEADER "Puedes hablar en los siguientes idiomas:\n"
#define _LANG_CMD_SPEAK_NONE "No conoces ningún idioma.\n"
#define _LANG_CMD_SPEAK_UNKNOWN "No conoces el idioma '" + str + "'.\n"
#define _LANG_CMD_SPEAK_NOW_USING "Usando el idioma " + name + " para hablar y escribir.\n"

// glance

#define _LANG_CMD_GLANCE_ALIAS ({ "ojear" })
#define _LANG_CMD_GLANCE_SYNTAX "ojear [<objeto>]"
#define _LANG_CMD_GLANCE_HELP "Devuelve una descripción corta de un objeto o del lugar donde estás."
#define _LANG_CMD_LOOK_TOO_DARK "Está demasiado oscuro como para ver algo.\n"
#define _LANG_CMD_LOOK_TOO_BRIGHT "Estás deslumbrado por la luz.\n"
#define _LANG_CMD_GLANCE_YOURSELF ", ¡Eres tú mismo!\n"

// prompt

#define _LANG_CMD_PROMPT_ALIAS ({ "prompt" })
#define _LANG_PROMPT_CLEAN "borrar"
#define _LANG_PROMPT_CURRENT "Tu prompt actual es: '" + user->query_prompt_string() + "'\n"
#define _LANG_PROMPT_SYNTAX "prompt <texto>"
#define _LANG_PROMPT_HELP "Cambia o elimina el prompt usado actualmente.\n" + \
           "Puedes cambiarlo usando 'prompt <texto>', o quitarlo con 'prompt " + \
           _LANG_PROMPT_CLEAN + "'.\n" + \
           "En <texto>, se aceptan los siguientes atajos:\n" + \
           "     $n -> tu nombre\n" + \
           "     $h -> tus puntos de vida actuales\n" + \
           "     $g -> tus puntos de energía actuales\n" + \
           "     $B -> comenzar negrita\n" + \
           "     $m -> el nombre del mud\n" + \
           ((user && user->query_coder()) ? "     $~ -> path actual\n" : "") + \
           "\nPor ejemplo: 'prompt $n ($h)' mostrará en cada línea:\n" + \
           "     " + (me ? me->query_name() : "$n") + \
           " (" + (me ? "" + me->query_hp() : "$h") + ") >\n"
#define _LANG_PROMPT_OK "Ok, prompt cambiado.\n"

// whoami

#define _LANG_CMD_WHOAMI_ALIAS ({ "whoami", "yo", "quiensoy" })
#define _LANG_CMD_WHOAMI_SYNTAX "whoami"
#define _LANG_CMD_WHOAMI_HELP "Muestra información sobre tu propio personaje."
#define _LANG_WHOAMI_CLASS_FIRST "   Clase:\n"
#define _LANG_WHOAMI_CLASS_EACH "     " + (me->query_class_ob())->query_short() + \
      " de nivel " + me->query_level() + ".\n"
#define _LANG_WHOAMI_GUILD_FIRST "   Gremios:\n"
#define _LANG_WHOAMI_GUILD_EACH "     " + capitalize(g_names[i]->query_short()) + \
      " de nivel " + guilds[g_names[i]][0] + ".\n";
#define _LANG_WHOAMI_JOB_FIRST "   Oficios:\n"
#define _LANG_WHOAMI_JOB_EACH "     " + capitalize(j_names[i]->query_short()) + \
      " de nivel " + jobs[j_names[i]][0] + ".\n";

// commands

#define _LANG_CMD_COMMANDS_ALIAS ({ "comandos" })
#define _LANG_CMD_COMMANDS_SYNTAX "comandos [-l]"
#define _LANG_CMD_COMMANDS_HELP "Muestra todos los comandos que tienes disponibles.\n" + \
                                "comandos -l para una lista con más información."
#define _LANG_CMD_COMMANDS_CATEGORY_MSG "\n%^GREEN%^Comandos de " + categories[i] + ":%^RESET%^\n"

// retire

#define _LANG_CMD_RETIRE_ALIAS ({ "retirar" })
#define _LANG_CMD_RETIRE_SYNTAX "retirar"
#define _LANG_CMD_RETIRE_HELP "Borra todos los datos y elimina tu personaje."
#define _LANG_CMD_RETIRE_GUEST "Eres un invitado en " + mud_name() + ".\n" + \
        "¿Aún no tienes un personaje y ya estás pensando en retirarte?\n"

// score

#define _LANG_CMD_SCORE_ALIAS ({ "puntos", "score" })
#define _LANG_CMD_SCORE_SYNTAX "puntos [-d]"
#define _LANG_CMD_SCORE_HELP "Muestra algunos datos sencillos sobre tu personaje.\n" + \
        "Usa -d para una información algo más detallada."
#define _LANG_CMD_SCORE_AGE "Tienes " + seconds_to_time_string(age) + " de edad.\n";

// configuration

#define _LANG_CMD_CONFIG_ALIAS ({ "configuracion", "configuración", "config", "settings" })
#define _LANG_CMD_CONFIG_SYNTAX "configuración [<categoría> [<tema> [<valor>]]]"
#define _LANG_CMD_CONFIG_HELP "Muestra y cambia de un solo sitio lo que puedes ajustar de tu cuenta y de tu personaje."
#define _LANG_CMD_CONFIG_NOT_VALID_CATEGORY "'" + words[0] + "' no es una categoría de la configuración.\n"
#define _LANG_CMD_CONFIG_NOT_VALID_TOPIC "'" + words[1] + "' no es algo que se configure en " + title + ".\n"
#define _LANG_CMD_CONFIG_NOT_VALID_VALUE "'" + value + "' no vale para " + title + " / " + topic + ". "
#define _LANG_CMD_CONFIG_CANNOT_SET "Eso no se puede cambiar ahora mismo.\n"
#define _LANG_CMD_CONFIG_REFUSED title + " / " + topic + ": no se puede cambiar ahora mismo.\n"
#define _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY "  %^BOLD%^" + title + "%^RESET%^ " + owner + ":\n"
#define _LANG_CMD_CONFIG_OWNER_USER "(de tu cuenta, igual en todos tus personajes)"
#define _LANG_CMD_CONFIG_OWNER_PLAYER "(de este personaje)"
#define _LANG_CMD_CONFIG_FOR_USER "Configuración de " + me->query_cap_name()
#define _LANG_CMD_CONFIG_CURRENT title + " / " + topic + ": [ " + shown + " ]\n"
#define _LANG_CMD_CONFIG_ADJUSTED title + " / " + topic + ": [ " + shown + " ] (se ha ajustado lo que pediste)\n"
#define _LANG_CMD_CONFIG_ACCEPTS_BOOL "Acepta sí o no.\n"
#define _LANG_CMD_CONFIG_ACCEPTS_INT "Acepta un número entre " + setting[CONFIG_MIN] + " y " + setting[CONFIG_MAX] + ".\n"
#define _LANG_CMD_CONFIG_ACCEPTS_CHOICE "Acepta: " + implode(setting[CONFIG_OPTIONS], ", ") + ".\n"
#define _LANG_CMD_CONFIG_ACCEPTS_STRING "Acepta cualquier texto, o 'nada' para dejarlo vacío.\n"
#define _LANG_CMD_CONFIG_USE_COMMANDS "  Entre corchetes, el comando que hace lo mismo y tiene su propia ayuda.\n"
#define _LANG_CMD_CONFIG_HINT "  'configuración <categoría> <tema>' dice qué valores acepta cada cosa,\n" + \
  "  y 'configuración <categoría> <tema> <valor>' la cambia.\n"

// sheet

#define _LANG_CMD_SHEET_ALIAS ({ "ficha" })
#define _LANG_CMD_SHEET_SYNTAX "ficha"
#define _LANG_CMD_SHEET_HELP "Muestra datos extendidos sobre tu personaje."

#define _LANG_CMD_SHEET_NAME "Nombre"
#define _LANG_CMD_SHEET_CANNOT_FIND "Lo siento, no encuentro a '"+name+"'.\n"
#define _LANG_CMD_SHEET_BASIC_CHARACTERISTICS "Características básicas"
#define _LANG_CMD_SHEET_CAN_ALSO_USE "También puedes usar: 'habilidades', " + \
        "'maestrías' y 'estilos'.\n"
#define _LANG_STATS_INTOX "Intoxicación"

// verbose

#define _LANG_CMD_VERBOSE_ALIAS ({ "detalle", "verbose" })
#define _LANG_CMD_VERBOSE_SYNTAX "detalle <sí|no>"
#define _LANG_CMD_VERBOSE_HELP "Configura descripciones más largas por defecto."
#define _LANG_CMD_VERBOSE_ON "Ok, estás en modo detallado.\n"
#define _LANG_CMD_VERBOSE_OFF "Ok, estás en modo abreviado.\n"
#define _LANG_CMD_VERBOSE_CURRENT_ON "Estás en modo detallado.\n"
#define _LANG_CMD_VERBOSE_CURRENT_OFF "Estás en modo abreviado.\n"

// inform

#define _LANG_CMD_INFORM_ALIAS ({ "informar", "información", "informacion", "inform" })
#define _LANG_CMD_INFORM_SYNTAX "informar [evento <sí|no>]"
#define _LANG_CMD_INFORM_HELP "Muestra los ajustes de información automática sobre eventos."
#define _LANG_INFORM_HEADER "Estado de los eventos de información:\n"
#define _LANG_INFORM_BLOCKED "Todos los eventos están siendo reprimidos.\n" + \
      "Utiliza 'informar sí' para desbloquearlos.\n"
#define _LANG_INFORM_EVENTS_UNBLOCKED "Bloqueo de los eventos eliminado.\n"
#define _LANG_INFORM_EVENTS_BLOCKED "Bloqueo de todos los eventos añadido.\n"
#define _LANG_INFORM_ALL_ON "Todos los eventos activados.\n"
#define _LANG_INFORM_ALL_OFF "Todos los eventos desactivados.\n"
#define _LANG_INFORM_UNKNOWN_EVENT "Tipo de evento desconocido.\n"
#define _LANG_INFORM_ON "Eventos de tipo '" + frog[0] + "' activados.\n"
#define _LANG_INFORM_OFF "Eventos de tipo '" + frog[0] + "' desactivados.\n"

// bury

#define _LANG_CMD_BURY_ALIAS ({ "enterrar" })
#define _LANG_CMD_BURY_SYNTAX "enterrar"
#define _LANG_CMD_BURY_HELP "Elimina todos los cuerpos en tu localización. Nada de los cuerpos enterrados podrá ser recuperado."
#define _LANG_CMD_BURY_DEAD "Tú, un espíritu, ¿quieres enterrar cadáveres?\n"
#define _LANG_CMD_BURY_WATER "¿Quieres enterrar algo estando en el agua?\n"
#define _LANG_CMD_BURY_MSG "Entierras " + (counter == 1 ? "el cuerpo" : "los cuerpos") + " bajo tierra.\n"
#define _LANG_CMD_BURY_NO "¡No has encontrado ningún cuerpo!\n"

// cost

#define _LANG_CMD_COST_ALIAS ({ "coste" })
#define _LANG_CMD_COST_SYNTAX "coste"
#define _LANG_CMD_COST_HELP "Muestra tu experiencia actual y cuánto te falta para subir de nivel."
#define _LANG_CMD_COST_INTRO "Avanzarás tu nivel de:\n"
#define _LANG_CMD_COST_CLASS "   - Clase  con: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ puntos (llevas "+me->query_xp()+").\n"
#define _LANG_CMD_COST_GUILD "   - Gremio con: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ puntos (llevas "+me->query_xp()+").\n"
#define _LANG_CMD_COST_JOB "   - Oficio con: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ puntos de oficio (llevas "+me->query_job_xp()+").\n"

// stop

#define _LANG_CMD_STOP_ALIAS ({ "detener", "stop" })
#define _LANG_CMD_STOP_SYNTAX "detener <combates | persecuciones>"
#define _LANG_CMD_STOP_HELP "Detiene tus combates pendientes o persecuciones en marcha."
#define _LANG_CMD_STOP_NOT_FIGHTING "No estás luchando con nadie.\n"
#define _LANG_CMD_STOP_MSG "Has dejado de luchar con " + query_multiple_short(obs)+".\n"
#define _LANG_CMD_STOP_FOLLOW_OPTIONS ({ "persecuciones", "persecución", "persecucion", "seguir" })
#define _LANG_CMD_STOP_NOT_FOLLOWING "No estás siguiendo a nadie en estos momentos.\n"
#define _LANG_CMD_STOP_FIGHT_OPTIONS ({ "combates", "peleas" })
#define _LANG_CMD_STOP_FIGHTS_OK "Ok. Deteniendo combates, tardará un poco...\n"
#define _LANG_CMD_STOP_ALREADY_STOPPING "Ya estás intentando detener los combates, espera un poco.\n"

// encumbrance

#define _LANG_CMD_ENCUMBRANCE_ALIAS ({ "carga" })
#define _LANG_CMD_ENCUMBRANCE_SYNTAX "carga"
#define _LANG_CMD_ENCUMBRANCE_HELP "Muestra tu carga actual y máxima."
#define _LANG_CMD_ENCUMBRANCE_MSG "Tu inventario actual pesa "+now+" y puedes cargar como mucho "+max+", gracias a tu fuerza.\n"

// wimpy

#define _LANG_CMD_WIMPY_ALIAS ({ "cobardía", "cobardia" })
#define _LANG_CMD_WIMPY_SYNTAX "cobardía <num>"
#define _LANG_CMD_WIMPY_HELP "Establece el porcentaje de vida al que tienes que llegar para que tu personaje intente huir."
#define _LANG_CMD_WIMPY_NO_NUMBER "Debes introducir un numero.\n"
#define _LANG_CMD_WIMPY_NO_VALID "Debes introducir un numero entre 0 y 100. Actualmente seleccionado a ( "+me->query_wimpy()+" % )."+"\n"
#define _LANG_CMD_WIMPY_BRAVE "Ok. Estás en modo valiente.\n"
#define _LANG_CMD_WIMPY_MSG "Ok. Tu valentía te hará no huir hasta que no bajes de un "+wimpy+"% de tus puntos de vida.\n"

// money

#define _LANG_CMD_MONEY_ALIAS ({ "dinero" })
#define _LANG_CMD_MONEY_SYNTAX "dinero"
#define _LANG_CMD_MONEY_HELP "Muestra la cantidad de dinero que llevas encima y que tienes en los bancos."
#define _LANG_CMD_MONEY_ON_YOU "Llevas encima monedas por valor de:\n   " + MONEY_HAND->money_value_string(value) + "\n\n"
#define _LANG_CMD_MONEY_ON_BANK "En ["+bank->query_short()+"] tienes almacenado:\n   "
#define _LANG_CMD_MONEY_ON_BANK_EMPTY "En ["+bank->query_short()+"] tienes una cuenta abierta sin fondos.\n"
#define _LANG_CMD_MONEY_TOTAL "\nEn total, tienes acumulado:\n   "+MONEY_HAND->money_value_string(total) + ".\n"
#define _LANG_CMD_MONEY_ON_YOU_EMPTY "No llevas dinero encima.\n"

// condition

#define _LANG_CMD_CONDITION_ALIAS ({ "estado" })
#define _LANG_CMD_CONDITION_SYNTAX "estado [<nombre> | todos]"
#define _LANG_CMD_CONDITION_HELP "Muestra el estado de salud de un personaje o de todos los personajes en la localización."
#define _LANG_CMD_CONDITION_OPTIONS "Sintaxis: 'estado' para comprobar tu estado de salud, o\n" + \
                                    "          'estado <nombre>' para ver el estado de otro, o\n" + \
                                    "          'estado todos' para comprobar a todos los de tu entorno.\n"
#define _LANG_CMD_CONDITION_IS_DEAD "Está muert"+obs[i]->query_vowel()+"."
#define _LANG_CMD_CONDITION_HEADER "Estado en que se encuentran:\n"

// combats

#define _LANG_CMD_COMBATS_ALIAS ({ "combates" })
#define _LANG_CMD_COMBATS_SYNTAX "combates"
#define _LANG_CMD_COMBATS_HELP "Muestra a los personajes con los que estás luchando."
#define _LANG_CMD_COMBATS_NOBODY "No estás luchando con nadie.\n"
#define _LANG_CMD_COMBATS_MSG "Estás luchando con " + query_multiple_short(obs)+".\n"

// past

#define _LANG_CMD_PAST_ALIAS ({ "pasado", "past" })
#define _LANG_CMD_PAST_SYNTAX "pasado"
#define _LANG_CMD_PAST_HELP "Muestra el historial de lo que otros personajes te han dicho."

// inventory

#define _LANG_CMD_INVENTORY_ALIAS ({ "inventario" })
#define _LANG_CMD_INVENTORY_SYNTAX "inventario"
#define _LANG_CMD_INVENTORY_HELP "Muestra el inventario de tu personaje."

// away

#define _LANG_CMD_AWAY_ALIAS ({ "ausente", "ausencia", "away" })
#define _LANG_CMD_AWAY_SYNTAX "ausencia [mensaje]"
#define _LANG_CMD_AWAY_HELP "Establece un mensaje de ausencia para que los demás jugadores lo vean cuando te hablen."
#define _LANG_CMD_AWAY_NONE "No has indicado ningún mensaje de ausencia.\n"
#define _LANG_CMD_AWAY_REMOVED "Mensaje de ausencia borrado.\n"
#define _LANG_CMD_AWAY_OK "Mensaje de ausencia establecido a: %^BOLD%^RED%^'" + str + "'%^RESET%^.\n"

// consider

#define _LANG_CMD_CONSIDER_ALIAS ({ "considerar" })
#define _LANG_CMD_CONSIDER_SYNTAX "considerar <nombre>"
#define _LANG_CMD_CONSIDER_HELP "Muestra la diferencia de nivel entre tu personaje y el de otro."
#define _LANG_CMD_CONSIDER_EXIST "Necesitas considerar algo que exista.\n"
#define _LANG_CMD_CONSIDER_CODER "Atacar a un programador no es recomendable.\n"
#define _LANG_CMD_CONSIDER_15 "¡Ni se te ocurra atacar a "+obs[i]->query_cap_name()+", podría acabar contigo sólo con mirarte!\n"
#define _LANG_CMD_CONSIDER_10 "¡Estás loco si quieres atacar a "+obs[i]->query_cap_name()+"!\n"
#define _LANG_CMD_CONSIDER_M10 "¡Podrías matar a "+obs[i]->query_cap_name()+" con un soplido!\n"
#define _LANG_CMD_CONSIDER_NOT_THAT "No puedes considerar eso.\n"
#define _LANG_CMD_CONSIDER_14 ({ + \
          " es demasiado débil para molestarse en atacar.\n", + \
          " es mucho más débil que tú.\n", + \
          " es bastante más débil que tú.\n", + \
          " es más débil que tú.\n", + \
          " es un poco más débil que tú.\n", + \
          " es aproximadamente igual que tú.\n", + \
          " es un poco más fuerte que tú.\n", + \
          " es más fuerte que tú.\n", + \
          " es bastante más fuerte que tú.\n", + \
          " es mucho más fuerte que tú.\n", + \
          " es "+(obs[i]->query_article())+" más fuerte de la región.\n" })
#define _LANG_CMD_CONSIDER_NOT_14 ({ + \
          " es demasiado débil para molestarse en atacar.\n", + \
          " es más débil que tú.\n", + \
          " es aproximadamente igual que tú.\n", + \
          " es más fuerte que tú.\n", + \
          " es "+(obs[i]->query_article())+" más fuerte de la región.\n" })

// travel

#define _LANG_CMD_TRAVEL_ALIAS ({ "viajar" })
#define _LANG_CMD_TRAVEL_SYNTAX "viajar [<dirección>]"
#define _LANG_CMD_TRAVEL_HELP "Te permite viajar en una dirección determinada.\nNecesita que estés sobre una montura o vehículo."
#define _LANG_CMD_TRAVEL_NO_MOUNT "Sólo funciona si estás sobre una montura o vehículo.\n"
#define _LANG_CMD_TRAVEL_WHERE "¿"+capitalize(verb)+" hacia dónde?\n"

// equipment

#define _LANG_CMD_EQUIPMENT_ALIAS ({ "equipo" })
#define _LANG_CMD_EQUIPMENT_SYNTAX "equipo [todo]"
#define _LANG_CMD_EQUIPMENT_HELP "Muestra el estado de tu equipo."
#define _LANG_CMD_EQUIPMENT_HEADER "Estado de tu equipo:\n"
#define _LANG_CMD_EQUIPMENT_WEAP "  Armas:\n"
#define _LANG_CMD_EQUIPMENT_SHIE "  Escudos:\n"
#define _LANG_CMD_EQUIPMENT_ARMO "  Armaduras:\n"
#define _LANG_CMD_EQUIPMENT_NOTHING "  No llevas nada puesto ni estás sosteniendo nada.\n"
#define _LANG_CMD_EQUIPMENT_FOOTER "Utiliza 'equipo todo' para ver el estado de todo lo que estás cargando.\n"

// pov

#define _LANG_CMD_POV_ALIAS ({ "pov" })
#define _LANG_CMD_POV_SYNTAX "pov < "+implode( POV_TYPES, " | ")+" >"
#define _LANG_CMD_POV_HELP "Cambia la forma en que se perciben el resto de personajes del entorno."
#define _LANG_CMD_POV_CURRENT "Tu apuntador actual es: "
#define _LANG_CMD_POV_MSG "Ok, tipo de apuntador cambiado a '" + str + "'.\n"

// time

#define _LANG_CMD_TIME_ALIAS ({ "time", "hora", "fecha" })
#define _LANG_CMD_TIME_SYNTAX "time"
#define _LANG_CMD_TIME_REAL  "Mundo real:  "
#define _LANG_CMD_TIME_GAME  "En el juego: "
#define _LANG_CMD_TIME_HELP  "Muestra la hora y la fecha del mundo real y las del juego en el que estás."

// grope 

#define _LANG_CMD_GROPE_ALIAS ({ "palpar" })
#define _LANG_CMD_GROPE_SYNTAX "palpar"
#define _LANG_CMD_GROPE_HELP "Para buscar alguna salida a tu alrededor cuando no puedas ver."
#define _LANG_CMD_GROPE_NO_DARK "Palpar los alrededores únicamente te servirá cuando estés ciego o esté muy oscuro.\n"
#define _LANG_CMD_GROPE_LOCKOUT "Debes esperar un rato antes de volver a palpar de nuevo.\n"
#define _LANG_CMD_GROPE_NOTHING "Buscas a tu alrededor pero no encuentras nada.\n"
#define _LANG_CMD_GROPE_PLAYER_ME "¡Te encuentras a " + contents[j]->query_cap_name()+"!\n"
#define _LANG_CMD_GROPE_PLAYER_THEM "¡"+me->query_cap_name()+" choca contigo.\n"
#define _LANG_CMD_GROPE_PLAYER_ROOM "¡"+me->query_cap_name()+" choca con "+contents[j]->query_cap_name()+".\n"
#define _LANG_CMD_GROPE_NPC_ME "¡Parece que te has chocado con "+contents[j]->query_cap_name()+"!\n"
#define _LANG_CMD_GROPE_NPC_ROOM "¡"+me->query_cap_name()+" se choca con "+contents[j]->query_cap_name()+"!\n"
#define _LANG_CMD_GROPE_DOOR_ME "Palpas a tu alrededor y encuentras una puerta hacia "+contents[j]->query_dest()+".\n"
#define _LANG_CMD_GROPE_DOOR_ROOM me->query_cap_name()+" se golpea contra la puerta hacia "+contents[j]->query_dest()+".\n"
#define _LANG_CMD_GROPE_OTHER_ME "Te golpeas contra "+contents[j]->query_cap_name()+".\n"
#define _LANG_CMD_GROPE_OTHER_ROOM me->query_cap_name()+" se golpea contra "+contents[j]->query_cap_name()+".\n"
#define _LANG_CMD_GROPE_MSG "Palpas a tu alrededor y encuentras una salida hacia " + dir + ".\n"

// map

#define _LANG_CMD_MAP_ALIAS ({ "mapa" })
#define _LANG_CMD_MAP_SYNTAX "mapa [compacto | unicode | color | coordenadas]"
#define _LANG_CMD_MAP_HELP "Muestra un mapa de tu entorno."
#define _LANG_CMD_MAP_NO_ENV "¡Sin entorno no puedes hacer eso!\n"
#define _LANG_CMD_MAP_INVALID "Por alguna razón el mapa no funciona en un lugar como este.\n"
#define _LANG_CMD_MAP_NO_WATER "No puedes ponerte a consultar el mapa rodeado de agua.\n"
#define _LANG_CMD_MAP_INVISIBLE "No puedes consultar el mapa estando completamente invisible.\n"
#define _LANG_CMD_MAP_JUST_ARRIVED "Acabas de llegar y todavía no reconoces el terreno, espera un momento.\n"
#define _LANG_CMD_MAP_LEGEND "Leyenda"
#define _LANG_CMD_MAP_YOUR_POS "Tu posición"
#define _LANG_CMD_MAP_ENEMIES "Enemigos"
#define _LANG_CMD_MAP_FRIENDS "Compañeros de grupos"
#define _LANG_CMD_MAP_GUARDS "Guardias"
#define _LANG_CMD_MAP_DOORS "Puertas"
#define _LANG_CMD_MAP_HOMES "Viviendas"
#define _LANG_CMD_MAP_UP_STAIRS "Escaleras de subida"
#define _LANG_CMD_MAP_DOWN_STAIRS "Escaleras de bajada"
#define _LANG_CMD_MAP_NEW_QUESTS "Nuevas misiones"
#define _LANG_CMD_MAP_FINISHED_QUESTS "Misiones terminadas"
#define _LANG_CMD_MAP_COAST "Orilla"

#define _LANG_CMD_MAP_OPT_COMPACT ({ "compacto", "compact" })
#define _LANG_CMD_MAP_OPT_UNICODE ({ "unicode" })
#define _LANG_CMD_MAP_OPT_COLOR   ({ "color", "colour" })
#define _LANG_CMD_MAP_OPT_COORDS  ({ "coordenadas", "coords" })

#define _LANG_CMD_MAP_VARIANTS_HELP \
        "\nVariantes:\n" + \
        "  mapa              el mapa estándar.\n" + \
        "  mapa compacto     una vista densa de un carácter por lugar.\n" + \
        "  mapa unicode      la misma densidad que compacto pero con\n" + \
        "                    glifos reales de dibujo de cajas (necesita\n" + \
        "                    un cliente UTF-8).\n" + \
        "  mapa color        cajas estándar, con cada habitación coloreada\n" + \
        "                    según el área a la que pertenece.\n"
#define _LANG_CMD_MAP_VARIANTS_HELP_CODER \
        "  mapa coordenadas  superposición de coordenadas (sólo coders).\n"

#define _LANG_CMD_MAP_UNKNOWN_VARIANT "Variante de mapa desconocida.\n"

// worldmap.c

#define _LANG_CMD_WORLDMAP_ALIAS   ({ "mapamundi", "worldmap" })
#define _LANG_CMD_WORLDMAP_SYNTAX  "mapamundi"
#define _LANG_CMD_WORLDMAP_HELP    "Muestra un mapa a vista de pájaro centrado en el sector en el que estás. " + \
                                   "Cada glifo representa un sector de 10x10; tu posición está marcada con '@'."
#define _LANG_CMD_WORLDMAP_HELP_CODER "Programadores: puedes indicar el tamaño en sectores, mapamundi <ancho> [alto] (3-80 de ancho, 3-40 de alto)."
#define _LANG_CMD_WORLDMAP_USAGE   "Sintaxis: mapamundi [ancho [alto]]\n"
#define _LANG_CMD_WORLDMAP_RANGE   "Las dimensiones deben estar entre 3 y 80 de ancho, 3 y 40 de alto.\n"
#define _LANG_CMD_WORLDMAP_CODER   "No puedes cambiar el tamaño del mapa.\n"
#define _LANG_CMD_WORLDMAP_NOMAP   "Ahora mismo no estás en ningún mapa del mundo.\n"

// idle.c
#define _LANG_IDLE_ALIAS ({ "inactivo", "idle" })
#define _LANG_IDLE_SYNTAX "inactivo [<minutos>]"
#define _LANG_IDLE_HELP "Cuántos minutos se mantiene tu personaje en el mundo cuando dejas de\n" + \
                        "estar activo. El mismo valor cubre dos situaciones:\n" + \
                        "  - Si tu conexión sigue viva pero no envías nada durante este tiempo,\n" + \
                        "    se cierra tu sesión y se guarda tu progreso.\n" + \
                        "  - Si tu conexión se cae, tu personaje permanece en el mundo este\n" + \
                        "    tiempo para que puedas reconectarte y continuar donde lo dejaste.\n" + \
                        "    Pasado ese plazo tu personaje sale del mundo y en el próximo login\n" + \
                        "    empezarás de nuevo desde tu último estado guardado.\n" + \
                        "El combate no puede evitarse desconectándose porque tu personaje\n" + \
                        "permanece en el mundo durante ese tiempo.\n" + \
                        "`inactivo` sin argumentos muestra el valor actual y el rango permitido.\n" + \
                        "`inactivo <minutos>` fija un nuevo valor; cualquier valor fuera del rango se ajusta a los límites.\n" + \
                        "También responde a `idle`."
#define _LANG_IDLE_SHOW "La espera de idle es %d minutos (rango permitido %d – %d).\n"
#define _LANG_IDLE_SET "Espera de idle ajustada a %d minutos.\n"

// read. `str` is what the player named, and `ob` the thing being read
#define _LANG_CMD_READ_ALIAS ({ "leer" })
#define _LANG_CMD_READ_SYNTAX "leer <objeto>"
#define _LANG_CMD_READ_HELP "Lee lo que esté escrito en algo: un cartel, una placa, " + \
                            "un libro, una carta. Se nombra la cosa, y se lee lo que pone en ella.\n" + \
                            "\n" + \
                            "Cada texto está escrito en un idioma. Si no lo conoces, verás las letras " + \
                            "revueltas, y sabrás al menos en qué idioma está; usa 'hablar' para ver " + \
                            "cuáles sabes.\n" + \
                            "\n" + \
                            "Leer no es lo mismo que mirar: 'mirar <objeto>' te dice cómo es la cosa, " + \
                            "'leer <objeto>' te da lo que pone en ella."
#define _LANG_CMD_READ_WHAT "¿Leer el qué?\n"
#define _LANG_CMD_READ_CANNOT_FIND "No ves ningún '" + str + "' por aquí.\n"
#define _LANG_CMD_READ_NOTHING_WRITTEN ob->query_short() + " no tiene nada escrito.\n"

// group. `name` is the party's name, `who` the one being talked about, and
// `str` what the player typed
#define _LANG_CMD_GROUP_ALIAS ({ "grupo" })
#define _LANG_CMD_GROUP_SYNTAX "grupo <orden> [<nombre>]"
#define _LANG_CMD_GROUP_HELP "Un grupo reúne a varios jugadores para algo que quieren hacer juntos. " + \
    "Tiene un canal propio, se ve en el mapa quién va contigo, y dura lo que queráis: " + \
    "no se guarda de una partida a otra.\n" + \
    "\n" + \
    "Quien lo crea lo dirige, y es el único que puede invitar, expulsar, cambiarle el " + \
    "nombre o cederlo. Si se marcha, el grupo pasa a manos del siguiente. Para crear " + \
    "uno hace falta llevar unas horas de juego, aunque a cualquiera pueden invitarle.\n" + \
    "\n" + \
    "  grupo crear [<nombre>]     lo crea, contigo al frente.\n" + \
    "  grupo invitar <jugador>    le ofrece unirse; tiene un rato para aceptar.\n" + \
    "  grupo unirse               acepta la invitación que te hayan hecho.\n" + \
    "  grupo lista                quiénes sois.\n" + \
    "  grupo estado               cómo está de salud cada uno.\n" + \
    "  grupo canal <mensaje>      habla por el canal del grupo.\n" + \
    "  grupo despedirse           te sales.\n" + \
    "  grupo nombre <nombre>      le cambia el nombre.\n" + \
    "  grupo expulsar <jugador>   lo saca del grupo.\n" + \
    "  grupo lider <jugador>      le cedes el mando.\n" + \
    "  grupo finalizar            lo deshace.\n" + \
    "\n" + \
    "El canal del grupo también responde a 'aventurero <mensaje>'. Para que no te " + \
    "inviten a ninguno, mira 'consentir'."

#define _LANG_CMD_GROUP_CREATE ({ "crear" })
#define _LANG_CMD_GROUP_END ({ "finalizar" })
#define _LANG_CMD_GROUP_LEAVE ({ "despedirse" })
#define _LANG_CMD_GROUP_LIST ({ "lista" })
#define _LANG_CMD_GROUP_STATUS ({ "estado" })
#define _LANG_CMD_GROUP_NAME ({ "nombre" })
#define _LANG_CMD_GROUP_INVITE ({ "invitar" })
#define _LANG_CMD_GROUP_JOIN ({ "unirse" })
#define _LANG_CMD_GROUP_KICK ({ "expulsar" })
#define _LANG_CMD_GROUP_LEADER ({ "lider", "líder" })
#define _LANG_CMD_GROUP_CHANNEL ({ "canal" })

#define _LANG_CMD_GROUP_DEFAULT_NAME "Grupo de " + me->query_cap_name()
#define _LANG_CMD_GROUP_UNKNOWN_OPTION "No sé qué es eso. Mira 'ayuda grupo'.\n"
#define _LANG_CMD_GROUP_IN_PARTY "Perteneces al grupo '" + name + "'.\n"
#define _LANG_CMD_GROUP_NOT_IN_PARTY "Para eso tienes que pertenecer a un grupo.\n"
#define _LANG_CMD_GROUP_ALREADY_IN_PARTY "Ya perteneces a un grupo.\n"
#define _LANG_CMD_GROUP_NOT_LEADER "Sólo quien dirige el grupo puede hacer eso.\n"
#define _LANG_CMD_GROUP_TOO_YOUNG "Tu personaje es aún demasiado joven para dirigir un grupo.\n"
#define _LANG_CMD_GROUP_NAME_TOO_LONG "Ese nombre es demasiado largo.\n"
#define _LANG_CMD_GROUP_NEEDS_NAME "Tienes que decir un nombre.\n"
#define _LANG_CMD_GROUP_NEEDS_PLAYER "Tienes que decir a quién.\n"

#define _LANG_CMD_GROUP_CREATED_ME "Creas el grupo '" + name + "'.\n"
#define _LANG_CMD_GROUP_CREATED_ROOM me->query_cap_name() + " crea el grupo '" + name + "'.\n"
#define _LANG_CMD_GROUP_ENDED_ME "Das por terminado el grupo.\n"
#define _LANG_CMD_GROUP_ENDED_THEM me->query_cap_name() + " da por terminado el grupo.\n"
#define _LANG_CMD_GROUP_LEFT_ME "Te despides del grupo.\n"
#define _LANG_CMD_GROUP_LEFT_THEM me->query_cap_name() + " se despide del grupo.\n"
#define _LANG_CMD_GROUP_DISSOLVED "Al quedarte solo, el grupo se disuelve.\n"
#define _LANG_CMD_GROUP_LEADER_CANNOT_LEAVE "Diriges el grupo: cédelo a otro o dalo por terminado.\n"

#define _LANG_CMD_GROUP_LIST_HEADER "Grupo '" + name + "':\n"
#define _LANG_CMD_GROUP_LIST_LEADER " (al mando)"
#define _LANG_CMD_GROUP_RENAMED_ME "El grupo pasa a llamarse '" + name + "'.\n"
#define _LANG_CMD_GROUP_RENAMED_THEM me->query_cap_name() + " le cambia el nombre al grupo: '" + name + "'.\n"

#define _LANG_CMD_GROUP_NO_SUCH_PLAYER "No hay nadie conectado con ese nombre.\n"
#define _LANG_CMD_GROUP_NOT_YOURSELF "A ti mismo no.\n"
#define _LANG_CMD_GROUP_THEY_HAVE_PARTY who->query_cap_name() + " ya pertenece a un grupo.\n"
#define _LANG_CMD_GROUP_THEY_REFUSE who->query_cap_name() + " no quiere que le inviten a grupos.\n"
#define _LANG_CMD_GROUP_THEY_BLOCK who->query_cap_name() + " no quiere aceptar tus invitaciones.\n"
#define _LANG_CMD_GROUP_FULL "El grupo está completo.\n"
#define _LANG_CMD_GROUP_INVITED_ME "Invitas a " + who->query_cap_name() + " a tu grupo.\n"
#define _LANG_CMD_GROUP_INVITED_THEM me->query_cap_name() + " te invita al grupo '" + name + "'.\n" + \
    "Escribe 'grupo unirse' si quieres entrar; la invitación no dura mucho.\n"

#define _LANG_CMD_GROUP_NOT_INVITED "No te ha invitado nadie a ningún grupo.\n"
#define _LANG_CMD_GROUP_INVITE_GONE "Ese grupo ya no existe.\n"
#define _LANG_CMD_GROUP_JOINED_ME "Te unes al grupo '" + name + "'.\n"
#define _LANG_CMD_GROUP_JOINED_THEM me->query_cap_name() + " se une al grupo.\n"

#define _LANG_CMD_GROUP_NOT_A_MEMBER "En tu grupo no hay nadie con ese nombre.\n"
#define _LANG_CMD_GROUP_KICKED_ME "Expulsas a " + who->query_cap_name() + " del grupo.\n"
#define _LANG_CMD_GROUP_KICKED_THEM me->query_cap_name() + " te expulsa del grupo.\n"
#define _LANG_CMD_GROUP_KICKED_OTHERS me->query_cap_name() + " expulsa del grupo a " + who->query_cap_name() + ".\n"

#define _LANG_CMD_GROUP_ALREADY_LEADER "Ya diriges tú el grupo.\n"
#define _LANG_CMD_GROUP_HANDED_ME "Cedes el mando del grupo a " + who->query_cap_name() + ".\n"
#define _LANG_CMD_GROUP_HANDED_THEM me->query_cap_name() + " te cede el mando del grupo.\n"
#define _LANG_CMD_GROUP_HANDED_OTHERS me->query_cap_name() + " cede el mando del grupo a " + who->query_cap_name() + ".\n"

// description

#define _LANG_CMD_DESCRIPTION_ALIAS ({ "descripcion", "descripción" })
#define _LANG_CMD_DESCRIPTION_SYNTAX "descripción [<texto> | escribir | borrar]"
#define _LANG_CMD_DESCRIPTION_HELP "Escribe lo que ven los demás cuando te miran."
#define _LANG_CMD_DESCRIPTION_CLEAR ({ "borrar" })
#define _LANG_CMD_DESCRIPTION_EDIT ({ "escribir" })
#define _LANG_CMD_DESCRIPTION_CURRENT "Cuando te miran, ven:\n\n   " + me->query_description() + \
  "\n\n'descripción borrar' vuelve a la de por defecto.\n"
#define _LANG_CMD_DESCRIPTION_DEFAULT "No has escrito ninguna descripción, así que al mirarte " + \
  "se ve la de por defecto.\nEscribe 'descripción <texto>', o 'descripción escribir' para " + \
  "usar tu editor.\n"
#define _LANG_CMD_DESCRIPTION_SET "Ahora, cuando te miran, ven lo que has escrito.\n"
#define _LANG_CMD_DESCRIPTION_CLEARED "Vuelves a tener la descripción por defecto.\n"
#define _LANG_CMD_DESCRIPTION_UNCHANGED "Tu descripción no cambia.\n"
#define _LANG_CMD_DESCRIPTION_TOO_LONG "Es demasiado larga: como mucho " + \
  me->query_max_description_length() + " caracteres.\n"

// title

#define _LANG_CMD_TITLE_ALIASES ({ "titulo", "título", "title" })
#define _LANG_CMD_TITLE_SYNTAX "titulo\n" + \
                               "          titulo <número>\n" + \
                               "          titulo ninguno"
#define _LANG_CMD_TITLE_HELP "Lista los títulos que has conseguido y escoge cuál luces."
#define _LANG_CMD_TITLE_NO_TITLE ({ "ninguno", "nada", "none" })
#define _LANG_CMD_TITLE_NONE_EARNED "Todavía no has conseguido ningún título.\n"
#define _LANG_CMD_TITLE_HEADER "Tus títulos:\n"
#define _LANG_CMD_TITLE_LINE "  %2d) %-40s (%s)%s\n"
#define _LANG_CMD_TITLE_FROM_CLASS "clase"
#define _LANG_CMD_TITLE_FROM_GUILD "gremio"
#define _LANG_CMD_TITLE_FROM_JOB "oficio"
#define _LANG_CMD_TITLE_FROM_DEED "hazaña"
#define _LANG_CMD_TITLE_WORN "  [ lo luces ]"
#define _LANG_CMD_TITLE_FOOTER "Escribe 'titulo <número>' para lucirlo, " + \
                               "o 'titulo ninguno' para no lucir ninguno.\n"
#define _LANG_CMD_TITLE_CLEARED "Ya no luces ningún título.\n"
#define _LANG_CMD_TITLE_NOT_YOURS "No tienes ningún título con ese número.\n"
#define _LANG_CMD_TITLE_SET "Ahora luces el título '" + me->query_title() + "'.\n"

// quests

#define _LANG_CMD_QUESTS_ALIAS ({ "misiones", "mision", "misión" })
#define _LANG_CMD_QUESTS_SYNTAX "misiones [terminadas|info|aceptar|entregar|abandonar] [<número>]"
#define _LANG_CMD_QUESTS_HELP "Muestra lo que te ofrecen quienes están contigo y las " + \
      "misiones que llevas entre manos, con un número para cada una.\n" + \
      "  misiones                ofertas de aquí y misiones tuyas, numeradas\n" + \
      "  misiones terminadas     las que ya has entregado, por cadenas\n" + \
      "  misiones info <n>       todo el detalle de la número n\n" + \
      "  misiones aceptar <n>    aceptas una de las que te ofrecen\n" + \
      "  misiones entregar <n>   entregas una de las tuyas a quien la espera aquí\n" + \
      "  misiones abandonar <n>  dejas una de las tuyas, y pierdes lo avanzado\n" + \
      "Los números son los del listado de ese momento, y valen para todo. " + \
      "Para aceptar o entregar, si sólo hay una que encaje, el número sobra."

#define _LANG_CMD_QUESTS_TITLE "Misiones"
#define _LANG_CMD_QUESTS_NONE "No llevas ninguna misión entre manos.\n"
#define _LANG_CMD_QUESTS_OFFERS_FROM creature->query_cap_name() + " te ofrece:\n"
#define _LANG_CMD_QUESTS_YOURS "Tus misiones:\n"
#define _LANG_CMD_QUESTS_ENTRY "  [%^BOLD%^" + index + "%^RESET%^] " + quest->query_title()
#define _LANG_CMD_QUESTS_PROGRESS_ONE " (" + done + " de " + needed + ")"
#define _LANG_CMD_QUESTS_PROGRESS_MANY " (" + met + " de " + total + " objetivos)"
#define _LANG_CMD_QUESTS_FINISHED " (%^BOLD%^RED%^terminada%^RESET%^)"
#define _LANG_CMD_QUESTS_HAND_IN_HERE " [%^BOLD%^YELLOW%^?%^RESET%^] " + \
      "entrégasela a " + creature->query_cap_name()

#define _LANG_CMD_QUESTS_NEW_OFFERS creature->query_cap_name() + \
      " tiene nuevas misiones para ti.\n"

#define _LANG_CMD_QUESTS_HINT_LEAD "Escribe "
#define _LANG_CMD_QUESTS_ANY_NUMBER "<número>"
#define _LANG_CMD_QUESTS_HINT_INFO "'misiones info " + which + "' para ver más de una"
#define _LANG_CMD_QUESTS_HINT_ACCEPT "'misiones aceptar " + which + "' para aceptarla"
#define _LANG_CMD_QUESTS_HINT_HAND_IN "'misiones entregar " + which + "' para entregarla"
#define _LANG_CMD_QUESTS_HINT_ABANDON "'misiones abandonar " + which + "' para dejarla"

#define _LANG_CMD_QUESTS_OBJECTIVE_LINE "- " + objectives[j][OBJ_TEXT] + " (" + \
      progress[j] + " de " + objectives[j][OBJ_COUNT] + ")\n"

#define _LANG_CMD_QUESTS_NO_SUCH "No hay ninguna misión con ese número.\n"
#define _LANG_CMD_QUESTS_NOT_AN_OFFER "Esa no es una de las que te ofrecen aquí.\n"
#define _LANG_CMD_QUESTS_NOT_YOURS "Esa no es una de tus misiones.\n"
#define _LANG_CMD_QUESTS_CANNOT_HAND_IN "Esa misión no se puede entregar aquí.\n"
#define _LANG_CMD_QUESTS_WHICH "¿Cuál? Dilo con su número.\n"

#define _LANG_CMD_QUESTS_CHAIN_INTRO "Esta misión es parte de la cadena:\n\n"
#define _LANG_CMD_QUESTS_CHAIN_STEPS_INTRO "Esta cadena está compuesta de las misiones:\n\n"
#define _LANG_CMD_QUESTS_CHAIN_TITLE "%^BOLD%^" + chain_title + "%^RESET%^\n"
#define _LANG_CMD_QUESTS_CHAIN_STEP "  " + (j + 1) + ". " + \
      step_quest->query_title() + " " + mark + "\n"
#define _LANG_CMD_QUESTS_STEP_DONE "(entregada)"
#define _LANG_CMD_QUESTS_STEP_HERE "(esta misma)"
#define _LANG_CMD_QUESTS_STEP_TO_COME "(aún no)"

#define _LANG_CMD_QUESTS_DONE_HEADER "Misiones terminadas"
#define _LANG_CMD_QUESTS_DONE_NONE "No has entregado ninguna misión todavía.\n"
#define _LANG_CMD_QUESTS_DONE_ENTRY "  [%^BOLD%^" + index + "%^RESET%^] " + \
      quest->query_title() + \
      (done[id][QUEST_TIMES] > 1 ? " (" + done[id][QUEST_TIMES] + " veces)" : "") + \
      ", " + ctime(done[id][QUEST_LAST], 4) + "\n"
#define _LANG_CMD_QUESTS_DONE_OPTIONS ({ "terminadas", "hechas" })

#define _LANG_CMD_QUESTS_INFO_OPTIONS ({ "info" })
#define _LANG_CMD_QUESTS_ACCEPT_OPTIONS ({ "aceptar" })
#define _LANG_CMD_QUESTS_HAND_IN_OPTIONS ({ "entregar" })
#define _LANG_CMD_QUESTS_ABANDON_OPTIONS ({ "abandonar", "dejar" })
