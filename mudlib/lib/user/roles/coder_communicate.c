
// #include "inet.h"
// #include "udp.h"
#include <user/roles.h>

// nosave string my_file_name;

void create()
{
}

static void role_commands()
{
  add_action("do_echo",      "echo");
  add_action("do_echo_to",   "echoto");
  add_action("do_channels",  "coders");

  // add_action("do_channels", "dwcre");
  // add_action("do_channels", "intercre");
  // add_action("do_channels", "sport");
  // add_action("do_channels", "geek");

  // Radix was here...
  // add_action("do_channels", "semi");
  // add_action("do_channels", "thane");
  // add_action("do_channels", "dwadmin");
  // if (this_object()->query_thane())
  //   add_action("do_channels", "thane");

} /* role_commands() */

int do_channels(string mess)
{
  return this_player()->do_channels(mess);
}

int do_echo(string str)
{
  object me, * listeners;
  int i;

  me = this_player();

  if (!strlen(str))
  {
    notify_fail("Syntax: echo <text>\n");
    return 0;
  }

  if (!environment(me))
    return 0;

  log_file("echoes", me->query_cap_name()+" echo's: "+
      str+" ["+ctime(time(), 4)+"]\n");

  str += "%^RESET%^";

  write("You echo to " + file_name(environment(me)) + ":\n" + str + "\n");

  // the event carries who echoed, and event() would name this role object
  // instead, so everybody here is told one by one
  listeners = all_inventory(environment(me));
  for (i = 0; i < sizeof(listeners); i++)
    listeners[i]->event_player_echo(me, str + "\n");

  return 1;
} /* do_echo() */

int do_echo_to(string str)
{
  string who, what;
  object me, ob;

  me = this_player();

  // Radix cause Piper & Taniwha wanted it...
  if (this_user()->query_object_type() == O_CODER)
  {
    notify_fail("echoto is not available to coders.\n");
    return(0);
  }

  if (!strlen(str))
  {
    notify_fail("Syntax: echoto <player> <text>\n");
    return 0;
  }

  if (sscanf(str, "%s %s", who, what) != 2)
  {
    notify_fail("Syntax: echoto <player> <text>\n");
    return 0;
  }

  who = lower_case(who);
  who = (string)me->expand_nickname(who);
  ob = find_player(who);

  if (!ob)
  {
    notify_fail("Cannot find '"+who+"'.\n");
    return 0;
  }

  log_file("echoes", me->query_cap_name()+
    " echoto's "+who+": " +what+" ["+ctime(time(),4)+"]\n");

  what += "%^RESET%^";
  write("You echo to " + who + ":\n" + what + "\n");

  // told directly, so the event carries who echoed and not this role object
  ob->event_player_echo_to(me, what + "\n");

  return 1;
} /* do_echo_to() */

/*
int do_inter_creator_tell(string str)
{
  int emoted;

  if (!str)
  str = "";
  else
  if (str[0] != ' ')
  {
    emoted = sscanf(str, "@ %s", str);
    if (!emoted) {
      notify_fail("Flag invalido para "+query_verb()+".\n");
      return 0;
    }
  }
  else if (sscanf(str, " @ %s", str))
  emoted = 1;
  else
  sscanf(str, " %s", str);
  if (!this_object()->query_coder())
  return 0;
  if (!str) {
    notify_fail("Sintaxis: "+query_verb()+" <mensaje>\n");
    return 0;
  }
  if (this_object()->check_earmuffs("inter-creator-tell")) {
    notify_fail("No puedes usar esto si no estas en el canal.\n");
    return 0;
  }
  INTERCREATORD->do_intercreator(str, emoted);
  // (UDP_PATH+"gwizmsg")->send_gwizmsg(str, emoted);
  write("El mensaje puede tardar un poco en llegar a algunos sitios.\n");
  return 1;
} */

mixed * stats()
{
  return ({
          });
}
