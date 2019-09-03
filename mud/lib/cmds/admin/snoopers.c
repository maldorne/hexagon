#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
void setup()
{
position = 2;
}

int check_snoop(object ob) {
  if((object)ob->query_snoopee())
    return 1;
  else
    return 0;
} /* check_snoop() */
 
int check_qsnoop(object ob) {
  if(query_snooping(ob))
    return 1;
  else
    return 0;
} /* check_snoop() */
 
protected int cmd(string str, object me, string verb) {
  object *obs, *qobs;
int i;
      notify_fail("No one is being snooped by anyone.\n");
  qobs = filter_array(users(), "check_qsnoop", this_object());
  obs = filter_array(users(), "check_snoop", this_object());
  if (!"/secure/master"->high_programmer(geteuid(me)))
    if (!sizeof(obs)) {
      notify_fail("No one is being snooped by anyone.\n");
      return 0;
    } else {
      for (i=0;i<sizeof(obs);i++)
        write(obs[i]->query_cap_name()+" is snooping "+
             obs[i]->query_snoopee()->query_cap_name()+".\n");
      return 1;
    }
  if (!sizeof(obs) && !sizeof(qobs)) {
    notify_fail("No one is being snooped by anyone.\n");
    return 0;
  } else {
    for (i=0;i<sizeof(obs);i++)
      write(obs[i]->query_cap_name()+" is snooping "+
            obs[i]->query_snoopee()->query_cap_name()+".\n");
    for (i=0;i<sizeof(qobs);i++) {
      if (member_array(qobs[i],obs) == -1)
        write(qobs[i]->query_cap_name()+" is qsnooping "+
              query_snooping(qobs[i])->query_cap_name()+".\n");
    }
    return 1;
  }   
} /* do_snoopers() */
