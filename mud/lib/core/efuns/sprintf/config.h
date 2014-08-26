# ifndef _SP_CONFIG_H_
# define _SP_CONFIG_H_

# include <limits.h>

# undef  __CLOSE_TO_C__      /* Define when putting output in first argument. */
# define __FLOATS__          /* Define if float conversions are wanted.       */
# undef  __TIME_CONVERSION__ /* Define if time conversions are wanted.        */
# undef  __TIME_ZONE__       /* Define for @Z conversion (timezone)           */

# define BASE  10         /* Base in which integers are presented by default. */

# define FLT_PRECISION 6  /* Default precision for floats.                    */

# endif
