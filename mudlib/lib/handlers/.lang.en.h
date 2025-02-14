
#define _LANG_CALENDAR_BIRTHDAY_STRING month(month-1)+" the "+ordinal(day)

// postal

#define _LANG_POSTAL_GROUP_CODERS "coders"
#define _LANG_POSTAL_GROUP_ADMINS "admins"
#define _LANG_POSTAL_GROUP_PATRONS "patrons"
#define _LANG_POSTAL_DOES_NOT_EXIST_USER_OR_GROUP "The user or group does not exist: "+capitalize(who[i])+".\n"
#define _LANG_POSTAL_DOES_NOT_EXIST_USER "The user does not exist: "+capitalize(g[i])+".\n"
#define _LANG_POSTAL_NEW_MAIL "  New mail has arrived from $N\n  Subject: $S"
#define _LANG_POSTAL_OLD_MAIL_1 "Old mail #"+low+"\n"
#define _LANG_POSTAL_OLD_MAIL_2 "Old mails #"+low+" and "+"#"+high+"\n"
#define _LANG_POSTAL_OLD_MAIL_3 "Old mails from #"+low+" to "+"#"+high+"\n"

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
