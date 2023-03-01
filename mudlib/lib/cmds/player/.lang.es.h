
// common

#define _LANG_CMD_NOTHING_HERE "No hay nada con el nombre '"+arg+"' aquí.\n"

// kill.c

#define _LANG_CMD_KILL_ALIAS ({ "matar", "atacar" })
#define _LANG_CMD_KILL_SYNTAX "Sintaxis: atacar <objetivos>"
#define _LANG_CMD_KILL_HELP "Comienza a atacar a los objetivos especificados."
#define _LANG_CMD_KILL_NOT_ALLOWED "No está permitido.\n"
#define _LANG_CMD_KILL_NOT_ALLOWED_ARRAY ({ "todo", "todos", "all", "everyone" })
#define _LANG_CMD_KILL_NO_NEED_WHEN_DEAD "Tu estado etéreo tiene poco efecto en el mundo material.\n"
#define _LANG_CMD_KILL_NOT_FOUND "No consigues encontrar a "+me->expand_nickname(str)+".\n"
#define _LANG_CMD_KILL_NOT_SUICIDE "¿Te quieres suicidar?\n"
#define _LANG_CMD_KILL_NOT_ALIVE "Intentas golpear a " + str + ", pero " + str + " no " + \
                                 "parece darse cuenta.\n"
#define _LANG_CMD_KILL_START "Ok. Comienzas a pelear con " + query_multiple_short(res, 0) + ".\n"

// look.c

#define _LANG_CMD_LOOK_ALIAS ({ "mirar" })
#define _LANG_CMD_LOOK_SYNTAX "Sintaxis: mirar [a] [<objeto>]"
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

#define _LANG_WHOAMI_HELP "Muestra onformación sobre tu propio personaje."
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

#define _LANG_CMD_COMMANDS_ALIAS ({ "comandos "})
#define _LANG_CMD_COMMANDS_SYNTAX "comandos [-l]"
#define _LANG_CMD_COMMANDS_HELP "Muestra todos los comandos que tienes disponibles.\n" + \
                                "comandos -l para una lista con más información."
#define _LANG_CMD_COMMANDS_CATEGORY_MSG "\n%^GREEN%^Comandos de " + categories[i] + ":%^RESET%^\n"
