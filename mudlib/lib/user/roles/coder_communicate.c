
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
  add_action("do_emote_all", "emoteall");
  add_action("do_channels",  "coders");

  // add_action("do_channels", "dwcre");
  // add_action("do_channels", "intercre");
  // add_action("do_channels", "sport");
  // add_action("do_channels", "geek");

  // Radix was here...
  if (this_object()->query_admin())
  {
    add_action("do_god_inform", "ginfo");

    // add_action("do_channels", "semi");
    // add_action("do_channels", "thane");
    // add_action("do_channels", "dwadmin");
  }
  // if (this_object()->query_thane())
  //   add_action("do_channels", "thane");

} /* role_commands() */

int do_channels(string mess)
{
  return this_player()->do_channels(mess);
}

int do_god_inform(string mess)
{
  if (!strlen(mess))
  {
    notify_fail("Sintaxis: ginfo <mensaje>\n");
    return(0);
  }

  event(users(), "god_inform", (string)this_player()->query_cap_name() +
                                " [Info]:", mess + "\n");

  return 1;
} /* do god inform */

int do_echo(string str)
{
  if (!strlen(str))
  {
    notify_fail("Sintaxis : echo <texto>\n");
    return 0;
  }

  if (!environment(this_player()))
    return 0;

  log_file("echoes", this_player()->query_cap_name()+" echo's: "+
      str+" ["+ctime(time(), 4)+"]\n");

  str += "%^RESET%^";

  write("Envías a "+file_name(environment(this_player()))+" el echo:\n" + str + "\n");
  event(environment(this_player()), "player_echo", str + "\n");

  return 1;
} /* do_echo() */

int do_echo_to(string str)
{
  string who, what;
  object ob;

  // Radix cause Piper & Taniwha wanted it...
  if (this_player(1)->query_object_type() == O_CODER)
  {
    notify_fail("Echoto no esta disponible para programadores.\n");
    return(0);
  }

  if (!strlen(str))
  {
    notify_fail("Sintaxis: echoto <jugador> <texto>\n");
    return 0;
  }

  if (sscanf(str, "%s %s", who, what) != 2)
  {
    notify_fail("Sintaxis: echoto <jugador> <texto>\n");
    return 0;
  }

  who = lower_case(who);
  who = (string)this_player()->expand_nickname(who);
  ob = find_player(who);

  if (!ob)
  {
    notify_fail("No se ha podido encontrar a '"+who+"'.\n");
    return 0;
  }

  log_file("echoes", this_player()->query_cap_name()+
    " echoto's "+who+": " +what+" ["+ctime(time(),4)+"]\n");

  what += "%^RESET%^";
  write("Haces echo a " + who + ":\n" + what + "\n");
  event(ob, "player_echo_to", what + "\n");

  return 1;
} /* do_echo_to() */

int do_emote_all(string str)
{
  if (!strlen(str))
  {
    notify_fail("Sintaxis: emoteall <texto>\n");
    return 0;
  }

  // Radix cause Piper & Taniwha wanted it...
  if (this_player(1)->query_object_type() == O_CODER)
  {
    notify_fail("Emoteall sólo esta disponible para administradores.\n");
    return(0);
  }

  log_file("echoes", this_player()->query_cap_name()+
    " emotealls: "+str+" ["+ctime(time(), 4)+"]\n");

  str += "%^RESET%^";
  write("Envías el emoteall:\n" + this_player()->query_cap_name() + " " + str + "\n");
  event(users(), "player_emote_all", this_player()->query_cap_name() + " " + str + "\n");

  return 1;
} /* do_emote_all() */

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
