/*
 *  Copyright (C) 1994, 1995 Haijo Schipper (abigail@mars.ic.iaf.nl)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _SP_TIME_H_
#define _SP_TIME_H_

#define WEEKDAYS ([ "Sun" : "Sunday",    "Mon" : "Monday", "Tue" : "Tuesday", \
                    "Wed" : "Wednesday", "Thu" : "Thursday", \
                    "Fri" : "Friday",    "Sat" : "Saterday" ])
#define WEEK   ({ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" })
#define MONTHS ([ "Jan" : "January", "Feb" : "February", "Mar" : "March", \
                  "Apr" : "April",   "May" : "May",      "Jun" : "June", \
                  "Jul" : "July",    "Aug" : "August",   "Sep" : "September", \
                  "Oct" : "October", "Nov" : "November", "Dec" : "December" ])
#define YEAR ({ "Jan", "Feb", "Mar", "Apr", "May", "Jun", \
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" })
#define YEAR_IN_DAYS ({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })

#define FIRST_DST_CONVERSION_YEAR   1970  /* When did we get DST?? */
#define SECOND_DST_CONVERSION_YEAR  1996  /* New rules in 1996.    */
 
#define TZ   "MET"
#define DSTZ "METDST"

#define AM   "am"
#define PM   "pm"
#define NOON  12

#endif

// # include "config.h"
#define  __TIME_ZONE__ 

mixed weekday (int time, int arg) {
  return (arg == 2
              ? index (ctime (time, -1) [0 .. 2], WEEK)
              : arg == 1 ? WEEKDAYS [ctime (time, -1) [0 .. 2]]
                         : ctime (time, -1) [0 .. 2]);
}

mixed month (int time, int arg) {
  return (arg == 2
              ? index (ctime (time, -1) [4 .. 6], YEAR)
              : arg == 1 ? MONTHS [ctime (time, -1) [4 .. 6]]
                         : ctime (time, -1) [4 .. 6]);
}

int day (int time) {
  int d;
  sscanf (ctime (time, -1), "%*s %*s %d %*s %*d", d);
  return (d);
}

string ttime (int time) {
  string t;
  sscanf (ctime (time, -1), "%*s %*s %*d %s %*d", t);
  return (t);
}

string date (int time) {
  return (ctime (time, -1) [0 .. 10] + ctime (time, -1) [20 .. 23]);
}

int year (int time) {
  int y;
  sscanf (ctime (time, -1), "%*s %*s %*d %*s %d", y);
  return (y);
}

int hour (int time) {
  int h;
  sscanf (ttime (time), "%d:%*d:%*d", h);
  return (h);
}

int minute (int time) {
  int m;
  sscanf (ttime (time), "%*d:%d:%*d", m);
  return (m);
}

int second (int time) {
  int s;
  sscanf (ttime (time), "%*d:%*d:%d", s);
  return (s);
}

# ifdef __TIME_ZONE__
/* European rules. DST from 2.00 last Sunday of March until 2.00 last    */
/*                 Sunday of September.                                  */
/* Note, this will change of 1996. DST will then run until October.      */
/* timezone () will take the new rules in account...                     */
/* The scedule will not hold in the UK and Ireland.                      */
/* I haven't tested all possibilities... Mail me if you find any bugs.   */
/* It looks complicated, but really, it isn't. It's just one expression! */
string timezone (int time) {
  int mm, dd, ww;
  return (year (time) < FIRST_DST_CONVERSION_YEAR ||
             (mm = month (time, 2) + 1) < 3 ||
              mm > (year (time) < SECOND_DST_CONVERSION_YEAR ? 9 : 10)
             ? TZ                                   /* Jan Feb Oct Nov Dec */
             : mm != 3 && mm != (year (time) < SECOND_DST_CONVERSION_YEAR
                                             ? 9 : 10)
                  ? DSTZ                            /* Apr - Aug | Sep */
                  : mm == 3 
                       ? (dd = day (time)) < 25     /* Mar */
                            ? TZ                    /* First weeks */
                            : (ww = weekday (time, 2))
                                ? dd - ww <= 25     /* Not a Sunday */
                                    ? TZ            /* Before last Sunday */
                                    : DSTZ          /* After... */
                                : hour (time) <= 2
                                    ? TZ            /* Before 02.00 */
                                    : DSTZ          /* After... */
                  : mm == 9
                       ? dd < 24                    /* Sep */
                            ? DSTZ                  /* First weeks */
                            : (ww = weekday (time, 2))
                                ? dd - ww <= 24     /* Not a Sunday */
                                    ? DSTZ          /* Before last Sunday */
                                    : TZ            /* After ... */
                                : hour (time) <= 1  /* Last Sunday */
                                    ? DSTZ          /* Before 2.00 */
                                    : hour (time) == 2
                                        ? ctime (time (), -1) ==
                                          ctime (time () + 3600, -1)
                                            ? DSTZ  /* First time between */
                                                    /* 2.00 and 3.00 */
                                            : TZ    /* Second time... */
                                        : TZ        /* After 3.00 */

                       : dd < 25                    /* Oct */
                            ? DSTZ                  /* First weeks */
                            : (ww = weekday (time, 2))
                                ? dd - ww <= 25     /* Not a Sunday */
                                    ? DSTZ          /* Before last Sunday */
                                    : TZ            /* After ... */
                                : hour (time) <= 1  /* Last Sunday */
                                    ? DSTZ          /* Before 2.00 */
                                    : hour (time) == 2
                                        ? ctime (time (), -1) ==
                                          ctime (time () + 3600, -1)
                                            ? DSTZ  /* First time between */
                                                    /* 2.00 and 3.00 */
                                            : TZ    /* Second time... */
                                        : TZ);      /* After 3.00 */
}
# endif

int leap_year (int year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int day_of_year (int time) {
  int mm, dd, i, doy;
  for (i = 0, mm = month (time, 2); i < mm; doy += YEAR_IN_DAYS [i ++]);
  return (doy + (dd = day (time)) +
          (leap_year (year (time)) && (mm > 1 || mm == 1 && dd == 29) ? 1 : 0));
}


int week_number (int time, int monday) {
  int doy, wn;
  wn = (doy = day_of_year (time) - 1) / 7;
  if (weekday (time, 2) <= doy % 7) {wn ++;}
  if (monday && (weekday (time, 2) == 0)) {wn --;}
  return (wn);
}
