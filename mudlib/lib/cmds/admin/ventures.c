
#include <mud/cmd.h>
#include <room/ventures.h>
#include <room/location.h>
#include <user/terminal.h>

inherit LOCATION_CMD;

private int do_list();
private int do_here(object loc);
private int do_condition(object loc, string * args);
private object venture_of(object loc);
private string condition_path(string name, object loc);

void setup()
{
  set_aliases(({ "ventures" }));
  set_usage("ventures\n" +
            "ventures here\n" +
            "ventures condition add <name> [key=value ...]\n" +
            "ventures condition remove <name>");
  set_help(
    "Ventures (shops, pubs, etc) of the whole system and of the location you " +
    "are standing in.\n" +
    "\n" +
    "  ventures                                   list every venture stored\n" +
    "  ventures here                              the open conditions of this shop or tavern\n" +
    "  ventures condition add <name> [key=value ...]   add an open condition\n" +
    "  ventures condition remove <name>           take one off\n" +
    "\n" +
    "A venture is open when all its conditions hold; with none it is always " +
    "open. The name of a condition is a file under the game's ventures/conditions " +
    "directory, or under /lib/ventures/conditions (a trailing .c is allowed), or " +
    "an absolute path. The arguments are key=value pairs; a value made of " +
    "digits is a number.");
}

static int cmd(string str, object me, string verb)
{
  string * args;
  object loc;

  args = words(str);
  if (!sizeof(args))
    return do_list();

  if ((args[0] != "here" || sizeof(args) != 1) && args[0] != "condition")
  {
    notify_fail("Usage:\n" + query_usage() + "\n");
    return 0;
  }

  loc = location_of(me);
  if (!loc)
  {
    notify_fail("Stand in a shop or a tavern.\n");
    return 0;
  }

  return args[0] == "here" ? do_here(loc) : do_condition(loc, args);
}

/**
 * ventures: every shop and pub the handler knows.
 */
private int do_list()
{
  string * ventures;
  string ret;
  int i, j;
  object terminal, where;

  terminal = load_object(TERM_HANDLER);

  ventures = ({ });
  ventures += handler("ventures", this_player())->query_shops_array();
  ventures += handler("ventures", this_player())->query_pubs_array();

  if (!ventures || !sizeof(ventures))
  {
    write("No ventures stored in the handler.\n");
    return 1;
  }

  ret = "Ventures stored:\n\n";

  for (i = 0; i < sizeof(ventures); i++)
  {
    where = load_object(ventures[i]);

    // loaded first, it is a room
    if (where)
    {
      ret += sprintf(" - %-25s", extract(terminal->clean_string(where->short()), 0, 20));
      ret += sprintf(" %-6s %-10s", (where->query_shop() ? "shop" : "pub"), "room");
      ret += sprintf("%30s.c\n", ventures[i]);

    }
    // check if it is a location
    else if (where = load_object(LOCATION_HANDLER)->load_location(ventures[i]))
    {
      ret += sprintf(" - %-25s", extract(terminal->clean_string(where->short()), 0, 20));
      ret += sprintf(" %-6s %-10s", (where->has_component(LOCATION_COMPONENT_SHOP) ? "shop" : "pub"), "location");
      ret += sprintf("%30s\n", ventures[i]);
    }
    else
    {
      ret += sprintf(" - %-25s", "not loaded");
      ret += sprintf(" %-17s", "(??)");
      ret += sprintf("%30s\n", ventures[i]);
    }
  }

  write(ret + "\n");
  return 1;
}

/**
 * The shop or pub component of a location, or nil.
 */
private object venture_of(object loc)
{
  object venture;

  venture = loc->query_component_by_type(LOCATION_COMPONENT_SHOP);
  if (!venture)
    venture = loc->query_component_by_type(LOCATION_COMPONENT_PUB);
  return venture;
}

/**
 * ventures here: the open conditions of the shop or tavern you stand in.
 */
private int do_here(object loc)
{
  object venture;
  mixed * conditions;
  int i;

  venture = venture_of(loc);
  if (!venture)
  {
    notify_fail("This location is neither a shop nor a tavern.\n");
    return 0;
  }

  conditions = (mixed *)venture->query_open_conditions();
  if (!sizeof(conditions))
  {
    write("No open conditions: it is always open.\n");
    return 1;
  }

  write("Open conditions (all must hold):\n");
  for (i = 0; i < sizeof(conditions); i++)
    write("  " + conditions[i][0] + " -- " +
          (string)conditions[i][0]->query_description(conditions[i][1]) + "\n");
  write("Right now it is " +
        (venture->check_open_condition() ? "open" : "closed") + ".\n");
  return 1;
}

/**
 * The file of an open condition: a trailing .c is allowed and dropped, an
 * absolute path is kept, otherwise the game's own conditions come before the
 * shared ones.
 */
private string condition_path(string name, object loc)
{
  string game;

  sscanf(name, "%s.c", name);
  if (name[0] == '/')
    return name;

  game = game_root(loc) + "ventures/conditions/" + name;
  if (file_size(game + ".c") >= 0)
    return game;
  return "/lib/ventures/conditions/" + name;
}

/**
 * ventures condition add <name> [key=value ...] | remove <name>.
 */
private int do_condition(object loc, string * args)
{
  object venture;
  string path, key, value;
  mapping cargs;
  int i, number;

  if (sizeof(args) < 3 || (args[1] != "add" && args[1] != "remove"))
  {
    notify_fail("Usage: ventures condition < add <name> [key=value ...] | " +
                "remove <name> >\n");
    return 0;
  }

  venture = venture_of(loc);
  if (!venture)
  {
    notify_fail("This location is neither a shop nor a tavern.\n");
    return 0;
  }

  path = condition_path(args[2], loc);
  if (file_size(path + ".c") < 0)
  {
    notify_fail("No condition at " + path + ".c\n");
    return 0;
  }

  if (args[1] == "remove")
  {
    if (!venture->remove_open_condition(path))
    {
      notify_fail("This venture has no condition " + path + ".\n");
      return 0;
    }
    loc->save_me();
    write("Removed " + path + ".\n");
    return 1;
  }

  cargs = ([ ]);
  for (i = 3; i < sizeof(args); i++)
  {
    if (sscanf(args[i], "%s=%s", key, value) != 2)
    {
      notify_fail("Arguments are key=value: '" + args[i] + "' is not.\n");
      return 0;
    }
    cargs[key] = (sscanf(value, "%d", number) == 1 && "" + number == value)
                   ? number : value;
  }

  venture->add_open_condition(path, cargs);
  loc->save_me();
  write("Added " + path + ": " + (string)path->query_description(cargs) + ".\n");
  return 1;
}
