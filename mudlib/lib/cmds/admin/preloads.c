
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "preloads" }));
  set_usage("preloads [add|remove <file>]");
  set_help("List or modify files that are preloaded during system startup.");
}

static int cmd(string str, object me, string verb) 
{
  string * preloads, * pieces, ret;
  object target;
  int i;

  preloads = SECURE->query_preload();
  ret = "";

  // without arguments, just list curret preloads
  if (!str || !strlen(str))
  {
    if (!sizeof(preloads))
    {
      write("No preloads.\n");
      return 1;
    }

    ret += "Preloads:\n";

    for (i = 0; i < sizeof(preloads); i++)
    {
      ret += "  - " + preloads[i] + "\n";
    }

    write(ret + "\n");
    return 1;
  }

  pieces = explode(str, " ");

  if (sizeof(pieces) < 2)
  {
    notify_fail("Syntax: " + query_usage() + "\n");
    return 0;
  }

  if (pieces[0] == "add")
  {
    target = load_object(pieces[1]);

    if (!target)
    {
      write("Error loading " + pieces[1] + ".\nIt was not added to the preload list.\n");
      return 1;
    }

    SECURE->add_preload(pieces[1]);
    write("Added " + pieces[1] + " to preloads.\n");
    return 1;
  }
  else if (pieces[0] == "remove")
  {
    if (member_array(pieces[1], preloads) == -1)
    {
      write(pieces[1] + " is not in the preloads list.\n");
      return 1;
    }

    SECURE->remove_preload(pieces[1]);
    write("Removed " + pieces[1] + " from preloads.\n");
    return 1;
  }

  return 1;
}
