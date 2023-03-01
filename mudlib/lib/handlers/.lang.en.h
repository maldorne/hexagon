
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
