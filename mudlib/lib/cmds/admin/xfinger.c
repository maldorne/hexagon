/* Hamlet, Feb 1996 - unix-style finger.
   Hamlet, Jun 1997 - reworked, simplified code.
*/
#include <standard.h>
#include <cmd.h>

#define XFINGERD "/net/xfingerd"

inherit CMD_BASE;

void setup() {
  position = 1;
}

string query_usage() { return "xfinger [-m] [-l] <player>\n"
                       "       xfinger [-l] [<user>]@<ipname|ipnumber>"; }

string query_short_help() {
	return "Devuelve el finger estándar de unix de un jugador, local o remoto.";
	/*
 return "xfinger implements the standard unix 'finger' command (with "
        "frills removed).\n\n"
        "As an argument, it can take either a player's name, or ip "
        "addresses in either alphabetic or numeric formats.\n\n"
        "Options:\n"
        "  -m   finger the whole machine instead of the specific userid "
        "when using a\n"
        "       playername.\n"
        "  -l   get the long finger information rather than the 1-liner.\n\n"
        "Examples:\n"
        "  xfinger hamlet\n"
        "  xfinger -l -m hamlet\n"
        "  xfinger @mamaclaus.opup.org\n"
        "  xfinger icharles@128.169.92.86";
		*/
 }
 
protected int cmd(string str, object me, string verb) {
  string name, user, hostip, hostname;
  string *strbits;
  object per;
  int MACHINE = 0, WIDE = 0;
  int WASMATCH = 1;
  
  notify_fail("Must specify user or host.\n");
  
  if(!str)
    return 0;

  if(catch(load_object(XFINGERD))) {
    notify_fail("Xfingerd will not load.  Have someone fix it.\n");
    return 0;
  }

  str = implode(explode(str," ") - ({ "" }), " "); /* strip extra spaces */

  while(WASMATCH) {
    switch(str[0..2]) {
      case "-m " :  MACHINE = 1;
                    break;
      case "-w " : 
      case "-l " :  WIDE = 1;
                    break;
      default    :  WASMATCH = 0;
    }

    if(WASMATCH) {
      if(strlen(str) < 4) /* Switch(es) without target. */
        return 0;
      /* else */
      str = str[3..<1];
    }
  }    
  
  if( strsrch(str, "@") == -1 ) {  /* It's a player's name. */
    name = me->expand_nickname(str);

    per = find_player(lower_case(name));

    if( !per ) {
      notify_fail("Player not found.\n");
      return 0;
    }

    if( !MACHINE && master()->query_administrator(geteuid(me)) ) 
      user = per->query_ident();

    hostip = query_ip_number(per);
    hostname = query_ip_name(per);
  }
  else { /* a user@host or an @host... */
    strbits = explode(str, "@");

    if( !sizeof(strbits) )
      hostip = query_host_name();
    else if( sizeof(strbits) == 1 ) {
      if( str[0] == '@' )
        hostip = strbits[0];
      else {
        hostip = query_host_name();
        user = strbits[0];
      }
    }
    else {
      hostip = strbits[<1];
      user = implode(strbits[0..<2], "@");
    }
  }

  XFINGERD->do_xfinger(hostip, user, me, name, hostname, WIDE);

  return 1;
} /* cmd() */
