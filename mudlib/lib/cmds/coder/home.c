/*
 * To take you home, or to other people's home. -- Wahooka May '96
 */

#include <mud/cmd.h>

inherit CMD_BASE;

void setup() {
  position = 1;
}

static int cmd(string str,object me, string verb) 
{
  if (!str || str == "") 
  {
    me->move_living("X","/home/" + me->query_name()+
      "/workroom.c");
    return 1;
  }
  else if (file_size("/home/" + me->expand_nickname(str)+"/workroom.c") > 0) 
  {
    me->move_living("X","/home/" + me->expand_nickname(str)+"/workroom.c");
    return 1;
  }
  else if (file_size("/home/" + me->expand_nickname(str)+"/workroom.c") == -1 && 
    file_size("/home/" + me->expand_nickname(str)) == -2) 
  {
    notify_fail("That person hasn't got a workroom!\n");
    return 0;
  }
  else if (file_size("/games/" + me->expand_nickname(str)+"/common.c") > 0) 
  {
    me->move_living("X","/games/" + me->expand_nickname(str)+"/common");
    return 1;
  }
  else 
  {
    notify_fail(capitalize(me->expand_nickname(str))+
      " doesn't exist, sorry.\n");
    return 0;
  }
}

string query_short_help() {
  return "Syntax: home\n" +
         "        home <person>\n\n" +
         "Moves you to <person>'s workroom. If nothing is specified, takes you to your own.\n";
}
