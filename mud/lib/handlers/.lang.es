
// calendar

#define _LANG_CALENDAR_DAY "día"
#define _LANG_CALENDAR_HOUR "hora"
#define _LANG_CALENDAR_MINUTE "minuto"
#define _LANG_CALENDAR_SECOND "segundo"

#define _LANG_CALENDAR_SUNDAY "Domingo"
#define _LANG_CALENDAR_MONDAY "Lunes"
#define _LANG_CALENDAR_TUESDAY "Martes"
#define _LANG_CALENDAR_WEDNESDAY "Miércoles"
#define _LANG_CALENDAR_THURSDAY "Jueves"
#define _LANG_CALENDAR_FRIDAY "Viernes"
#define _LANG_CALENDAR_SATURDAY "Sábado"

#define _LANG_CALENDAR_JANUARY "Enero"
#define _LANG_CALENDAR_FEBRUARY "Febrero"
#define _LANG_CALENDAR_MARCH "Marzo"
#define _LANG_CALENDAR_APRIL "Abril"
#define _LANG_CALENDAR_MAY "Mayo"
#define _LANG_CALENDAR_JUNE "Junio"
#define _LANG_CALENDAR_JULY "Julio"
#define _LANG_CALENDAR_AUGUST "Agosto"
#define _LANG_CALENDAR_SEPTEMBER "Septiembre"
#define _LANG_CALENDAR_OCTOBER "Octubre"
#define _LANG_CALENDAR_NOVEMBER "Noviembre"
#define _LANG_CALENDAR_DECEMBER "Diciembre"

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

#define _LANG_CALENDAR_BIRTHDAY_STRING ordinal(day)+" de "+month(month-1)
#define _LANG_CALENDAR_LONG_DATETIME day(info[LT_WDAY])+" "+ordinal(info[LT_MDAY])+" de "+ \
            month(info[LT_MON])+" de "+info[LT_YEAR]
