# ifndef _SP_TIME_H_
# define _SP_TIME_H_

# define WEEKDAYS ([ "Sun" : "Sunday",    "Mon" : "Monday", "Tue" : "Tuesday", \
                     "Wed" : "Wednesday", "Thu" : "Thursday", \
                     "Fri" : "Friday",    "Sat" : "Saterday" ])
# define WEEK   ({ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" })
# define MONTHS ([ "Jan" : "January", "Feb" : "February", "Mar" : "March", \
                   "Apr" : "April",   "May" : "May",      "Jun" : "June", \
                   "Jul" : "July",    "Aug" : "August",   "Sep" : "September", \
                   "Oct" : "October", "Nov" : "November", "Dec" : "December" ])
# define YEAR ({ "Jan", "Feb", "Mar", "Apr", "May", "Jun", \
                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" })
# define YEAR_IN_DAYS ({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })

# define FIRST_DST_CONVERSION_YEAR   1970  /* When did we get DST?? */
# define SECOND_DST_CONVERSION_YEAR  1996  /* New rules in 1996.    */
 
# define TZ   "MET"
# define DSTZ "METDST"                            

# define AM   "am"
# define PM   "pm"
# define NOON  12

# endif
