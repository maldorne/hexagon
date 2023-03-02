
// common

#define _LANG_CMD_NOTHING_HERE "Nothing with the name '"+arg+"' is here.\n"

// kill.c

#define _LANG_CMD_KILL_ALIAS ({ "kill", "attack" })
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

#define _LANG_CMD_LOOK_ALIAS ({ "look" })
#define _LANG_CMD_LOOK_SYNTAX "look [<something>]"
#define _LANG_CMD_LOOK_HELP "Shows the description of an item or (by default) " + \
                    "the localization you are in at the moment."
#define _LANG_CMD_LOOK_NO_ENVIRONMENT "You are nowhere... you can't see a thing.\n"
#define _LANG_CMD_LOOK_BLINDED "You can't see a thing. ¡You are blinded!\n"

// glance

#define _LANG_CMD_GLANCE_ALIAS ({ "glance" })
#define _LANG_CMD_GLANCE_SYNTAX "glance [<item>]"
#define _LANG_CMD_GLANCE_HELP "Returns a short description of an item or the place where you are."
#define _LANG_CMD_LOOK_TOO_DARK "It's too dark to see something.\n"
#define _LANG_CMD_LOOK_TOO_BRIGHT "It's too bright, you cannot see.\n"
#define _LANG_CMD_GLANCE_YOURSELF ", it is yourself!\n"

// prompt

#define _LANG_CMD_PROMPT_ALIAS ({ "prompt" })
#define _LANG_PROMPT_CLEAN "clean"
#define _LANG_PROMPT_CURRENT "Your current prompt is: '" + user->query_prompt_string() + "'\n"
#define _LANG_PROMPT_SYNTAX "prompt <text>"
#define _LANG_PROMPT_HELP "Changes or removes the current prompt.\n" + \
           "You can change it using 'prompt <text>', or remove it using 'prompt " + \
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

// commands

#define _LANG_CMD_COMMANDS_ALIAS ({ "commands" })
#define _LANG_CMD_COMMANDS_SYNTAX "commands [-l]"
#define _LANG_CMD_COMMANDS_HELP "Show every command available.\n" + \
                                "commands -l to include more information.\n"
#define _LANG_CMD_COMMANDS_CATEGORY_MSG "\n%^GREEN%^" + capitalize(categories[i]) + " commands:%^RESET%^\n"

// retire

#define _LANG_CMD_RETIRE_ALIAS ({ "retire" })
#define _LANG_CMD_RETIRE_SYNTAX "retire"
#define _LANG_CMD_RETIRE_HELP "Delete all your data and your character."
#define _LANG_CMD_RETIRE_GUEST "You are a guest in " + mud_name() + ".\n" + \
        "You don't have a character, so you cannot retire yet.\n"

// configuration

#define _LANG_CMD_CONFIG_ALIAS ({ "configuration", "config" })
#define _LANG_CMD_CONFIG_SYNTAX "configuration <category> <subcategory> <value>"
#define _LANG_CMD_CONFIG_HELP "Sets various player configuration values."
#define _LANG_CMD_CONFIG_NOT_VALID_CATEGORY "'"+str+"' is not a valid category to configure.\n"
#define _LANG_CMD_CONFIG_NOT_VALID_TOPIC "'"+str+"' is not a valid topic to configure.\n"
#define _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY "  Topics available in " + title + ":\n"
#define _LANG_CMD_CONFIG_FOR_USER this_player()->query_cap_name() + " configuration"
#define _LANG_CMD_CONFIG_CURRENT_CONFIG_FOR "Current configuration for "
#define _LANG_CMD_CONFIG_UNKNOWN "Unknown"
