
#include <status.h>
#include <mud/time.h>

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

  result[LT_SEC]  = handler("time")->second(timestamp);
  result[LT_MIN]  = handler("time")->minute(timestamp);
  result[LT_HOUR] = handler("time")->hour(timestamp);
  result[LT_MDAY] = handler("time")->day(timestamp);
  result[LT_MON]  = handler("time")->month(timestamp, 2);
  result[LT_YEAR] = handler("time")->year(timestamp);
  result[LT_WDAY] = handler("time")->weekday(timestamp, 2);
  result[LT_YDAY] = handler("time")->day_of_year(timestamp);
  result[LT_GMTOFF] = 0;
  result[LT_ZONE] = handler("time")->timezone(timestamp);;

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
