
// common

#define _LANG_CMD_NOTHING_HERE "Nothing with the name '"+arg+"' is here.\n"
#define _LANG_CMD_DEAD "You are in spiritual form, you don't need to know that.\n"

// kill.c

#define _LANG_CMD_KILL_ALIAS ({ "kill", "attack" })
#define _LANG_CMD_KILL_SYNTAX "kill <targets>"
#define _LANG_CMD_KILL_HELP "Start attacking the provided targets."
#define _LANG_CMD_KILL_NOT_ALLOWED "Not allowed.\n"
#define _LANG_CMD_KILL_NOT_ALLOWED_ARRAY ({ "all", "everyone", "*" })
#define _LANG_CMD_KILL_NO_NEED_WHEN_DEAD "In your current state your don't have effect in the material world.\n"
#define _LANG_CMD_KILL_NOT_FOUND "You cannot find "+me->expand_nickname(str)+".\n"
#define _LANG_CMD_KILL_NOT_SUICIDE "You want to commit suicide?\n"
#define _LANG_CMD_KILL_NOT_ALIVE "You try to hit " + str + ", but " + str + " does not seem " + \
                                 "to notice it.\n"
#define _LANG_CMD_KILL_START "Ok. You start fighting with " + query_multiple_short(res, 0) + ".\n"
#define _LANG_CMD_KILL_NO_FIGHT "You cannot fight here.\n"

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

#define _LANG_CMD_WHOAMI_ALIAS ({ "whoami" })
#define _LANG_CMD_WHOAMI_SYNTAX "whoami"
#define _LANG_CMD_WHOAMI_HELP "Shows information about your own character."
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

// score

#define _LANG_CMD_SCORE_ALIAS ({ "score" })
#define _LANG_CMD_SCORE_SYNTAX "score [-d]"
#define _LANG_CMD_SCORE_HELP "Show some basic information about your character.\n" + \
        "Use -d to get more detailed information."
#define _LANG_CMD_SCORE_AGE "You are " + seconds_to_time_string(age) + " old.\n";

// configuration

#define _LANG_CMD_CONFIG_ALIAS ({ "configuration", "config", "settings" })
#define _LANG_CMD_CONFIG_SYNTAX "configuration <category> <topic> <value>"
#define _LANG_CMD_CONFIG_HELP "Sets various player configuration values."
#define _LANG_CMD_CONFIG_NOT_VALID_CATEGORY "'"+str+"' is not a valid category to configure.\n"
#define _LANG_CMD_CONFIG_NOT_VALID_TOPIC "'"+str+"' is not a valid topic to configure.\n"
#define _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY "  Topics available in %^BOLD%^" + title + "%^RESET%^:\n"
#define _LANG_CMD_CONFIG_FOR_USER this_player()->query_cap_name() + " configuration"
#define _LANG_CMD_CONFIG_CURRENT_CONFIG_FOR "Current configuration for "
#define _LANG_CMD_CONFIG_UNKNOWN "Unknown"
#define _LANG_CMD_CONFIG_USE_COMMANDS "  Between brackets you can see the specific command\n" + \
        "  that you can use to get more information.\n"
#define _LANG_CMD_CONFIG_HINT "  Use '" + _LANG_CMD_CONFIG_SYNTAX + "'\n  to change the values.\n"
#define _LANG_CMD_CONFIG_USER "  Remember these configurations are made at user-level,\n  so they are " + \
        "common to all your characters.\n"

// sheet

#define _LANG_CMD_SHEET_ALIAS ({ "sheet" })
#define _LANG_CMD_SHEET_SYNTAX "sheet"
#define _LANG_CMD_SHEET_HELP "Show extended information about your character."

#define _LANG_CMD_SHEET_NAME "Name"
#define _LANG_CMD_SHEET_CANNOT_FIND "Sorry, but I cannot find any '"+name+"'.\n"
#define _LANG_CMD_SHEET_BASIC_CHARACTERISTICS "Basic characteristics"
#define _LANG_CMD_SHEET_CAN_ALSO_USE "You can also use: 'skills', 'masteries' and " + \
        "'styles'.\n"

// verbose

#define _LANG_CMD_VERBOSE_ALIAS ({ "verbose" })
#define _LANG_CMD_VERBOSE_SYNTAX "verbose <yes|no>"
#define _LANG_CMD_VERBOSE_HELP "Configures longer descriptions for various actions."
#define _LANG_CMD_VERBOSE_ON "Ok, you are using verbose mode.\n"
#define _LANG_CMD_VERBOSE_OFF "Ok, you are not using verbose mode.\n"
#define _LANG_CMD_VERBOSE_CURRENT_ON "You are currently using verbose mode.\n"
#define _LANG_CMD_VERBOSE_CURRENT_OFF "You are currently not using verbose mode.\n"

// inform

#define _LANG_CMD_INFORM_ALIAS ({ "inform" })
#define _LANG_CMD_INFORM_SYNTAX "inform [event <yes|no>]"
#define _LANG_CMD_INFORM_HELP "Shows your automatic event information settings."
#define _LANG_INFORM_HEADER "Status of automatic events information:\n"
#define _LANG_INFORM_BLOCKED "Every event is being repressed.\n" + \
        "Use 'inform yes' to unblock them.\n"
#define _LANG_INFORM_EVENTS_UNBLOCKED "Events are unblocked.\n"
#define _LANG_INFORM_EVENTS_BLOCKED "Events are blocked.\n"
#define _LANG_INFORM_ALL_ON "Every event is being informed.\n"
#define _LANG_INFORM_ALL_OFF "Every event is being blocked.\n"
#define _LANG_INFORM_UNKNOWN_EVENT "Event type unknown.\n"
#define _LANG_INFORM_ON "Events of type '" + frog[0] + "' are being informed.\n"
#define _LANG_INFORM_OFF "Events of type '" + frog[0] + "' are being blocked.\n"

// bury

#define _LANG_CMD_BURY_ALIAS ({ "bury" })
#define _LANG_CMD_BURY_SYNTAX "bury"
#define _LANG_CMD_BURY_HELP "Buries every corpse in your location. " + \
        "Nothing from the corpses will be recoverable."
#define _LANG_CMD_BURY_DEAD "You, a spirit, cannot bury corpses.\n"
#define _LANG_CMD_BURY_WATER "Do you want to bury something in the water?\n"
#define _LANG_CMD_BURY_MSG "You bury " + (counter == 1 ? "the corpse" : "the corpses") + ".\n"
#define _LANG_CMD_BURY_NO "You haven't found any corpses.\n"

// cost

#define _LANG_CMD_COST_ALIAS ({ "cost" })
#define _LANG_CMD_COST_SYNTAX "cost"
#define _LANG_CMD_COST_HELP "Shows your current experience and how much you need to advance your level."
#define _LANG_CMD_COST_INTRO "You will advance your level in your:\n"
#define _LANG_CMD_COST_CLASS "   - Class with: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ experience points (you have "+me->query_xp()+").\n"
#define _LANG_CMD_COST_GUILD "   - Guild with: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ experience points (you have "+me->query_xp()+").\n"
#define _LANG_CMD_COST_JOB   "   - Job   with: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ job experience points (you have "+me->query_job_xp()+").\n"

// stop

#define _LANG_CMD_STOP_ALIAS ({ "stop" })
#define _LANG_CMD_STOP_SYNTAX "stop <fights | follows>"
#define _LANG_CMD_STOP_HELP "Stops your pending fights or chases."
#define _LANG_CMD_STOP_NOT_FIGHTING "You are not fighting with anyone.\n"
#define _LANG_CMD_STOP_MSG "You stop fighting with " + query_multiple_short(obs)+".\n"
#define _LANG_CMD_STOP_FOLLOW_OPTIONS ({ "follow", "follows", "following", "followings", "chase", "chases", "chasings" })
#define _LANG_CMD_STOP_NOT_FOLLOWING "You are not following anyone.\n"
#define _LANG_CMD_STOP_FIGHT_OPTIONS ({ "fights" })
#define _LANG_CMD_STOP_FIGHTS_OK "Ok. Stopping fights, it will take a while...\n"
#define _LANG_CMD_STOP_ALREADY_STOPPING "You already are stopping fights, be patient.\n"

// encumbrance

#define _LANG_CMD_ENCUMBRANCE_ALIAS ({ "encumbrance"})
#define _LANG_CMD_ENCUMBRANCE_SYNTAX "encumbrance"
#define _LANG_CMD_ENCUMBRANCE_HELP "Shows your current and maximum encumbrance."
#define _LANG_CMD_ENCUMBRANCE_MSG "You current inventory weight is "+now+" and you can carry "+max+" thanks to your strength.\n"

// wimpy

#define _LANG_CMD_WIMPY_ALIAS ({ "wimpy" })
#define _LANG_CMD_WIMPY_SYNTAX "wimpy <num>"
#define _LANG_CMD_WIMPY_HELP "Sets the percentage of life you have to be at to make your character try to run away."
#define _LANG_CMD_WIMPY_NO_NUMBER "You have to put a number.\n"
#define _LANG_CMD_WIMPY_NO_VALID "You have to put a number between 0 and 100. Currently set to ( "+me->query_wimpy()+" % ).\n"
#define _LANG_CMD_WIMPY_BRAVE "Ok. You are now in brave mode.\n"
#define _LANG_CMD_WIMPY_MSG "Ok. Your bravery will make you not to run away until you are below "+wimpy+"% of your life.\n"

// money

#define _LANG_CMD_MONEY_ALIAS ({ "money" })
#define _LANG_CMD_MONEY_SYNTAX "money"
#define _LANG_CMD_MONEY_HELP "Shows the amount of money you have on you and in the banks."
#define _LANG_CMD_MONEY_ON_YOU "You have on you coins for a total of:\n   " + MONEY_HAND->money_value_string(value) + "\n\n"
#define _LANG_CMD_MONEY_ON_BANK "In ["+bank->query_short()+"] you have the following amount:\n   "
#define _LANG_CMD_MONEY_ON_BANK_EMPTY "In ["+bank->query_short()+"] you have an account with no money.\n"
#define _LANG_CMD_MONEY_TOTAL "\nIn total, you have:\n   " + MONEY_HAND->money_value_string(total) + "\n"
#define _LANG_CMD_MONEY_ON_YOU_EMPTY "You have no money on you.\n"

// condition

#define _LANG_CMD_CONDITION_ALIAS ({ "condition" })
#define _LANG_CMD_CONDITION_SYNTAX "condition [<name> | all]"
#define _LANG_CMD_CONDITION_HELP "Shows the health status of a character or of all the characters in the location."
#define _LANG_CMD_CONDITION_OPTIONS "Syntax: 'condition' to check your health status, or\n" + \
                                    "        'condition <name>' to check the health status of another, or\n" + \
                                    "        'condition all' to check the health status of all the characters in the location.\n"
#define _LANG_CMD_CONDITION_IS_DEAD "Is dead."
#define _LANG_CMD_CONDITION_HEADER "Health status of:\n"

// combats

#define _LANG_CMD_COMBATS_ALIAS ({ "combats" })
#define _LANG_CMD_COMBATS_SYNTAX "combats"
#define _LANG_CMD_COMBATS_HELP "Shows the characters you are fighting with."
#define _LANG_CMD_COMBATS_NOBODY "You are not fighting with anyone.\n"
#define _LANG_CMD_COMBATS_MSG "You are fighting with " + query_multiple_short(obs)+".\n"

// past

#define _LANG_CMD_PAST_ALIAS ({ "past" })
#define _LANG_CMD_PAST_SYNTAX "past"
#define _LANG_CMD_PAST_HELP "Shows the history of what other characters have said to you."

// inventory

#define _LANG_CMD_INVENTORY_ALIAS ({ "inventory" })
#define _LANG_CMD_INVENTORY_SYNTAX "inventory"
#define _LANG_CMD_INVENTORY_HELP "Shows the inventory of your character."

// away

#define _LANG_CMD_AWAY_ALIAS ({ "away" })
#define _LANG_CMD_AWAY_SYNTAX "away [message]"
#define _LANG_CMD_AWAY_HELP "Sets an away message so other players can see it when they talk to you."
#define _LANG_CMD_AWAY_NONE "You have not set any away message.\n"
#define _LANG_CMD_AWAY_REMOVED "Away message removed.\n"
#define _LANG_CMD_AWAY_OK "Away message set to: %^BOLD%^RED%^'" + str + "'%^RESET%^.\n"

// consider

#define _LANG_CMD_CONSIDER_ALIAS ({ "consider" })
#define _LANG_CMD_CONSIDER_SYNTAX "consider <name>"
#define _LANG_CMD_CONSIDER_HELP "Shows the difference of level between your character and another."
#define _LANG_CMD_CONSIDER_EXIST "You need to consider something that exists.\n"
#define _LANG_CMD_CONSIDER_CODER "Attacking a coder is not recommended.\n"
#define _LANG_CMD_CONSIDER_15 "Do not even think about attacking "+obs[i]->query_cap_name()+", you would be dead in seconds.\n"
#define _LANG_CMD_CONSIDER_10 "You are crazy if you want to attack "+obs[i]->query_cap_name()+"!\n"
#define _LANG_CMD_CONSIDER_M10 "You could kill "+obs[i]->query_cap_name()+" with a single blow!\n"
#define _LANG_CMD_CONSIDER_NOT_THAT "You cannot consider that.\n"
#define _LANG_CMD_CONSIDER_14 ({ + \
          " is too weak to bother attacking.\n", + \
          " is much weaker than you.\n", + \
          " is pretty weaker than you.\n", + \
          " is weaker than you.\n", + \
          " is a little weaker than you.\n", + \
          " is about the same strength as you.\n", + \
          " is a little stronger than you.\n", + \
          " is stronger than you.\n", + \
          " is pretty stronger than you.\n", + \
          " is much stronger than you.\n", + \
          " is the strongest of the region.\n" })
#define _LANG_CMD_CONSIDER_NOT_14 ({ + \
          " is too weak to bother attacking.\n", + \
          " is weaker than you.\n", + \
          " is about the same strength as you.\n", + \
          " is stronger than you.\n", + \
          " is the strongest of the region.\n" })

// travel

#define _LANG_CMD_TRAVEL_ALIAS ({ "travel" })
#define _LANG_CMD_TRAVEL_SYNTAX "travel [<direction>]"
#define _LANG_CMD_TRAVEL_HELP "Allows you to travel in a certain direction.\nIt needs you to be on a mount or vehicle."
#define _LANG_CMD_TRAVEL_NO_MOUNT "It only works if you are on a mount or vehicle.\n"
#define _LANG_CMD_TRAVEL_WHERE "Travel where?\n"

// equipment

#define _LANG_CMD_EQUIPMENT_ALIAS ({ "equipment" })
#define _LANG_CMD_EQUIPMENT_SYNTAX "equipment [all]"
#define _LANG_CMD_EQUIPMENT_HELP "Shows the current status of your equipment."
#define _LANG_CMD_EQUIPMENT_HEADER "Status of your equipment:\n"
#define _LANG_CMD_EQUIPMENT_WEAP "  Weapons:\n"
#define _LANG_CMD_EQUIPMENT_SHIE "  Shields:\n"
#define _LANG_CMD_EQUIPMENT_ARMO "  Armours:\n"
#define _LANG_CMD_EQUIPMENT_NOTHING "  You are not carrying or wearing anything.\n"
#define _LANG_CMD_EQUIPMENT_FOOTER "Use 'equipment all' to see the status of all the equipment you are carrying.\n"

// pov

#define _LANG_CMD_POV_ALIAS ({ "pov" })
#define _LANG_CMD_POV_SYNTAX "pov < "+implode( POV_TYPES, " | ")+" >"
#define _LANG_CMD_POV_HELP "Change the way you see the other characters in your environment."
#define _LANG_CMD_POV_CURRENT "Your current point of view is: "
#define _LANG_CMD_POV_MSG "Ok, point of view changed to '" + str + "'.\n"

// time

#define _LANG_CMD_TIME_ALIAS ({ "time", "date" })
#define _LANG_CMD_TIME_SYNTAX "time"
#define _LANG_CMD_TIME_HELP "Shows the current time in the real world."
