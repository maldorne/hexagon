#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
 
void setup()
{
position = 1;
}
 
int cmd(string str, object me, string verb) {
  string name;
  string reason;
  if (this_player() != this_player(1))
    return 0;
  if (!str)
  { 
    notify_fail("Sintasis: ban <nombre> <razon>\n");
    return 0;
  }
  if (sscanf(str, "%s %s", name, reason) != 2) {
    notify_fail("Sintasis: ban <nombre> <razon>\n");
    return 0;
  }
  notify_fail("Fallamos!\n");
  if (!"/secure/bastards"->banish_playername(name, reason))
    return 0;
  write(name+" Baneado por: "+reason+"\n");
  return 1;
}
