
// common

#define _LANG_CMD_NOTHING_HERE "Nothing with the name '"+arg+"' is here.\n"

// kill.c

#define _LANG_CMD_KILL_SYNTAX "kill <targets>"
#define _LANG_CMD_KILL_HELP "Start attacking the provided targets."
#define _LANG_CMD_KILL_NOT_ALLOWED "Not allowed.\n"
#define _LANG_CMD_KILL_NOT_ALLOWED_ARRAY ({ "all", "everyone" })
#define _LANG_CMD_KILL_NO_NEED_WHEN_DEAD "In your current state your don't have effect in the material world.\n"
#define _LANG_CMD_KILL_NOT_FOUND "You cannot find "+me->expand_nickname(str)+".\n"
#define _LANG_CMD_KILL_NOT_SUICIDE "You want to commit suicide?\n"
#define _LANG_CMD_KILL_NOT_ALIVE "You try to hit " + str + ", but " + str + " does not seem " + \
                                 "to notice it.\n"
#define _LANG_CMD_KILL_START "Ok. You start fighting with " + query_multiple_short(res, 0) + ".\n"

// look.c

#define _LANG_CMD_LOOK "look"
#define _LANG_CMD_LOOK_SYNTAX "look [<something>]"
#define _LANG_CMD_LOOK_HELP "Shows the description of an item or (by default) " + \
                    "the localization you are in at the moment."
#define _LANG_CMD_LOOK_NO_ENVIRONMENT "You are nowhere... you can't see a thing.\n"
#define _LANG_CMD_LOOK_BLINDED "You can't see a thing. ¡You are blinded!\n"

// prompt

#define _LANG_PROMPT_CLEAN "clean"
#define _LANG_PROMPT_CURRENT "Your current prompt is: '" + user->query_prompt_string() + "'\n"
#define _LANG_PROMPT_SYNTAX "prompt <text>"
#define _LANG_PROMPT_HELP "You can change it using 'prompt <text>', or remove it using 'prompt " + \
           _LANG_PROMPT_CLEAN + "'.\n" + \
           "In <text>, the following shortcuts are accepted:\n" + \
           "     $n -> your name\n" + \
           "     $h -> your current health\n" + \
           "     $g -> your current stamina\n" + \
           "     $B -> start bold text\n" + \
           "     $m -> mud name\n" + \
           (user->query_coder() ? "     $~ -> current path\n" : "") + \
           "\ni.e.: 'prompt $n ($h)' will show:\n" + \
           "     " + me->query_name() + " (" + me->query_hp() + ") >\n"
#define _LANG_PROMPT_OK "Ok, prompt changed.\n"

// whoami

#define _LANG_WHOAMI_HELP "Shows information about my own character."
#define _LANG_WHOAMI_CLASS_FIRST "   Class:\n"
#define _LANG_WHOAMI_CLASS_EACH "     " +  ordinal(me->query_level()) + " level " + \
                    (me->query_class_ob())->query_short()
#define _LANG_WHOAMI_GUILD_FIRST "   Guilds:\n"
#define _LANG_WHOAMI_GUILD_EACH "     " +  ordinal(guilds[g_names[i]][0]) + " level " + \
                    capitalize(g_names[i]->query_short())
#define _LANG_WHOAMI_JOB_FIRST "   Jobs:\n"
#define _LANG_WHOAMI_JOB_EACH "     " +  ordinal(jobs[j_names[i]][0]) + " level " + \
                    capitalize(j_names[i]->query_short())
