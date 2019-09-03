/* Hamlet, Jun 1997 - unix-style whois command.
                      Get information from one of the whois servers.
*/
#include <standard.h>
#include <cmd.h>

#define WHOISD "/net/whoisd"

inherit CMD_BASE;

void setup() {
  position = 1;
}

string query_usage() { 
  return "whois [-s <whois.server>] <data>\n"
         "       whois --servers\n"; 
}

string query_short_help() {
	return "Implementa el comando whois estándar de unix.";
	/*
 return "whois implements the standard unix 'whois' command.\n"
        "As an argument, anything that's in a standard whois database\n"
        "may be supplied (username, real name, domain name, etc).\n"
        "The keyword for the servers (as returned by 'whois --servers')\n"
        "may be used as a server name.\n\n"
        "Examples:\n"
        "  whois charles, isaac\n"
        "  whois potato.org\n"
        "  whois -s nic.ddn.mil army.mil\n";
	*/
 }
 
protected int cmd(string str, object me, string verb) {
  string server;
  string *strbits;
  int SKIP_PROC;
  mapping servers;

  notify_fail("Must specify data to send.\n");
  
  if(!str)
    return 0;

  if(catch(load_object(WHOISD))) {
    notify_fail("Whoisd will not load.  Have someone fix it.\n");
    return 0;
  }

  str = implode(explode(str," ") - ({ "" }), " "); /* strip extra spaces */

  if( (str[0] == '"') && (str[<1] == '"') ) {
    SKIP_PROC = 1;
    if(strlen(str) > 2)
      str = str[1..<2];
    else
      str = "";
  }

  if( str[0..5] == "--serv" ) { /* They just want a server list. */
    int i;
    string resp = "";

    servers = WHOISD->get_servers();
    strbits = keys(servers);

    for(i=0; i<sizeof(strbits); i++)
      resp += sprintf("   %-10s:   %s\n", strbits[i], servers[strbits[i]]);

    tell_object(me, resp);
    return 1;
  }
 
  if( !SKIP_PROC && (str[0..1] == "-s") ) {  
    /* They're specifying a whois server */
    strbits = explode(str[2..], " ") - ({ "" });

    if( !sizeof(strbits) )
      return 0;
    else if( sizeof(strbits) == 1 ) {
      server = strbits[0];
      str = "";
    }
    else {
      server = strbits[0];
      str = implode(strbits[1..], " ");
    }

    servers = WHOISD->get_servers();

    if(servers[server])
      server = servers[server];
  }

  WHOISD->do_whois(me, str, server);

  return 1;
} /* cmd() */
