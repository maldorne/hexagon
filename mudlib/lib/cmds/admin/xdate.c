/* Hamlet, Feb 1996 */
#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

void setup(){
  position = 1;
}

string query_usage() { return "xdate <player>\n"
                       "       xdate @<ipname|ipnumber>"; }

string query_short_help() {
	return "Obtiene la fecha actual de la máquina destino.";
	/*
 return "xdate asks the target machine what time it is.\n"
        "As an argument, it can take either a player's name, or ip "
        "addresses in either alphabetic or numeric formats.\n\n"
        "Examples:\n"
        "  xdate hamlet\n"
        "  xdate @mamaclaus.opup.org\n"
        "  xdate @128.169.92.86";
	*/
 }
 
protected int cmd(string str, object me, string verb) {
  object xdate_cmd;
  string name;
  string data;
  object per;
  string extrajunk = "";
  
  notify_fail("Must specify user or host.\n");
  
  if(!str)
    return 0;
  
  if(str[0..0] != "@") {  /* It may be a player's name. */
    name = me->expand_nickname(str);
    if (per = find_player(lower_case(name))) {
      data = query_ip_number(per);
      extrajunk = query_ip_name(per);

      if(data == extrajunk)
        extrajunk = name;
      else
        extrajunk += " - "+name;
    }
  }

  if(!data) { /* Nope, this was  @something ... not a player. */
    if(str[0..0] == "@")
      data = str[1..strlen(str)-1];
    else
      data = str;
  }
  
  if (xdate_cmd = clone_object("/secure/xdate_cmd")) {
    if(xdate_cmd->do_xdate(data, me, extrajunk))
      write("Ok.. Please wait..\n");
    else
      write("xdate: The name you passed must not have been valid.\n");
    return 1;
  } 
  else {
    notify_fail("Oops... xdate_cmd didn't want to clone.\n");
    return 0;
  }
} /* cmd() */
