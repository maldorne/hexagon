#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
position = 0;
}

string query_usage()
{
return "command";
}


protected int cmd(string str, object me, string verb) {
  object *mems;
  string g_ob, mems2;
  int cols, i;
  mems2 = "";
  cols = (int)me->query_cols();
  g_ob = (string)me->query_guild_ob();
  mems = filter_array(users(),"get_members",this_object(),g_ob);
  for (i=0;i<sizeof(mems);i++)
    mems2 += mems[i]->query_cap_name()+"\n";
  write(sprintf("\n%*#-s\n",cols,mems2));
  return 1;
 }

int get_members(object ob, string gob) {
  return ( (gob == (string)ob->query_guild_ob()) &&
           (!ob->query_invis()) );
} /* get_members() */

