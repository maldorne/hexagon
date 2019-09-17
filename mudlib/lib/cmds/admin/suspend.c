#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
 
void setup()
{
position = 1;
}
 
int cmd(string str, object me, string verb) {
  string name;
  int tim;
  if (this_player() != this_player(1))
    return 0;
  if (!str)
  { 
    notify_fail("Syntax: suspend <nombre> <tiempo>\nDonde el tiempo es en horas.\n");
    return 0;
  }
  if (sscanf(str, "%s %d", name, tim) != 2) {
    notify_fail("Syntax: suspend <nombre> <tiempo>\nDonde el tiempo es en horas.\n");
    return 0;
  }
  notify_fail("We failed!\n");
  if (!"/secure/bastards"->suspend_person(name, tim*60*60))
    return 0;
  write(name+" suspendido hasta "+ctime(time()+tim*60*60)+"\n");
  return 1;
}
