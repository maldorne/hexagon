
#include <status.h>
#include <mud/time.h>
#include <mud/translations.h>

// uptime - return the number of seconds elapsed since the last driver reboot
// int uptime();
// This function returns the number of seconds since the last driver reboot.

static nomask int uptime()
{
  return status()[ST_UPTIME];
}

// localtime() converts a time value (as returned by time()) into an array
// of values which represents the time locally.  In the past time() was used
// to get the time in GMT (UTC), and then local definitions were used to
// determine the local offset from GMT.  This roundabout approach is no
// longer necessary.  localtime() returns the seconds, minutes and hours,
// the day, month and year, day of the week, day of the year,
// the name of the local timezone and how far the MUD is from GMT.  This
// information is retrieved directly from the operating system and made
// available to the driver without the use of MUD-specific configuration
// files.

// localtime() returns an array containing the values specified above.
// The index for each value is defined symbolically in localtime.h.  The
// following table summarizes the array returned by localtime().

//         int     LT_SEC          Seconds after the minute (0..59)
//         int     LT_MIN          Minutes after the hour (0..59)
//         int     LT_HOUR         Hour since midnight (0..23)
//         int     LT_MDAY         Day of the month (1..31)
//         int     LT_MON          Months since January (0..11)
//         int     LT_YEAR         Year (guarenteed to be >= 1900)
//         int     LT_WDAY         Days since Sunday (0..6)
//         int     LT_YDAY         Days since January 1 (0..365)
//         int     LT_GMTOFF       Seconds after GMT (UTC)
//         string  LT_ZONE         Timezone name

static mixed * localtime(int timestamp)
{
  mixed * result;

  result = allocate(10);

  result[LT_SEC]  = package("time")->second(timestamp);
  result[LT_MIN]  = package("time")->minute(timestamp);
  result[LT_HOUR] = package("time")->hour(timestamp);
  result[LT_MDAY] = package("time")->day(timestamp);
  result[LT_MON]  = package("time")->month(timestamp, 2);
  result[LT_YEAR] = package("time")->year(timestamp);
  result[LT_WDAY] = package("time")->weekday(timestamp, 2);
  result[LT_YDAY] = package("time")->day_of_year(timestamp);
  result[LT_GMTOFF] = 0;
  result[LT_ZONE] = package("time")->timezone(timestamp);;

  return result;
}

/*
 * Different modes:
 *  - flag = -1: just call the kfun
 *  - no flag or flag = 0: reduced datetime for logging:
 *    "Lun 21 Abr 2003 15:58:0"
 *  - flag = 1: generic real-world time (i.e. for the who command):
 *    "Lunes 21 de Abril de 2003"
 *  - flag = 2: date with 6 characters: ddmmaa
 *  - flag = 3: mud datetime (from weather.c)
 *
 *  Added to make the 'ls -la' shorter, neverbot 23/11/03
 *  - flag = 4: dd/mm/yy hh:mm:ss
 *
 *  Added real-world datetime for the "time" command
 *    neverbot 09/12/04
 *  - flag = 5:  "Lunes 21 de Abril de 2003 - hh:mm:ss"
 *
 *  Added for the sake of ftpd compatibility, neverbot 10/12/04
 *  - flag = 6:  "Fri Dec 10 hh:mm:ss aaaa"
 */
static string ctime(int time, varargs int flag)
{
  if (flag && (flag == -1))
    return ::ctime(time);

  return handler("calendar")->ctime(time, flag);
}

// taken from calendar handler, neverbot 03/2023
string seconds_to_time_string(int seconds)
{
  string * values;
  values = ({ });

  if (seconds > 86400)
    values += ({ sprintf("%d " + _LANG_TIME_DAY + 
                         ((seconds/86400 == 1) ? "" : "s"),
                           seconds/86400) });

  seconds = seconds % 86400;
  if (seconds > 3600)
    values += ({ sprintf("%d " + _LANG_TIME_HOUR + 
                         ((seconds/3600 == 1) ? "" : "s"),
                           seconds/3600) });

  seconds = seconds % 3600;
  if (seconds > 60)
    values += ({ sprintf("%d " + _LANG_TIME_MINUTE + 
                         (((seconds/60)%60 == 1) ? "" : "s"),
                           (seconds/60)%60) });

  seconds = seconds % 60;
  values += ({ sprintf("%d " + _LANG_TIME_SECOND + 
                       ((seconds%60 == 1) ? "" : "s"),
                           seconds%60) });

  return query_multiple_short(values);
}

/*
 * We'll try to do a simul_efun to wrap the ctime and make it
 * always return information in spanish, with different modes
 * for real-world time or mud time, neverbot 4/2003
 *
 * Added date and time inside the mud, Folken 6/03
 *  (takes info from the weather handler, ignoring the time
 *   passed as parameter)
 *
 * Added english/international formatted date, for ftpd compatibility,
 *   neverbot 12/04
 *
 * Added format with only the time, neverbot 1/06
 */

string day(int num)
{
  switch(num)
  {
    case 0: return _LANG_TIME_SUNDAY;
    case 1: return _LANG_TIME_MONDAY;
    case 2: return _LANG_TIME_TUESDAY;
    case 3: return _LANG_TIME_WEDNESDAY;
    case 4: return _LANG_TIME_THURSDAY;
    case 5: return _LANG_TIME_FRIDAY;
    default:
    case 6: return _LANG_TIME_SATURDAY;
  }
}

string intl_day(int num)
{
  switch(num)
  {
   case 0: return _LANG_TIME_INTL_SUNDAY;
   case 1: return _LANG_TIME_INTL_MONDAY;
   case 2: return _LANG_TIME_INTL_TUESDAY;
   case 3: return _LANG_TIME_INTL_WEDNESDAY;
   case 4: return _LANG_TIME_INTL_THURSDAY;
   case 5: return _LANG_TIME_INTL_FRIDAY;
   default:
   case 6: return _LANG_TIME_INTL_SATURDAY;
  }
}

string month(int num)
{
 switch(num)
 {
  case 0:  return _LANG_TIME_JANUARY;
  case 1:  return _LANG_TIME_FEBRUARY;
  case 2:  return _LANG_TIME_MARCH;
  case 3:  return _LANG_TIME_APRIL;
  case 4:  return _LANG_TIME_MAY;
  case 5:  return _LANG_TIME_JUNE;
  case 6:  return _LANG_TIME_JULY;
  case 7:  return _LANG_TIME_AUGUST;
  case 8:  return _LANG_TIME_SEPTEMBER;
  case 9:  return _LANG_TIME_OCTOBER;
  case 10: return _LANG_TIME_NOVEMBER;
  default:
  case 11: return _LANG_TIME_DECEMBER;
  }
}

string intl_month(int num)
{
 switch(num)
 {
  case 0:  return _LANG_TIME_INTL_JANUARY;
  case 1:  return _LANG_TIME_INTL_FEBRUARY;
  case 2:  return _LANG_TIME_INTL_MARCH;
  case 3:  return _LANG_TIME_INTL_APRIL;
  case 4:  return _LANG_TIME_INTL_MAY;
  case 5:  return _LANG_TIME_INTL_JUNE;
  case 6:  return _LANG_TIME_INTL_JULY;
  case 7:  return _LANG_TIME_INTL_AUGUST;
  case 8:  return _LANG_TIME_INTL_SEPTEMBER;
  case 9:  return _LANG_TIME_INTL_OCTOBER;
  case 10: return _LANG_TIME_INTL_NOVEMBER;
  default:
  case 11: return _LANG_TIME_INTL_DECEMBER;
  }
}
