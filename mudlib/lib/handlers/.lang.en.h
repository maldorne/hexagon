
#define _LANG_CALENDAR_BIRTHDAY_STRING month(month-1)+" the "+ordinal(day)

// Composition of a year name. References `num`, `gender` and `year`
// (a mixed * with POS_YEAR_NAME / POS_YEAR_ADJECTIVE indices) from
// the caller, which is calendar.c::query_year_name(). English drops
// the gender distinction; the adjective goes before the noun.
#define _LANG_CALENDAR_YEAR_STRING \
  "" + num + ", the year of the " + \
  year[POS_YEAR_ADJECTIVE] + " " + year[POS_YEAR_NAME]

// weather

#define _LANG_WEATHER_MONTHS ({ "january", "february", "march", "april", \
                                "may", "june", "july", "august", \
                                "september", "october", "november", \
                                "december" })

#define _LANG_WEATHER_SEASONS ({ "winter", "spring", "summer", "autumn" })

#define _LANG_WEATHER_DAYNIGHT ({ "day", "night" })

#define _LANG_WEATHER_MOON_PHASES ({ \
  "full", "waning three quarters", "waning half", \
  "waning crescent", "waning crescent", "new", "waxing crescent", \
  "waxing crescent", "waxing half", "waxing three quarters" })

// Single moon name for the shared base handler. Per-game subclasses
// with multiple / coloured moons override moon_string() outright.
#define _LANG_WEATHER_MOON_NAME "moon"

// The weather controller object, as whoever examines it sees it
#define _LANG_WEATHER_NO_CLONING "This object cannot be cloned.\n"
#define _LANG_WEATHER_OB_NAME    "weather"
#define _LANG_WEATHER_OB_SHORT   "Extraordinary weather controller"
#define _LANG_WEATHER_OB_LONG    "This is the extraordinary weather controller!\n"
#define _LANG_WEATHER_OB_ALIAS   "controller"

// Full "The moon is <phase>" sentence. References `phase` from the caller.
// "The" already handles sentence-start capitalisation.
#define _LANG_WEATHER_MOON_STRING \
  "The %^BOLD%^" + _LANG_WEATHER_MOON_NAME + "%^RESET%^ is " + phase + ".\n"

#define _LANG_WEATHER_TEMP_SCORCHING  "scorching heat"
#define _LANG_WEATHER_TEMP_VERY_HOT   "much heat"
#define _LANG_WEATHER_TEMP_HOT        "a hot climate"
#define _LANG_WEATHER_TEMP_WARM       "a warm climate"
#define _LANG_WEATHER_TEMP_PLEASANT   "a pleasant temperature"
#define _LANG_WEATHER_TEMP_COOL       "a cool climate"
#define _LANG_WEATHER_TEMP_COLD       "cold"
#define _LANG_WEATHER_TEMP_VERY_COLD  "much cold"
#define _LANG_WEATHER_TEMP_POLAR      "polar cold"

#define _LANG_WEATHER_WIND_STRONG      "a strong wind"
#define _LANG_WEATHER_WIND_VERY_STRONG "a very strong wind"
#define _LANG_WEATHER_WIND_BLIZZARD    "a blizzard"
#define _LANG_WEATHER_WIND_HURRICANE   "a hurricane"

#define _LANG_WEATHER_RAIN_LIGHT   "light rain"
#define _LANG_WEATHER_RAIN_NORMAL  "rain"
#define _LANG_WEATHER_RAIN_STORM         ({ "a storm", "hail", "snow" })
#define _LANG_WEATHER_RAIN_STRONG_STORM  ({ "a strong storm", "heavy hail", \
                                            "a heavy snowfall" })
#define _LANG_WEATHER_RAIN_HEAVY_STORM   ({ "a very strong storm", "very heavy hail", \
                                            "a very dense snowfall" })

#define _LANG_WEATHER_STRING_OF   " of "
#define _LANG_WEATHER_STRING_WITH " with "
#define _LANG_WEATHER_STRING_SEP  ", "
#define _LANG_WEATHER_STRING_AND  " and "
#define _LANG_WEATHER_STRING_END  ".\n"

#define _LANG_WEATHER_DATE_STRING \
  "It is " + number_as_string(data[0]) + " o'clock" + \
  " on " + capitalize(handler("calendar", this_object())->query_week_day_string(0, this_object())) + \
  " " + (day_of_month + 1) + " of " + \
  capitalize(month_string()) + \
  ", year " + handler("calendar")->query_year_name(data[4])

// postal

#define _LANG_POSTAL_NEW_MAIL "A letter from " + capitalize(from) + \
  " has arrived for you: " + subject + ".\n"

// weapons

#define _LANG_WEAPONS_SLASH_MSG_ME "You cut"
#define _LANG_WEAPONS_SLASH_MSG_HIM " cuts you"
#define _LANG_WEAPONS_SLASH_MSG_ENV " cuts"
#define _LANG_WEAPONS_PIERCE_MSG_ME "You pierce"
#define _LANG_WEAPONS_PIERCE_MSG_HIM " pierces you"
#define _LANG_WEAPONS_PIERCE_MSG_ENV " pierces"
#define _LANG_WEAPONS_BLUNT_MSG_ME "You blunt"
#define _LANG_WEAPONS_BLUNT_MSG_HIM " blunts you"
#define _LANG_WEAPONS_BLUNT_MSG_ENV " blunts"
#define _LANG_WEAPONS_FIRE_MSG_ME "You burn"
#define _LANG_WEAPONS_FIRE_MSG_HIM " burns you"
#define _LANG_WEAPONS_FIRE_MSG_ENV " burns"
#define _LANG_WEAPONS_COLD_MSG_ME "You freeze"
#define _LANG_WEAPONS_COLD_MSG_HIM " freezes you"
#define _LANG_WEAPONS_COLD_MSG_ENV " freezes"

#define _LANG_WEAPONS_NO_EFFECT " without effect"
#define _LANG_WEAPONS_WEAKLY " weakly"
#define _LANG_WEAPONS_WITH_LITTLE_FORCE " with little force"
#define _LANG_WEAPONS_NORMAL ""
#define _LANG_WEAPONS_WITH_FORCE " with force"
#define _LANG_WEAPONS_WITH_MUCH_FORCE " with much force"
#define _LANG_WEAPONS_VIOLENTLY " violentely"
#define _LANG_WEAPONS_INCREDIBLE_FORCE " with an incredible force"
#define _LANG_WEAPONS_SUPERHUMAN_FORCE " with a superhuman force"

#define _LANG_WEAPONS_WHO_ME " "+defender->query_cap_name()
#define _LANG_WEAPONS_WHO_HIM ""
#define _LANG_WEAPONS_WHO_ENV " "+defender->query_cap_name()

#define _LANG_WEAPONS_WHERE_RELATIVE_ME " in "+defender->query_possessive()+" "+where->query_name()
#define _LANG_WEAPONS_WHERE_RELATIVE_HIM " in your "+where->query_name()
#define _LANG_WEAPONS_WHERE_RELATIVE_ENV " in "+defender->query_possessive()+" "+where->query_name()

#define _LANG_WEAPONS_WHERE_ME " "+defender->query_cap_name() + "'s " + where->query_name()
#define _LANG_WEAPONS_WHERE_HIM "r "+where->query_name()
#define _LANG_WEAPONS_WHERE_ENV " "+defender->query_cap_name() + "'s " + where->query_name()

#define _LANG_WEAPONS_LOC_ME " on "+localization
#define _LANG_WEAPONS_LOC_HIM " on "+localization
#define _LANG_WEAPONS_LOC_ENV " on "+localization

// #define _LANG_WEAPONS_NO_LOC_ME " "+defender->query_cap_name()
// #define _LANG_WEAPONS_NO_LOC_ENV " "+defender->query_cap_name()

#define _LANG_WEAPONS_SLASHING_MSG_1 ", making serious cuts"
#define _LANG_WEAPONS_SLASHING_MSG_2 ", provoking terrible bleeding"
#define _LANG_WEAPONS_SLASHING_MSG_3 ", making deep cuts"
#define _LANG_WEAPONS_SLASHING_MSG_4 ", leaving a bleeding wound"
#define _LANG_WEAPONS_SLASHING_MSG_5 ", leaving a light wound"
#define _LANG_WEAPONS_SLASHING_MSG_6 ", making only scratches"
#define _LANG_WEAPONS_SLASHING_MSG_7 ", leaving a mortal wound"
#define _LANG_WEAPONS_NON_SLASHING_MSG_1 ", leaving serious internal wounds"
#define _LANG_WEAPONS_NON_SLASHING_MSG_2 ", making a terrible wound"
#define _LANG_WEAPONS_NON_SLASHING_MSG_3 ", provoking terrible pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_4 ", provoking great pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_5 ", provoking serious pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_6 ", provoking light pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_7 ", provoking mortal wounds"

// shutdown.c

#define _LANG_SHUTDOWN_NAME "herald"
#define _LANG_SHUTDOWN_SHORT "The Herald of the Gods"
#define _LANG_SHUTDOWN_LONG "A celestial herald waiting for the Gods to tell him when a new Era will come.\n"
#define _LANG_SHUTDOWN_LONG2 "A celestial herald, of immense beauty, firmly focused on the countdown.\n"
#define _LANG_SHUTDOWN_EXTRA_LONG "The destiny of the world will be decided in "+(time_of_crash - time())+" seconds.\n"
#define _LANG_SHUTDOWN_IN_SECONDS "The destiny of the world will be decided in "+time_to_crash+" seconds."
#define _LANG_SHUTDOWN_IN_MINUTES "The destiny of the world will be decided in "+time_to_crash+" minutes."
#define _LANG_SHUTDOWN_IN_ONE_MINUTE "The destiny of the world will be decided in 1 minute."
#define _LANG_SHUTDOWN_WRONG_PARAM "Wrong argument.\n"
#define _LANG_SHUTDOWN_ALREADY "The shutdown was already scheduled, within "+(time_of_crash - time())+" seconds.\n"
#define _LANG_SHUTDOWN_ACCEPT "Shutdown scheduled, within "+(time_of_crash - time())+" seconds.\n"
#define _LANG_SHUTDOWN_MUD_CLOSING "The mud is closing!\n"
#define _LANG_SHUTDOWN_AUTO_REBOOT "The end of the world is coming...\nA new world will be " + \
    "built on its ashes...\n\n\t%^BOLD%^Within ten minutes%^RESET%^.\n"

// souls.c

#define _LANG_SOULS_HELP_HEADER "These are the available emotions:\n"+ \
    "   # means it accepts arguments.\n"+ \
    "   * means it can only be used with living beings.\n"+ \
    "   + means it can be used with living beings or independently.\n\n"+ \
    "Total number of emotions: "+m_sizeof(soul_data)+".\n\n"
#define _LANG_SOULS_CANNOT_DO "You cannot do that. Write \"help <emotion>\" "+ \
            "to see the available options.\n"
#define _LANG_SOULS_BLOCKING_YOU ((string)target->query_cap_name()) + " is blocking you.\n"
#define _LANG_SOULS_STATUS_NOT_CONNECTED "I'm sorry, but '"+liv+"' is not connected.\n"
#define _LANG_SOULS_STATUS_NOT_THAT_WAY "You cannot use that emotion command that way.\n"
#define _LANG_SOULS_STATUS_CANNOT_DO "You cannot do that.\n"
#define _LANG_SOULS_STATUS_CANNOT_FIND "I cannot find '"+livfail+"'.\n"
#define _LANG_SOULS_HELP_NO_PARAMETERS "The emotion command '"+str+"' has no optional parameters.\n"
#define _LANG_SOULS_HELP_PERSON "<person>"
#define _LANG_SOULS_HELP_PARAMETER "<parameter>"
#define _LANG_SOULS_HELP_SYNTAX "Syntax of the emotion command %^BOLD%^'"+str+"'%^RESET%^:\n"
#define _LANG_SOULS_HELP_NO_LIVINGS "Without applying it to living beings:\n"
#define _LANG_SOULS_HELP_LIVINGS "Applying it to living beings:\n"
#define _LANG_SOULS_HELP_ANY_TEXT "Any text can be used as a parameter.\n"
#define _LANG_SOULS_HELP_WITHOUT_PARAMETERS "It can be used without parameters.\n"
#define _LANG_SOULS_HELP_AVAILABLE_PARAMETERS "The available parameters are: "+implode(bit,", ")+".\n"
#define _LANG_SOULS_HELP_HAS_NO_PARAMETERS "It has no parameters.\n"

// channels
//
// A channel is stored and asked for by its id, an English word. The words a
// player types are here, and so is the name the channel is shown under.
// `name` in these messages is the display name of the channel.

#define _LANG_CHANNEL_VERBS ([ \
  "emergency"  : "emergency", \
  "guild"      : "guild", \
  "clan"       : "clan", \
  "race"       : "race", \
  "adventurer" : "adventurer", \
])

#define _LANG_CHANNEL_NAMES ([ ])

#define _LANG_CHANNEL_ANY "<channel>"

#define _LANG_CHANNEL_OPENED "Ok, you open the " + name + " channel.\n"
#define _LANG_CHANNEL_CLOSED "Ok, you close the " + name + " channel.\n"
#define _LANG_CHANNEL_ALREADY_ON "The '" + name + "' channel is already on!\n"
#define _LANG_CHANNEL_ALREADY_OFF "The '" + name + "' channel is already off!\n"
#define _LANG_CHANNEL_NOT_LISTENING "You are not listening to that channel.\n"
#define _LANG_CHANNEL_DISABLED "Your channel has been disabled.\n"
#define _LANG_CHANNEL_NO_PERMISSION "You are not allowed to use the '" + name + "' channel.\n"
#define _LANG_CHANNEL_NEEDS_MESSAGE "It works better if you give it a message.\n"

#define _LANG_CHANNEL_NO_GUILD "You are not a member of any guild.\n"
#define _LANG_CHANNEL_GUILD_NO_CHANNEL "Your guild does not have that ability.\n"
#define _LANG_CHANNEL_NO_CLAN "You are not a member of any clan.\n"
#define _LANG_CHANNEL_CLAN_NO_CHANNEL "Your clan does not have that ability.\n"
#define _LANG_CHANNEL_NO_RACE_GROUP "You are not a member of any racial group.\n"
#define _LANG_CHANNEL_RACE_NO_CHANNEL "Your racial group does not have that ability.\n"
#define _LANG_CHANNEL_NO_PARTY "You have to belong to a party to use this channel.\n"
#define _LANG_CHANNEL_PARTY_AUTOMATIC "You cannot open or close your party channel, it works by itself.\n"

#define _LANG_CHANNEL_HISTORY_HEADER "History of the '" + name + "' channel:\n"
#define _LANG_CHANNEL_HISTORY_EMPTY "The channel is empty.\n"
#define _LANG_CHANNEL_HISTORY_NOT_ALLOWED "Attempt to read the history of a channel you are not allowed to use.\n"
#define _LANG_CHANNEL_HELP_HEADER "Help for the '" + name + "' channel:\n"
#define _LANG_CHANNEL_WHO_HEADER "People listening to the '" + name + "' channel:"

#define _LANG_CHANNEL_HELP_MESSAGE "\t" + name + " <message> : Sends a message to everybody listening.\n"
#define _LANG_CHANNEL_HELP_HELP "\t" + name + " ?     : Shows this help message.\n"
#define _LANG_CHANNEL_HELP_HISTORY "\t" + name + " !     : Shows the last " + number_as_string(MAX_CHANNEL_HISTORY) + " messages sent to the channel.\n"
#define _LANG_CHANNEL_HELP_WHO "\t" + name + " .     : Lists who is listening to the channel.\n"

// titles

#define _LANG_TITLE_DEMO_FINISHED "Newly Arrived"
#define _LANG_TITLE_GRANTED "%^BOLD%^You have earned the title '" + text + "'.%^RESET%^\n"

// quests

#define _LANG_QUEST_ACCEPTED "You take the quest: " + quest->query_title() + ".\n"
#define _LANG_QUEST_ABANDONED "You give up the quest: " + quest->query_title() + ".\n"
#define _LANG_QUEST_COMPLETED "%^BOLD%^You have finished " + quest->query_title() + ".%^RESET%^\n"
#define _LANG_QUEST_OBJECTIVE_DONE "%^BOLD%^" + quest->query_title() + \
      ": " + objectives[j][OBJ_TEXT] + " (done).%^RESET%^\n"
#define _LANG_QUEST_OBJECTIVE_PROGRESS quest->query_title() + ": " + \
      objectives[j][OBJ_TEXT] + " (" + after + " of " + objectives[j][OBJ_COUNT] + ").\n"
