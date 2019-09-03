
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb)
{
  int tim, num;
  string *bits;

  write(_LANG_UPTIME_SINCE);

  tim = uptime();
  bits = ({ });

  if (tim > 60*60*24)
    bits += ({ (num=tim/(60*60*24)) + " " + _LANG_UPTIME_DAY + (num == 1 ? "" : "s") });

  if (tim > 60*60 && tim%(60*60*24))
    bits += ({ (num=(tim/(60*60))%24) + " " + _LANG_UPTIME_HOUR + (num == 1 ? "" : "s") });

  if (tim > 60 && tim%(60*60))
    bits += ({ (num=(tim/60)%60) + " " + _LANG_UPTIME_MINUTE + (num == 1 ? "" : "s") });

  if (tim%60)
    bits += ({ (num=tim%60) + " " + _LANG_UPTIME_SECOND + (num == 1 ? "" : "s") });

  if (sizeof(bits) > 1)
  {
    write(query_multiple_short(bits) + ".\n");
    // tell_object(me, implode(bits[0..sizeof(bits)-2], ", ") +
    //       " y " + bits[sizeof(bits)-1] + ".\n" +
    //       "Load average: " + query_load_average() + ".\n");
  }

  return 1;
}
