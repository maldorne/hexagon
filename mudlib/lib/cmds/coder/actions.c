
#include <mud/cmd.h>


inherit CMD_BASE;

string query_help()
{
  return "Prints a list of all available actions for the user.";
}

static int cmd(string str, object me, string verb)
{
  object * targets;
  mapping actions;
  int i, j;
  string * keys_objects, * keys_actions, ret;

  targets = targets(me);
  actions = ([ ]);

  for (i = 0; i < sizeof(targets); i++)
    if (targets[i])
      actions[base_name(targets[i])] = targets[i]->query_actions();

  // write(to_string(actions));

  ret = "";
  keys_objects = keys(actions);

  for (i = 0; i < sizeof(keys_objects); i++)
  {
    ret += keys_objects[i] + ":\n";

    keys_actions = keys(actions[keys_objects[i]]);

    for (j = 0; j < sizeof(keys_actions); j++)
    {
      ret += "  " + keys_actions[j] + " -> " + actions[keys_objects[i]][keys_actions[j]] + "\n";
    }
  }

  write(ret);
  return 1;
}
