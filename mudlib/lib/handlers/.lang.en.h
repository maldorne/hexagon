
// calendar

#define _LANG_CALENDAR_DAY "day"
#define _LANG_CALENDAR_HOUR "hour"
#define _LANG_CALENDAR_MINUTE "minute"
#define _LANG_CALENDAR_SECOND "second"

#define _LANG_CALENDAR_SUNDAY "Sunday"
#define _LANG_CALENDAR_MONDAY "Monday"
#define _LANG_CALENDAR_TUESDAY "Tuesday"
#define _LANG_CALENDAR_WEDNESDAY "Wednesday"
#define _LANG_CALENDAR_THURSDAY "Thursday"
#define _LANG_CALENDAR_FRIDAY "Friday"
#define _LANG_CALENDAR_SATURDAY "Saturday"

#define _LANG_CALENDAR_JANUARY "January"
#define _LANG_CALENDAR_FEBRUARY "February"
#define _LANG_CALENDAR_MARCH "March"
#define _LANG_CALENDAR_APRIL "April"
#define _LANG_CALENDAR_MAY "May"
#define _LANG_CALENDAR_JUNE "June"
#define _LANG_CALENDAR_JULY "July"
#define _LANG_CALENDAR_AUGUST "August"
#define _LANG_CALENDAR_SEPTEMBER "September"
#define _LANG_CALENDAR_OCTOBER "October"
#define _LANG_CALENDAR_NOVEMBER "November"
#define _LANG_CALENDAR_DECEMBER "December"

// for ftp and others, always in english
#define _LANG_CALENDAR_INTL_SUNDAY "Sunday"
#define _LANG_CALENDAR_INTL_MONDAY "Monday"
#define _LANG_CALENDAR_INTL_TUESDAY "Tuesday"
#define _LANG_CALENDAR_INTL_WEDNESDAY "Wednesday"
#define _LANG_CALENDAR_INTL_THURSDAY "Thursday"
#define _LANG_CALENDAR_INTL_FRIDAY "Friday"
#define _LANG_CALENDAR_INTL_SATURDAY "Saturday"

#define _LANG_CALENDAR_INTL_JANUARY "January"
#define _LANG_CALENDAR_INTL_FEBRUARY "February"
#define _LANG_CALENDAR_INTL_MARCH "March"
#define _LANG_CALENDAR_INTL_APRIL "April"
#define _LANG_CALENDAR_INTL_MAY "May"
#define _LANG_CALENDAR_INTL_JUNE "June"
#define _LANG_CALENDAR_INTL_JULY "July"
#define _LANG_CALENDAR_INTL_AUGUST "August"
#define _LANG_CALENDAR_INTL_SEPTEMBER "September"
#define _LANG_CALENDAR_INTL_OCTOBER "October"
#define _LANG_CALENDAR_INTL_NOVEMBER "November"
#define _LANG_CALENDAR_INTL_DECEMBER "December"

#define _LANG_CALENDAR_BIRTHDAY_STRING month(month-1)+" the "+ordinal(day)
#define _LANG_CALENDAR_LONG_DATETIME day(info[LT_WDAY])+", "+month(info[LT_MON])+" the "+ordinal(info[LT_MDAY])+ \
            " "+info[LT_YEAR]

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

#define _LANG_WEAPONS_WHERE_RELATIVE_ME " "+defender->query_cap_name()+" in "+defender->query_possessive()+" "+where->query_name()
#define _LANG_WEAPONS_WHERE_RELATIVE_HIM " in your "+where->query_name()
#define _LANG_WEAPONS_WHERE_RELATIVE_ENV " "+defender->query_cap_name()+" in "+defender->query_possessive()+" "+where->query_name()

#define _LANG_WEAPONS_WHERE_ME " "+defender->query_cap_name() + "'s " + where->query_name()
#define _LANG_WEAPONS_WHERE_HIM "r "+where->query_name()
#define _LANG_WEAPONS_WHERE_ENV " "+defender->query_cap_name() + "'s " + where->query_name()

#define _LANG_WEAPONS_LOC_ME " "+defender->query_cap_name() + " on "+localization
#define _LANG_WEAPONS_LOC_HIM " on "+localization
#define _LANG_WEAPONS_LOC_ENV " "+defender->query_cap_name() + " on "+localization

#define _LANG_WEAPONS_NO_LOC_ME " "+defender->query_cap_name()
#define _LANG_WEAPONS_NO_LOC_ENV " "+defender->query_cap_name()

#define _LANG_WEAPONS_SLASHING_MSG_1 " making serious cuts"
#define _LANG_WEAPONS_SLASHING_MSG_2 " provoking terrible bleeding"
#define _LANG_WEAPONS_SLASHING_MSG_3 " making deep cuts"
#define _LANG_WEAPONS_SLASHING_MSG_4 " leaving a bleeding wound"
#define _LANG_WEAPONS_SLASHING_MSG_5 " leaving a light wound"
#define _LANG_WEAPONS_SLASHING_MSG_6 " making only scratches"
#define _LANG_WEAPONS_SLASHING_MSG_7 " leaving a mortal wound"
#define _LANG_WEAPONS_NON_SLASHING_MSG_1 " leaving serious internal wounds"
#define _LANG_WEAPONS_NON_SLASHING_MSG_2 " making a terrible wound"
#define _LANG_WEAPONS_NON_SLASHING_MSG_3 " provoking terrible pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_4 " provoking great pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_5 " provoking serious pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_6 " provoking light pain"
#define _LANG_WEAPONS_NON_SLASHING_MSG_7 " provoking mortal wounds"
