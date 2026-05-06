# ifndef _SP_MACROS_H_
# define _SP_MACROS_H_

# include "config.h"
# include <status.h>


/* Special chars. */
# define CONV_CHAR ([ '%' : 1, '@' : 1 ])
# define DIGIT     ([ '0' : 1, '1' : 1, '2' : 1, '3' : 1, '4' : 1, \
                      '5' : 1, '6' : 1, '7' : 1, '8' : 1, '9' : 1 ])
# define FLAGS     ([ '-' : 1, '_' : 1, '|' : 1, '+' : 1, ' ' : 1, '#' : 1, \
                      '0' : 1, '<' : 1, '=' : 1, '>' : 1, '`' : 1, '~' : 1, \
                      '&' : 1])

/* Error checks. */
# ifdef  __CLOSE_TO_C__
# define TYPECHECK(type, arg, n) \
            if (!(type##p (arg))) {error ("sprintf: bad argument " + (n + 1));}
# else              
# define TYPECHECK(type, arg, n) \
                  if (!(type##p (arg))) {error ("sprintf: bad argument " + n);}
# endif              
# define ARGCOUNTCHECK(n, m) \
                        if ((n) == (m)) {error ("sprintf: too few arguments");}
# define ENDCHECK(c, e) \
      if ((c) == (e)) {error ("sprintf: missing format conversion character");}

/* My favourites. */
# define Case    break; case
# define Default break; default

# define MIN(x, y) ((x) < (y) ? (x) : (y))

# define intp(arg)     (typeof (arg) == T_INT)
# define floatp(arg)   (typeof (arg) == T_FLOAT)
# define stringp(arg)  (typeof (arg) == T_STRING)
# define objectp(arg)  (typeof (arg) == T_OBJECT)
# define arrayp(arg)   (typeof (arg) == T_ARRAY)
# define mappingp(arg) (typeof (arg) == T_MAPPING)
# ifdef  __FLOATS__
# define nump(arg)     (intp (arg) || floatp (arg))
# else
# define nump(arg)     (intp (arg))
# endif

# define NOTE

# endif
