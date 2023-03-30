
// common

#define _LANG_CMD_NOTHING_HERE "No hay nada con el nombre '"+arg+"' aquí.\n"
#define _LANG_CMD_DEAD "Estás en forma espiritual, no necesitas conocer eso.\n"

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
           (user->query_coder() ? "     $~ -> path actual\n" : "") + \
           "\nPor ejemplo: 'prompt $n ($h)' mostrará en cada línea:\n" + \
           "     " + me->query_name() + " (" + me->query_hp() + ") >\n"
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
#define _LANG_CMD_CONFIG_SYNTAX "configuracion <categoría> <tema> <valor>"
#define _LANG_CMD_CONFIG_HELP "Establece diversos valores de configuración del jugador."
#define _LANG_CMD_CONFIG_NOT_VALID_CATEGORY "'"+str+"' no es una categoría válida a configurar.\n"
#define _LANG_CMD_CONFIG_NOT_VALID_TOPIC "'"+str+"' no es un tema válido a configurar.\n"
#define _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY "  Valores configurables dentro de %^BOLD%^"+title+"%^RESET%^:\n"
#define _LANG_CMD_CONFIG_FOR_USER "Configuración de " + this_player()->query_cap_name()
#define _LANG_CMD_CONFIG_CURRENT_CONFIG_FOR "Configuración actual para "
#define _LANG_CMD_CONFIG_UNKNOWN "Desconocido"
#define _LANG_CMD_CONFIG_USE_COMMANDS "  Entre corchetes se indica el comando específico\n" + \
        "  que puedes usar para obtener más información.\n"
#define _LANG_CMD_CONFIG_HINT "  Usa '" + _LANG_CMD_CONFIG_SYNTAX + "'\n  para cambiar los valores.\n"
#define _LANG_CMD_CONFIG_USER "  Recuerda que estas configuraciones son a nivel de usuario,\n" + \
        "  por lo que son comunes a todos tus personajes.\n"

// sheet

#define _LANG_CMD_SHEET_ALIAS ({ "ficha" })
#define _LANG_CMD_SHEET_SYNTAX "ficha"
#define _LANG_CMD_SHEET_HELP "Muestra datos extendidos sobre tu personaje."

#define _LANG_CMD_SHEET_NAME "Nombre"
#define _LANG_CMD_SHEET_CANNOT_FIND "Lo siento, no encuentro a '"+name+"'.\n"
#define _LANG_CMD_SHEET_BASIC_CHARACTERISTICS "Características básicas"
#define _LANG_CMD_SHEET_CAN_ALSO_USE "También puedes usar: 'habilidades', " + \
        "'maestrías' y 'estilos'.\n"

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
#define _LANG_CMD_TIME_HELP "Muestra la hora actual del mundo real."
