/* Hamlet, Mar 1996 */
/* Reworked, Aug, 1997.  Hamlet again. */
#include <standard.h>
#include <cmd.h>

#define MENU_OBJECT "/obj/menu"
#define MAN_HAND    "/obj/handlers/man"

inherit CMD_BASE;

void setup(){
  position = 1;
}

string query_usage() { return "man [-d] [-a] [-x] <topic>\n"; }

string query_short_help() {
	return "Muestra la página del manual del tema especificado.";
	/*
  return "man gives you the manual page for a given topic.\n"
         "If no argument is given, or the topic is a directory,\n"
         "the man menu system is entered.\n\n"
         "If you wish to enter the menu at the point of a document, "
         "use -d\nIf you wish to view all files that match a topic, "
         "use -a\nIf you wish to avoid seeing the cute 'help' "
         "messages at the bottom of each menu, use -x or add "
         "the property MENU_EXPERT to yourself.\nThis system now "
         "accepts full unix-style wildcards at any point,\nso "
         "something like 'man *string*' works.\n";
		*/
}
 
protected int cmd(string str, object me, string verb) {
  object man_obj;
  int match_all, enter_menu, menu_expert;
  int WAS_MATCH = 1;
  string *bits;

  if(!str)
    str = "";

  if(member_array(str, ({ "--rebuild", "--reindex", "--REBUILD", 
                          "--REINDEX" })) > -1) {
    MAN_HAND->rebuild();
    tell_object(me, "The index will be rebuilt in a few seconds.\n");
    return 1;
  }

  bits = explode(str, " ") - ({ "" });

  while(sizeof(bits) && WAS_MATCH) {
    switch(bits[0]) {
      case "-d" : enter_menu = 1;  break;
      case "-a" : match_all = 1;  break;
      case "-x" : menu_expert = 1;  break;
      default : WAS_MATCH = 0;
    }

    if(WAS_MATCH)
      bits = bits[1..];
  }

  str = implode(bits, " ");

  if(catch(man_obj = clone_object(MENU_OBJECT)) || !man_obj) {
    notify_fail("It appears that the manual page displayer is broken.\n");
    return 0;
  }

  man_obj->do_query(MAN_HAND, str, me, match_all, 
                    enter_menu, menu_expert); 
  
  return 1;
} /* cmd() */

