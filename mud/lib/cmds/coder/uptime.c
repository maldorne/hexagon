
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  int tim, num;
  string *bits;

  tell_object(me, "En funcionamiento desde hace:\n\t");

  tim = uptime();
  bits = ({ });

  if (tim > 60*60*24)
    bits += ({ (num=tim/(60*60*24)) + " día" + (num==1?"":"s") });
  
  if (tim > 60*60 && tim%(60*60*24))
    bits += ({ (num=(tim/(60*60))%24) + " hora" + (num==1?"":"s") });
  
  if (tim > 60 && tim%(60*60))
    bits += ({ (num=(tim/60)%60) + " minuto" + (num==1?"":"s") });
  
  if (tim%60)
    bits += ({ (num=tim%60) + " segundo" + (num==1?"":"s") });
  
  if (sizeof(bits) > 1)
    tell_object(me, implode(bits[0..sizeof(bits)-2], ", ") + 
          " y " + bits[sizeof(bits)-1] + 
          ".\nCarga Media (load_average): " + query_load_average() + ".\n");
  
  return 1;
}
