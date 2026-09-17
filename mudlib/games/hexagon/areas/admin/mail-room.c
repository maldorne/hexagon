// Mail control room: where mailing lists are created and kept.
// Rewritten over the mailing lists handler, neverbot 09/2026.

inherit "/lib/room.c";

#include "path.h"
#include <files/postal.h>

int do_create(string str);
int do_add(string str);
int do_remove(string str);
int do_delete(string str);
int do_list(string str);

void setup()
{
  set_short("Mail control room");
  set_long(
"This is the mail control room. Mailing lists are kept here: writing a letter " +
"to a list sends it to every member. Administrators create lists, and the " +
"controllers of a list keep its members. Every game has a list of its own, " +
"with its coordinator and members, which follows the game.\n" +
"Available commands here are:\n" +
"  create <list>              : create a new mailing list\n" +
"  add <list> <names>         : add members, *name adds a controller\n" +
"  remove <list> <names>      : remove members, *name only takes control away\n" +
"  delete <list>              : delete a mailing list\n" +
"  list [list]                : show every list, or the members of one\n");

  set_light(100);
  add_exit(DIR_SOUTH, ADMIN + "admin2", "standard");
}

void init()
{
  ::init();
  add_action("do_create", "create");
  add_action("do_add", "add");
  add_action("do_remove", "remove");
  add_action("do_delete", "delete");
  add_action("do_list", "list");
}

private string * split_names(string str)
{
  return explode(replace_string(lower_case(str), ",", " "), " ") - ({ "" });
}

int do_create(string str)
{
  if (!strlen(str) || sizeof(split_names(str)) != 1)
  {
    notify_fail("Syntax: create <list>\n");
    return 0;
  }

  str = lower_case(trim(str));

  if (!MAILING_LISTS_D->create_list(str))
  {
    notify_fail("Could not create the list: only administrators create " +
                "lists, and the name cannot be taken by a list, a game or a character.\n");
    return 0;
  }

  write("Mailing list " + str + " created.\n");
  return 1;
}

int do_add(string str)
{
  string list, * names;
  int i;

  if (!strlen(str) || sizeof(names = split_names(str)) < 2)
  {
    notify_fail("Syntax: add <list> <names>\n");
    return 0;
  }

  list = names[0];

  if (!MAILING_LISTS_D->query_list(list))
  {
    notify_fail("The mailing list " + list + " does not exist.\n");
    return 0;
  }

  for (i = 1; i < sizeof(names); i++)
    if (names[i][0] == '*')
    {
      if (MAILING_LISTS_D->add_controller(list, names[i][1..]))
        write("Added " + names[i][1..] + " as a controller of " + list + ".\n");
      else
        write("Could not add " + names[i][1..] + " as a controller of " + list + ".\n");
    }
    else if (MAILING_LISTS_D->add_member(list, names[i]))
      write("Added " + names[i] + " to " + list + ".\n");
    else
      write("Could not add " + names[i] + " to " + list + ".\n");

  return 1;
}

int do_remove(string str)
{
  string list, * names;
  int i;

  if (!strlen(str) || sizeof(names = split_names(str)) < 2)
  {
    notify_fail("Syntax: remove <list> <names>\n");
    return 0;
  }

  list = names[0];

  if (!MAILING_LISTS_D->query_list(list))
  {
    notify_fail("The mailing list " + list + " does not exist.\n");
    return 0;
  }

  for (i = 1; i < sizeof(names); i++)
    if (names[i][0] == '*')
    {
      if (MAILING_LISTS_D->remove_controller(list, names[i][1..]))
        write("Removed " + names[i][1..] + " as a controller of " + list + ".\n");
      else
        write("Could not remove " + names[i][1..] + " as a controller of " + list + ".\n");
    }
    else if (MAILING_LISTS_D->remove_member(list, names[i]))
      write("Removed " + names[i] + " from " + list + ".\n");
    else
      write("Could not remove " + names[i] + " from " + list + ".\n");

  return 1;
}

int do_delete(string str)
{
  if (!strlen(str))
  {
    notify_fail("Syntax: delete <list>\n");
    return 0;
  }

  str = lower_case(trim(str));

  if (!MAILING_LISTS_D->delete_list(str))
  {
    notify_fail("Could not delete " + str + ": it does not exist, it belongs " +
                "to a game, or you do not control it.\n");
    return 0;
  }

  write("Mailing list " + str + " deleted.\n");
  return 1;
}

int do_list(string str)
{
  string * lists, * members, * controllers;
  string text;
  int i;

  if (!strlen(str))
  {
    lists = MAILING_LISTS_D->query_lists();

    if (!sizeof(lists))
    {
      write("There are no mailing lists.\n");
      return 1;
    }

    text = "Mailing lists:\n";
    for (i = 0; i < sizeof(lists); i++)
      text += "  " + lists[i] +
              (MAILING_LISTS_D->query_game_list(lists[i]) ? " (game)" : "") + "\n";

    write(text);
    return 1;
  }

  str = lower_case(trim(str));

  if (!MAILING_LISTS_D->query_list(str))
  {
    notify_fail("The mailing list " + str + " does not exist.\n");
    return 0;
  }

  members = MAILING_LISTS_D->query_members(str);
  controllers = MAILING_LISTS_D->query_controllers(str);

  for (i = 0; i < sizeof(members); i++)
    if (member_array(members[i], controllers) != -1)
      members[i] = "*" + members[i];

  write(str + ": " + (sizeof(members) ? implode(members, ", ") : "nobody") + ".\n");
  return 1;
}
