#include <cmd.h>
inherit CMD_BASE;

void setup()
{
   position = 1;
}

protected int cmd(string str, object me, string verb) {
   string ret, env;
   object ob;
   ret = "";
   foreach(ob in users())
   {
      if(!ob || !environment(ob))
         continue;
      if(ob->query_object_type() == "X")
         continue;
      env = file_name(environment(ob));
      if(explode(env,"/")[1] != str)
         continue;
      ret += sprintf("%-15s%45-s\n", ob->query_cap_name(), env);
   }
   me->more_string(ret);
  return 1;
}

string short_help()
{
   return "Returns players that are in a specific domain and their "
      "current environment.\n";
}
