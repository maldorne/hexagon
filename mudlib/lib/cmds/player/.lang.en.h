
// common

#define _LANG_CMD_NOTHING_HERE "Nothing with the name '"+arg+"' is here.\n"
#define _LANG_CMD_DEAD "You are in spiritual form, you don't need to know that.\n"

// calendar.c

#define _LANG_CMD_CALENDAR_ALIAS       ({ "calendar" })
#define _LANG_CMD_CALENDAR_SYNTAX      "calendar [years]"
#define _LANG_CMD_CALENDAR_HELP        "Shows the calendar for the current month. With the " + \
                                       "'years' argument lists the names of every past year."
#define _LANG_CMD_CALENDAR_ARG_YEARS   ({ "years" })
#define _LANG_CMD_CALENDAR_ERA_HEADER  "Calendar of the current era:\n\n"
#define _LANG_CMD_CALENDAR_TITLE       "Calendar"
#define _LANG_CMD_CALENDAR_USAGE       "Syntax: calendar [years]\n"
#define _LANG_CMD_CALENDAR_WEEK_HEADER "  M   T   W   T   F   S   S\n"

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

// speak (set the language you speak / write). `name` is the language's
// real-world display name; `str` the raw input the player typed.
#define _LANG_CMD_SPEAK_ALIAS ({ "speak" })
#define _LANG_CMD_SPEAK_SYNTAX "speak [<language>]"
#define _LANG_CMD_SPEAK_HELP "Choose the language you speak and write in. With " + \
                    "no argument it shows your current language and the ones you know."
#define _LANG_CMD_SPEAK_SELECT "You must choose a language to speak.\n"
#define _LANG_CMD_SPEAK_CURRENT "You are now speaking " + name + ".\n"
#define _LANG_CMD_SPEAK_KNOWN_HEADER "You can speak the following languages:\n"
#define _LANG_CMD_SPEAK_NONE "You do not know any language.\n"
#define _LANG_CMD_SPEAK_UNKNOWN "You do not know the language '" + str + "'.\n"
#define _LANG_CMD_SPEAK_NOW_USING "Now using " + name + " to speak and write.\n"

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
           ((user && user->query_coder()) ? "     $~ -> current path\n" : "") + \
           "\ni.e.: 'prompt $n ($h)' will show:\n" + \
           "     " + (me ? me->query_name() : "$n") + \
           " (" + (me ? "" + me->query_hp() : "$h") + ") >\n"
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
#define _LANG_CMD_CONFIG_SYNTAX "configuration [<category> [<topic> [<value>]]]"
#define _LANG_CMD_CONFIG_HELP "Shows and changes, from one place, what you can adjust of your account and your character."
#define _LANG_CMD_CONFIG_NOT_VALID_CATEGORY "'" + words[0] + "' is not a configuration category.\n"
#define _LANG_CMD_CONFIG_NOT_VALID_TOPIC "'" + words[1] + "' is not something configured in " + title + ".\n"
#define _LANG_CMD_CONFIG_NOT_VALID_VALUE "'" + value + "' does not do for " + title + " / " + topic + ". "
#define _LANG_CMD_CONFIG_CANNOT_SET "That cannot be changed right now.\n"
#define _LANG_CMD_CONFIG_REFUSED title + " / " + topic + ": it cannot be changed right now.\n"
#define _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY "  %^BOLD%^" + title + "%^RESET%^ " + owner + ":\n"
#define _LANG_CMD_CONFIG_OWNER_USER "(of your account, the same for all your characters)"
#define _LANG_CMD_CONFIG_OWNER_PLAYER "(of this character)"
#define _LANG_CMD_CONFIG_FOR_USER me->query_cap_name() + " configuration"
#define _LANG_CMD_CONFIG_CURRENT title + " / " + topic + ": [ " + shown + " ]\n"
#define _LANG_CMD_CONFIG_ADJUSTED title + " / " + topic + ": [ " + shown + " ] (what you asked for was adjusted)\n"
#define _LANG_CMD_CONFIG_ACCEPTS_BOOL "It takes yes or no.\n"
#define _LANG_CMD_CONFIG_ACCEPTS_INT "It takes a number between " + setting[CONFIG_MIN] + " and " + setting[CONFIG_MAX] + ".\n"
#define _LANG_CMD_CONFIG_ACCEPTS_CHOICE "It takes: " + implode(setting[CONFIG_OPTIONS], ", ") + ".\n"
#define _LANG_CMD_CONFIG_ACCEPTS_STRING "It takes any text, or 'none' to leave it empty.\n"
#define _LANG_CMD_CONFIG_USE_COMMANDS "  Between brackets, the command that does the same and has its own help.\n"
#define _LANG_CMD_CONFIG_HINT "  'configuration <category> <topic>' tells what each thing takes,\n" + \
  "  and 'configuration <category> <topic> <value>' changes it.\n"

// sheet

#define _LANG_CMD_SHEET_ALIAS ({ "sheet" })
#define _LANG_CMD_SHEET_SYNTAX "sheet"
#define _LANG_CMD_SHEET_HELP "Show extended information about your character."

#define _LANG_CMD_SHEET_NAME "Name"
#define _LANG_CMD_SHEET_CANNOT_FIND "Sorry, but I cannot find any '"+name+"'.\n"
#define _LANG_CMD_SHEET_BASIC_CHARACTERISTICS "Basic characteristics"
#define _LANG_CMD_SHEET_CAN_ALSO_USE "You can also use: 'skills', 'masteries' and " + \
        "'styles'.\n"
#define _LANG_STATS_INTOX "Intoxication"

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
#define _LANG_CMD_TIME_REAL  "Real world: "
#define _LANG_CMD_TIME_GAME  "In game:    "
#define _LANG_CMD_TIME_HELP  "Shows the date and time of the real world and of the game you are in."

// grope 

#define _LANG_CMD_GROPE_ALIAS ({ "grope" })
#define _LANG_CMD_GROPE_SYNTAX "grope"
#define _LANG_CMD_GROPE_HELP "To search for some exit when you are blind or in the dark.\n"
#define _LANG_CMD_GROPE_NO_DARK "Grope only works when it's dark or you are blind.\n"
#define _LANG_CMD_GROPE_LOCKOUT "You must wait a while before groping again.\n"
#define _LANG_CMD_GROPE_NOTHING "You grope around but you don't find anything.\n"
#define _LANG_CMD_GROPE_PLAYER_ME "You find "+contents[j]->query_cap_name()+"!\n"
#define _LANG_CMD_GROPE_PLAYER_THEM me->query_cap_name()+" bumps into you.\n"
#define _LANG_CMD_GROPE_PLAYER_ROOM me->query_cap_name()+" bumps into "+contents[j]->query_cap_name()+".\n"
#define _LANG_CMD_GROPE_NPC_ME "You bump into "+contents[j]->query_cap_name()+"!\n"
#define _LANG_CMD_GROPE_NPC_ROOM me->query_cap_name()+" bumps into "+contents[j]->query_cap_name()+".\n"
#define _LANG_CMD_GROPE_DOOR_ME "You grope around and find a door towards "+contents[j]->query_dest()+".\n"
#define _LANG_CMD_GROPE_DOOR_ROOM me->query_cap_name()+" bumps into a door towards "+contents[j]->query_dest()+".\n"
#define _LANG_CMD_GROPE_OTHER_ME "You bump into "+contents[j]->query_cap_name()+"!\n"
#define _LANG_CMD_GROPE_OTHER_ROOM me->query_cap_name()+" bumps into "+contents[j]->query_cap_name()+".\n"
#define _LANG_CMD_GROPE_MSG "You grope around and find a door towards " + dir + ".\n"

// map

#define _LANG_CMD_MAP_ALIAS ({ "map" })
#define _LANG_CMD_MAP_SYNTAX "map [compact | unicode | color | coords]"
#define _LANG_CMD_MAP_HELP "Shows a map of your surroundings."
#define _LANG_CMD_MAP_NO_ENV "Without an environment you cannot do that.\n"
#define _LANG_CMD_MAP_INVALID "For some you cannot use your map here.\n"
#define _LANG_CMD_MAP_NO_WATER "You cannot check the map while you are in the water.\n"
#define _LANG_CMD_MAP_INVISIBLE "You cannot read the map while completely invisible.\n"
#define _LANG_CMD_MAP_JUST_ARRIVED "You have just arrived and do not know your way around yet, wait a moment.\n"
#define _LANG_CMD_MAP_LEGEND "Legend"
#define _LANG_CMD_MAP_YOUR_POS "Your position"
#define _LANG_CMD_MAP_ENEMIES "Enemies"
#define _LANG_CMD_MAP_FRIENDS "Group members"
#define _LANG_CMD_MAP_GUARDS "Guards"
#define _LANG_CMD_MAP_DOORS "Doors"
#define _LANG_CMD_MAP_HOMES "Homes"
#define _LANG_CMD_MAP_UP_STAIRS "Stairs up"
#define _LANG_CMD_MAP_DOWN_STAIRS "Stairs down"
#define _LANG_CMD_MAP_NEW_QUESTS "New quests"
#define _LANG_CMD_MAP_FINISHED_QUESTS "Finished quests"
#define _LANG_CMD_MAP_COAST "Shore"

#define _LANG_CMD_MAP_OPT_COMPACT ({ "compact" })
#define _LANG_CMD_MAP_OPT_UNICODE ({ "unicode" })
#define _LANG_CMD_MAP_OPT_COLOR   ({ "color", "colour" })
#define _LANG_CMD_MAP_OPT_COORDS  ({ "coords" })

#define _LANG_CMD_MAP_VARIANTS_HELP \
        "\nVariants:\n" + \
        "  map           the standard map.\n" + \
        "  map compact   a denser one-character-per-location view.\n" + \
        "  map unicode   the same density as compact but with proper\n" + \
        "                box-drawing glyphs (needs a UTF-8 client).\n" + \
        "  map color     standard chunky boxes, each room tinted by\n" + \
        "                the area it belongs to.\n"
#define _LANG_CMD_MAP_VARIANTS_HELP_CODER \
        "  map coords    coordinates overlay (coder only).\n"

#define _LANG_CMD_MAP_UNKNOWN_VARIANT "Unknown map variant.\n"

// worldmap.c

#define _LANG_CMD_WORLDMAP_ALIAS   ({ "worldmap" })
#define _LANG_CMD_WORLDMAP_SYNTAX  "worldmap"
#define _LANG_CMD_WORLDMAP_HELP    "Show a bird's-eye map centered on the sector you are in. " + \
                                   "Each glyph represents one 10x10 sector; your position is marked '@'."
#define _LANG_CMD_WORLDMAP_HELP_CODER "Coders: you can pass a size in sectors, worldmap <width> [height] (3-80 wide, 3-40 tall)."
#define _LANG_CMD_WORLDMAP_USAGE   "Syntax: worldmap [width [height]]\n"
#define _LANG_CMD_WORLDMAP_RANGE   "Dimensions must be between 3 and 80 wide, 3 and 40 tall.\n"
#define _LANG_CMD_WORLDMAP_CODER   "You cannot change the map size.\n"
#define _LANG_CMD_WORLDMAP_NOMAP   "You are nowhere on any world map right now.\n"

// idle.c
#define _LANG_IDLE_ALIAS ({ "idle" })
#define _LANG_IDLE_SYNTAX "idle [<minutes>]"
#define _LANG_IDLE_HELP "How many minutes the mud keeps your character in the world when you\n" + \
                        "stop being active. The same value covers two situations:\n" + \
                        "  - If your connection stays up but you send no input for this long,\n" + \
                        "    you are logged out and your progress is saved.\n" + \
                        "  - If your connection drops, your character stays in the world for\n" + \
                        "    this many minutes so you can reconnect and pick up right where\n" + \
                        "    you left off. After that your character leaves the world and the\n" + \
                        "    next login starts you again from your last saved state.\n" + \
                        "Combat cannot be avoided by pulling the plug because your character\n" + \
                        "stays in the world during that time.\n" + \
                        "Running `idle` alone shows the current value and the allowed range.\n" + \
                        "Running `idle <minutes>` sets a new value; anything outside the range is clamped."
#define _LANG_IDLE_SHOW "Idle grace is %d minutes (allowed range %d – %d).\n"
#define _LANG_IDLE_SET "Idle grace set to %d minutes.\n"

// read. `str` is what the player named, and `ob` the thing being read
#define _LANG_CMD_READ_ALIAS ({ "read" })
#define _LANG_CMD_READ_SYNTAX "read <object>"
#define _LANG_CMD_READ_HELP "Read whatever is written on something: a sign, a plaque, " + \
                            "a book, a letter. Name the thing, and you read what it says.\n" + \
                            "\n" + \
                            "Every text is written in some language. If you do not know it, you see " + \
                            "the letters jumbled, and at least learn which language it is in; use " + \
                            "'speak' to see the ones you know.\n" + \
                            "\n" + \
                            "Reading is not looking: 'look <object>' tells you what the thing is like, " + \
                            "'read <object>' gives you what it says."
#define _LANG_CMD_READ_WHAT "Read what?\n"
#define _LANG_CMD_READ_CANNOT_FIND "You see no '" + str + "' here.\n"
#define _LANG_CMD_READ_NOTHING_WRITTEN ob->query_short() + " has nothing written on it.\n"

// group. `name` is the party's name, `who` the one being talked about, and
// `str` what the player typed
#define _LANG_CMD_GROUP_ALIAS ({ "party", "group" })
#define _LANG_CMD_GROUP_SYNTAX "party <option> [<name>]"
#define _LANG_CMD_GROUP_HELP "A party brings several players together for something they want to " + \
    "do as one. It has a channel of its own, you see who is with you on the map, and it lasts " + \
    "as long as you want it to: nothing about it is kept from one session to the next.\n" + \
    "\n" + \
    "Whoever creates it leads it, and is the only one who can invite, throw somebody out, " + \
    "rename it or hand it over. If they leave, the party passes to the next one in. " + \
    "Creating one asks for a few hours of play; anybody at all can be invited to one.\n" + \
    "\n" + \
    "  party create [<name>]    creates it, with you at the head.\n" + \
    "  party invite <player>    offers them a place; they have a short while to take it.\n" + \
    "  party join               takes the invitation you were given.\n" + \
    "  party list               who you are.\n" + \
    "  party status             how each of you is holding up.\n" + \
    "  party channel <message>  talks on the party channel.\n" + \
    "  party leave              you walk out.\n" + \
    "  party name <name>        renames it.\n" + \
    "  party kick <player>      puts them out.\n" + \
    "  party leader <player>    hands them the lead.\n" + \
    "  party end                breaks it up.\n" + \
    "\n" + \
    "The party channel also answers to 'adventurer <message>'. To be left out of parties " + \
    "altogether, see 'consent'."

#define _LANG_CMD_GROUP_CREATE ({ "create" })
#define _LANG_CMD_GROUP_END ({ "end", "disband" })
#define _LANG_CMD_GROUP_LEAVE ({ "leave" })
#define _LANG_CMD_GROUP_LIST ({ "list" })
#define _LANG_CMD_GROUP_STATUS ({ "status" })
#define _LANG_CMD_GROUP_NAME ({ "name" })
#define _LANG_CMD_GROUP_INVITE ({ "invite" })
#define _LANG_CMD_GROUP_JOIN ({ "join" })
#define _LANG_CMD_GROUP_KICK ({ "kick" })
#define _LANG_CMD_GROUP_LEADER ({ "leader" })
#define _LANG_CMD_GROUP_CHANNEL ({ "channel" })

#define _LANG_CMD_GROUP_DEFAULT_NAME me->query_cap_name() + "'s party"
#define _LANG_CMD_GROUP_UNKNOWN_OPTION "I do not know that one. See 'help party'.\n"
#define _LANG_CMD_GROUP_IN_PARTY "You are in the party '" + name + "'.\n"
#define _LANG_CMD_GROUP_NOT_IN_PARTY "You have to be in a party for that.\n"
#define _LANG_CMD_GROUP_ALREADY_IN_PARTY "You are in a party already.\n"
#define _LANG_CMD_GROUP_NOT_LEADER "Only whoever leads the party can do that.\n"
#define _LANG_CMD_GROUP_TOO_YOUNG "Your character is too young yet to lead a party.\n"
#define _LANG_CMD_GROUP_NAME_TOO_LONG "That name is too long.\n"
#define _LANG_CMD_GROUP_NEEDS_NAME "You have to give a name.\n"
#define _LANG_CMD_GROUP_NEEDS_PLAYER "You have to say who.\n"

#define _LANG_CMD_GROUP_CREATED_ME "You create the party '" + name + "'.\n"
#define _LANG_CMD_GROUP_CREATED_ROOM me->query_cap_name() + " creates the party '" + name + "'.\n"
#define _LANG_CMD_GROUP_ENDED_ME "You break up the party.\n"
#define _LANG_CMD_GROUP_ENDED_THEM me->query_cap_name() + " breaks up the party.\n"
#define _LANG_CMD_GROUP_LEFT_ME "You leave the party.\n"
#define _LANG_CMD_GROUP_LEFT_THEM me->query_cap_name() + " leaves the party.\n"
#define _LANG_CMD_GROUP_DISSOLVED "Left on your own, the group breaks up.\n"
#define _LANG_CMD_GROUP_LEADER_CANNOT_LEAVE "You lead the party: hand it over or break it up.\n"

#define _LANG_CMD_GROUP_LIST_HEADER "Party '" + name + "':\n"
#define _LANG_CMD_GROUP_LIST_LEADER " (leading)"
#define _LANG_CMD_GROUP_RENAMED_ME "The party is called '" + name + "' now.\n"
#define _LANG_CMD_GROUP_RENAMED_THEM me->query_cap_name() + " renames the party: '" + name + "'.\n"

#define _LANG_CMD_GROUP_NO_SUCH_PLAYER "Nobody by that name is connected.\n"
#define _LANG_CMD_GROUP_NOT_YOURSELF "Not yourself.\n"
#define _LANG_CMD_GROUP_THEY_HAVE_PARTY who->query_cap_name() + " is in a party already.\n"
#define _LANG_CMD_GROUP_THEY_REFUSE who->query_cap_name() + " does not want to be invited to parties.\n"
#define _LANG_CMD_GROUP_THEY_BLOCK who->query_cap_name() + " does not want to accept your invitations.\n"
#define _LANG_CMD_GROUP_FULL "The party is full.\n"
#define _LANG_CMD_GROUP_INVITED_ME "You invite " + who->query_cap_name() + " to your party.\n"
#define _LANG_CMD_GROUP_INVITED_THEM me->query_cap_name() + " invites you to the party '" + name + "'.\n" + \
    "Type 'party join' if you want in; the invitation does not stand for long.\n"

#define _LANG_CMD_GROUP_NOT_INVITED "Nobody has invited you to a party.\n"
#define _LANG_CMD_GROUP_INVITE_GONE "That party is gone.\n"
#define _LANG_CMD_GROUP_JOINED_ME "You join the party '" + name + "'.\n"
#define _LANG_CMD_GROUP_JOINED_THEM me->query_cap_name() + " joins the party.\n"

#define _LANG_CMD_GROUP_NOT_A_MEMBER "Nobody by that name is in your party.\n"
#define _LANG_CMD_GROUP_KICKED_ME "You throw " + who->query_cap_name() + " out of the party.\n"
#define _LANG_CMD_GROUP_KICKED_THEM me->query_cap_name() + " throws you out of the party.\n"
#define _LANG_CMD_GROUP_KICKED_OTHERS me->query_cap_name() + " throws " + who->query_cap_name() + " out of the party.\n"

#define _LANG_CMD_GROUP_ALREADY_LEADER "You lead the party already.\n"
#define _LANG_CMD_GROUP_HANDED_ME "You hand the party to " + who->query_cap_name() + ".\n"
#define _LANG_CMD_GROUP_HANDED_THEM me->query_cap_name() + " hands you the party.\n"
#define _LANG_CMD_GROUP_HANDED_OTHERS me->query_cap_name() + " hands the party to " + who->query_cap_name() + ".\n"

// description

#define _LANG_CMD_DESCRIPTION_ALIAS ({ "description" })
#define _LANG_CMD_DESCRIPTION_SYNTAX "description [<text> | write | clear]"
#define _LANG_CMD_DESCRIPTION_HELP "Write what others see when they look at you."
#define _LANG_CMD_DESCRIPTION_CLEAR ({ "clear" })
#define _LANG_CMD_DESCRIPTION_EDIT ({ "write" })
#define _LANG_CMD_DESCRIPTION_CURRENT "When somebody looks at you, they see:\n\n   " + \
  me->query_description() + "\n\n'description clear' goes back to the default one.\n"
#define _LANG_CMD_DESCRIPTION_DEFAULT "You have not written a description, so whoever looks at " + \
  "you sees the default one.\nType 'description <text>', or 'description write' to use " + \
  "your editor.\n"
#define _LANG_CMD_DESCRIPTION_SET "Now, when somebody looks at you, they see what you wrote.\n"
#define _LANG_CMD_DESCRIPTION_CLEARED "You have the default description again.\n"
#define _LANG_CMD_DESCRIPTION_UNCHANGED "Your description does not change.\n"
#define _LANG_CMD_DESCRIPTION_TOO_LONG "It is too long: " + me->query_max_description_length() + \
  " characters at most.\n"

// title

#define _LANG_CMD_TITLE_ALIASES ({ "title" })
#define _LANG_CMD_TITLE_SYNTAX "title\n" + \
                               "          title <number>\n" + \
                               "          title none"
#define _LANG_CMD_TITLE_HELP "Lists the titles you have earned and chooses which one you wear."
#define _LANG_CMD_TITLE_NO_TITLE ({ "none", "nothing" })
#define _LANG_CMD_TITLE_NONE_EARNED "You have not earned any title yet.\n"
#define _LANG_CMD_TITLE_HEADER "Your titles:\n"
#define _LANG_CMD_TITLE_LINE "  %2d) %-40s (%s)%s\n"
#define _LANG_CMD_TITLE_FROM_CLASS "class"
#define _LANG_CMD_TITLE_FROM_GUILD "guild"
#define _LANG_CMD_TITLE_FROM_JOB "job"
#define _LANG_CMD_TITLE_FROM_DEED "deed"
#define _LANG_CMD_TITLE_WORN "  [ worn ]"
#define _LANG_CMD_TITLE_FOOTER "Write 'title <number>' to wear it, " + \
                               "or 'title none' to wear none.\n"
#define _LANG_CMD_TITLE_CLEARED "You no longer wear any title.\n"
#define _LANG_CMD_TITLE_NOT_YOURS "You have no title with that number.\n"
#define _LANG_CMD_TITLE_SET "You now wear the title '" + me->query_title() + "'.\n"

// quests

#define _LANG_CMD_QUESTS_ALIAS ({ "quests", "quest" })
#define _LANG_CMD_QUESTS_SYNTAX "quests [done|info|accept|deliver|abandon] [<number>]"
#define _LANG_CMD_QUESTS_HELP "Shows what whoever is with you offers and the quests you " + \
      "have in hand, with a number for each.\n" + \
      "  quests                offers here and your quests, numbered\n" + \
      "  quests done           the ones you have handed in, by chain\n" + \
      "  quests info <n>       the full detail of number n\n" + \
      "  quests accept <n>     you take one of the ones offered\n" + \
      "  quests deliver <n>    you hand one of yours to whoever awaits it here\n" + \
      "  quests abandon <n>    you drop one of yours, losing what you had done\n" + \
      "The numbers are those of the listing at that moment, and they serve for " + \
      "everything. To accept or deliver, when only one fits, the number can be left out."

#define _LANG_CMD_QUESTS_TITLE "Quests"
#define _LANG_CMD_QUESTS_NONE "You have no quest in hand.\n"
#define _LANG_CMD_QUESTS_OFFERS_FROM name + " offers you:\n"
#define _LANG_CMD_QUESTS_OFFERS_HERE "Offered here:\n"
#define _LANG_CMD_QUESTS_YOURS "Your quests:\n"
#define _LANG_CMD_QUESTS_ENTRY "  [%^BOLD%^" + index + "%^RESET%^] " + quest->query_title()
#define _LANG_CMD_QUESTS_PROGRESS_ONE " (" + done + " of " + needed + ")"
#define _LANG_CMD_QUESTS_PROGRESS_MANY " (" + met + " of " + total + " objectives)"
#define _LANG_CMD_QUESTS_FINISHED " (%^BOLD%^RED%^finished%^RESET%^)"
#define _LANG_CMD_QUESTS_COMPLETE_HERE " [%^BOLD%^YELLOW%^?%^RESET%^] " + \
      "deliver it to " + name

#define _LANG_CMD_QUESTS_NEW_OFFERS name + \
      " has new quests for you.\n"

#define _LANG_CMD_QUESTS_COMPLETE_IN_PLACE " [%^BOLD%^YELLOW%^?%^RESET%^] " + \
      "you can deliver it here"
#define _LANG_CMD_QUESTS_COMPLETE_ALONE " [%^BOLD%^YELLOW%^?%^RESET%^] " + \
      "you can deliver it yourself, wherever you are"

#define _LANG_CMD_QUESTS_HINT_LEAD "Type "
#define _LANG_CMD_QUESTS_ANY_NUMBER "<number>"
#define _LANG_CMD_QUESTS_HINT_INFO "'quests info " + which + "' to see more of one"
#define _LANG_CMD_QUESTS_HINT_ACCEPT "'quests accept " + which + "' to take it"
#define _LANG_CMD_QUESTS_HINT_COMPLETE "'quests deliver " + which + "' to hand it in"
#define _LANG_CMD_QUESTS_HINT_ABANDON "'quests abandon " + which + "' to drop it"

#define _LANG_CMD_QUESTS_OBJECTIVE_LINE "- " + objectives[j][OBJ_TEXT] + " (" + \
      progress[j] + " of " + objectives[j][OBJ_COUNT] + ")\n"

#define _LANG_CMD_QUESTS_NO_SUCH "There is no quest with that number.\n"
#define _LANG_CMD_QUESTS_NOT_AN_OFFER "That is not one of the quests offered here.\n"
#define _LANG_CMD_QUESTS_NOT_YOURS "That is not one of your quests.\n"
#define _LANG_CMD_QUESTS_CANNOT_COMPLETE "That quest cannot be delivered here.\n"
#define _LANG_CMD_QUESTS_WHICH "Which one? Say it by its number.\n"

#define _LANG_CMD_QUESTS_CHAIN_INTRO "This quest is part of the chain:\n\n"
#define _LANG_CMD_QUESTS_CHAIN_STEPS_INTRO "This chain is made up of the quests:\n\n"
#define _LANG_CMD_QUESTS_CHAIN_TITLE "%^BOLD%^" + chain_title + "%^RESET%^\n"
#define _LANG_CMD_QUESTS_CHAIN_STEP "  " + (j + 1) + ". " + \
      step_quest->query_title() + mark + "\n"
#define _LANG_CMD_QUESTS_STEP_DONE " %^BOLD%^YELLOW%^✓%^RESET%^"

#define _LANG_CMD_QUESTS_DONE_HEADER "Quests finished"
#define _LANG_CMD_QUESTS_DONE_NONE "You have not handed in any quest yet.\n"
#define _LANG_CMD_QUESTS_DONE_ENTRY "  [%^BOLD%^" + index + "%^RESET%^] " + \
      quest->query_title() + \
      (done[id][QUEST_TIMES] > 1 ? " (" + done[id][QUEST_TIMES] + " times)" : "") + \
      ", " + ctime(done[id][QUEST_LAST], 4) + "\n"
#define _LANG_CMD_QUESTS_DONE_OPTIONS ({ "done", "finished" })

#define _LANG_CMD_QUESTS_INFO_OPTIONS ({ "info" })
#define _LANG_CMD_QUESTS_ACCEPT_OPTIONS ({ "accept" })
#define _LANG_CMD_QUESTS_COMPLETE_OPTIONS ({ "deliver" })
#define _LANG_CMD_QUESTS_ABANDON_OPTIONS ({ "abandon", "drop" })
