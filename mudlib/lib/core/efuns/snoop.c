
#include <mud/secure.h>

// snoop - snoop an interactive user
// varargs object snoop( object snooper, object snoopee );

// When both arguments are used, begins snooping of `snoopee' by `snooper'.
// If the second argument is omitted, turns off all snooping by `snoopee'.
// Security for snoop() is normally controlled by a simul_efun.  snoop() returns
// `snoopee' if successful in the two-argument case, and `snooper' if it was
// successful in the single-argument case.  A return of 0 indicates failure.

static nomask int snoop(object sno, varargs object snop)
{
  // end snooping
  // if (!snop)
  // {

  // }

  // TODO snoop
  /*
  if (SECURE->valid_snoop(sno, snop, this_player(1)))
  {
    if (!snop)
      efun::snoop(sno);
    else
      efun::snoop(sno, snop);
    return 1;
  }
  */

  return 0;
}

// TODO query_snoop
// object query_snoop(object);
