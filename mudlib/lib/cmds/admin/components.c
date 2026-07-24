
#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

#define COMPONENTS_DIR "/lib/location/components/"

void setup()
{
  set_aliases(({ "components" }));
  set_usage("components\n" +
            "components add <type>\n" +
            "components remove <type>");
  set_help(
    "Manage the components attached to the location you are standing in.\n" +
    "\n" +
    "  components              list the components on this location\n" +
    "  components add <type>   attach a component (city, forest, ...)\n" +
    "  components remove <type> detach a component\n" +
    "\n" +
    "The type is the component blueprint name under " + COMPONENTS_DIR + ".\n" +
    "The location is saved after any change so its sector reclassifies.\n");
}

static int cmd(string str, object me, string verb)
{
  string * tokens;
  object env;
  string type;

  env = environment(me);
  if (!env || !env->query_location())
  {
    notify_fail("You are not standing in a location.\n");
    return 0;
  }

  tokens = explode(str ? str : "", " ") - ({ "" });

  // ===== no args: list =====
  if (!sizeof(tokens))
  {
    object * comps;
    string ret;
    int i;

    comps = env->query_components();
    if (!comps || !sizeof(comps))
    {
      write("This location has no components.\n");
      return 1;
    }

    ret = "Components on this location:\n";
    for (i = 0; i < sizeof(comps); i++)
      ret += "  - " + comps[i]->query_type() + "\n";
    write(ret);
    return 1;
  }

  if (sizeof(tokens) != 2 ||
      (tokens[0] != "add" && tokens[0] != "remove"))
  {
    notify_fail("Usage: components [ add <type> | remove <type> ]\n");
    return 0;
  }

  type = tokens[1];

  if (tokens[0] == "remove")
  {
    if (!env->query_component_by_type(type))
    {
      notify_fail("This location has no '" + type + "' component.\n");
      return 0;
    }
    env->remove_component(type);
    env->save_me();
    write("Removed component '" + type + "'.\n");
    return 1;
  }

  // add
  if (file_size(COMPONENTS_DIR + type + ".c") <= 0)
  {
    notify_fail("No component blueprint '" + type + "' under " +
                COMPONENTS_DIR + ".\n");
    return 0;
  }
  if (env->query_component_by_type(type))
  {
    notify_fail("This location already has a '" + type + "' component.\n");
    return 0;
  }
  env->add_component(type, ([ ]));
  env->save_me();
  write("Added component '" + type + "'.\n");
  return 1;
}
