
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "uptime" }));
  set_usage("uptime");
  set_help("Shows for how long the mud server has been functioning.");
}

static int cmd(string str, object me, string verb)
{
  int tim, num;
  string *bits;

  write("The mud has been up for:\n\t");

  tim = uptime();
  bits = ({ });

  if (tim > 60*60*24)
    bits += ({ (num=tim/(60*60*24)) + " " + "day" + (num == 1 ? "" : "s") });

  if (tim > 60*60 && tim%(60*60*24))
    bits += ({ (num=(tim/(60*60))%24) + " " + "hour" + (num == 1 ? "" : "s") });

  if (tim > 60 && tim%(60*60))
    bits += ({ (num=(tim/60)%60) + " " + "minute" + (num == 1 ? "" : "s") });

  if (tim%60)
    bits += ({ (num=tim%60) + " " + "second" + (num == 1 ? "" : "s") });

  if (sizeof(bits) > 1)
  {
    write(query_multiple_short(bits) + ".\n");
    // tell_object(me, implode(bits[0..sizeof(bits)-2], ", ") +
    //       " y " + bits[sizeof(bits)-1] + ".\n" +
    //       "Load average: " + query_load_average() + ".\n");
  }

  return 1;
}
