
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "shutdown" }));
  set_usage("shutdown <time>");
  set_help("Starts a process to shut down the mud.\n" + 
          "Use with no arguments to see how much time is left.\n" + 
          "Use time in minutes to specify when the mud will be shut down.\n" +
          "Use 'shutdown off' to cancel a shutdown in progress.\n");
}

static int cmd(string arg, object me, string verb) 
{
  int tim,upt;
  object ob;
  string ret;

  ret = "";

  if (!strlen(arg)) 
  {
    ob = handler("shutdown");
    if (!ob || !ob->shutdown_activated())
      write("No shutdown in progress.\nSyntax: " + query_usage() + "\n");
    else
    {
      if (ob->query_time_to_crash()/60 < 1)
        write("Less than a minute left to shutdown.\n"+
            "Use 'shutdown off' to cancel it.\n");
      else        
        write("Still "+ob->query_time_to_crash()/60+" minutes left to shutdown.\n"+
            "Use 'shutdown off' to cancel it.\n");
    }
    return 1;
  }
  
  if (!SECURE_OB->high_programmer( geteuid(me) ) )
  {
    upt = uptime();
    if ((upt/(60*60)%24) < 3) 
    {
      write("It has not been 3 hours yet since mud started. Perhaps you should wait awhile.\n");
      return 1;
    }
  }
  
  sscanf(arg, "%d %s", tim, arg);
  if (tim < 10 && !SECURE_OB->high_programmer(geteuid(me)))
    tim = 10;

  /* 
  if (str[0..2] == "now" && SECURE_OB->high_programmer(geteuid(me)))
    tim = 0;
  */
  /* Dejamos la comprobacion del tiempo para el /obj/shut.c, neverbot
  if (!tim)
    handler("shutdown")->end_it_all();
  else
  */

  // cancel shutdown added by neverbot, 23/4/2003
  if ((arg == "off") && SECURE_OB->high_programmer(geteuid(me)))
  {
    ob = handler("shutdown");

    if (!ob || !ob->shutdown_activated())
      write("No shutdown in progress.\n");
    else
      write("Shutdown cancelled.\n");

    handler("shutdown")->dest_me();
    return 1;
  }

  if (arg != "off")
  {
    ret += "["+ctime(time())+"] Shutdown started by: ";
    ret += (string)me->query_cap_name();
    ret += "\n  ("+arg+" minutes)\n";
    ret += "  (Memory usage: "+memory_status()+")\n";

    log_file("game_log", ret);
  }
  
  // message already given from the handler
  // write("Ok... shutting down the mud.\n");

  handler("shutdown")->shut(tim);
  return 1;
} 
 
