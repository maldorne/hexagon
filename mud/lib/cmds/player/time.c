#include <mud/cmd.h>

inherit CMD_BASE;

static int cmd(string str, object me, string verb)
{
   tell_object(me, ctime(time(), 5) + ".\n");

   // tell_object(me,ctime(time()-3600)+" Canarias.\n");
   // tell_object(me,ctime(time()-14400)+" America Central.\n");

   // mud time
   // tell_object(me, ctime(time(), 3)+".\n");

   return 1;
}
