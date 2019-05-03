
inherit "/lib/room.c";

#include "path.h"
#include <mud/mail.h>

void setup() {
  set_short("Mail control room");
  set_long(
"This is the mail control room.  You can order mailing lists here if you're "+
"getting tired of extensive CC'ing.\n"+
"Available commands here are:\n"+
"create <list>              :  Creating a new mailing list.\n"+
"add <list> <names>         :  Adding a name to a mailing list.\n"+
"remove <list>              :  Removing a mailing list.\n"+
"list                       :  List all the mailing lists.\n");

  set_light(100);
  add_exit("sur",ADMIN+"admin2","standard");
} /* setup() */

void init() {
  ::init();
  add_action("do_add", "add");
  add_action("do_list", "list");
  add_action("do_remove", "remove");
  add_action("do_create", "create");
} /* init() */

int do_add(string str) {
  string name, list, *names;
  int i;

  notify_fail("Syntax: add <list> <name(s)>\n");
  if (!str) return 0;
  if (sscanf(str, "%s %s", list, name) != 2) return 0;
  if (!MAIL_TRACK->query_list(list)) {
    notify_fail("The mailing list "+list+" does not exist.\n");
    return 0;
  }
  if (!MAIL_TRACK->query_controller(list, this_player()->query_name())) {
    notify_fail("You have to be a member of a mailing list to get mail "+
                "from it.\n");
    return 0;
  }
  names = explode(replace(lower_case(name), " ", ","), ",") - ({ "" });
  if (!sizeof(names)) {
    notify_fail("It helps if you tell me who to remove.\n");
    return 0;
  }
  for (i=0;i<sizeof(names);i++)
    if (names[i][0] == '*') {
/* They are trying to add a controller. */
      names[i] = names[i][1..1000];
      if (!"/lib/core/login"->test_user(names[i]))
        write("the player "+names[i]+" does not exist.\n");
      else if (MAIL_TRACK->add_controller(list, names[i]))
        write("Added "+names[i]+" as a controller of "+list+".\n");
      else
        write("Failed to add "+names[i]+" as a controller of "+list+".\n");
    } else if (!"/lib/core/login"->test_user(names[i]) &&
               !MAIL_TRACK->query_list(names[i]))
      write("The player "+names[i]+" does not exist.\n");
    else if (MAIL_TRACK->add_member(list, names[i]))
      write("Added the player "+names[i]+" to the mailing list.\n");
    else
      write("Failed to add the player "+names[i]+" to the mailing list.\n");
  return 1;
} /* do_add() */

int do_list(string str) {
  string *strs, *cont;
  int i, j;

  if (!str) {
    strs = MAIL_TRACK->query_mailing_lists();
    printf("Current mailing lists: %-=*s", (int)this_user()->query_cols()-24,
                                           implode(strs, ", ")+".\n");
  } else {
    strs = MAIL_TRACK->query_members(str);
    cont = MAIL_TRACK->query_controllers(str);
    if (!strs) {
      notify_fail("The mailing list "+str+" does not exist.\n");
      return 0;
    }
    for (i=0;i<sizeof(cont);i++)
      if ((j = member_array(cont[i], strs)) != -1)
        strs[j] = "*"+strs[j];
      else
        strs += ({ "*"+cont[i] });
    printf(str+": %-=*s", (int)this_user()->query_cols()-strlen(str)-3,
                          implode(strs, ", ")+".\n");
  }
  return 1;
} /* do_list() */

int do_remove(string str) {
  string name, list, *names;
  int i;

  notify_fail("Syntax: remove <list>\n");
  if (!str) return 0;
  if (sscanf(str, "%s %s", list, name) != 2) return 0;
  if (!MAIL_TRACK->query_list(list)) {
    notify_fail("The mailing list "+list+" does not exist.\n");
    return 0;
  }
  if (!MAIL_TRACK->query_controller(list, this_player()->query_name())) {
    notify_fail("You have to be a controller of a mailing list to get mail "+
                "from it.\n");
    return 0;
  }
  names = explode(replace(lower_case(name), " ", ","), ",") - ({ "" });
  if (!sizeof(names)) {
    notify_fail("It helps if you tell me who to remove.\n");
    return 0;
  }
  for (i=0;i<sizeof(names);i++)
    if (names[i][0] == '*') {
/* They are trying to remove a controller. */
      names[i] = names[i][1..1000];
      if (MAIL_TRACK->remove_controller(list, names[i]))
        write("Removed "+names[i]+" as a controller of "+list+".\n");
      else
        write("Failed to remove "+names[i]+" as a controller of "+list+".\n");
    } else if (MAIL_TRACK->remove_member(list, names[i]))
      write("Removed the player "+names[i]+" from the mailing list.\n");
    else
      write("Could not remove "+names[i]+" from the mailing list.\n");
  return 1;
} /* do_remove() */

int do_create(string str) {
  string s1, s2;

  notify_fail("Syntax: create <name>\n");
  if (!str) return 0;
  if (MAIL_TRACK->query_list(str)) {
    notify_fail("The list "+str+" already exists.\n");
    return 0;
  }
  if (sscanf(str, "%s %s", s1, s2) == 2 ||
      sscanf(str, "%s,%s", s1, s2)) {
    notify_fail("The mailing list name cannot have spaces or commas in it.\n");
    return 0;
  }
  if (MAIL_TRACK->create_list(str, this_player()->query_name())) {
    write("Created the mailing list "+str+".\n");
    return 1;
  }
  notify_fail("Failed to create the mailing list.\n");
  return 0;
} /* do_create() */
