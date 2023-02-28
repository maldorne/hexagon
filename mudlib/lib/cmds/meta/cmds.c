// prints a list of available cmds

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "cmds" }));
  set_usage("cmds");
  set_help("Prints a list of available cmds.\n");
}

static int cmd(string str, object me, string verb)
{
  mapping aliases, hash;
  string * cmds;
  string * lines, line;
  int i;

  lines = ({ });

  aliases = CMD_HANDLER->query_aliases();
  hash = CMD_HANDLER->query_hash();

  cmds = keys(aliases);

  for (i = 0; i < sizeof(cmds); i++)
  {
    line = "";

    switch(hash[aliases[cmds[i]]]["category"])
    {
      case USER_CMD: line += sprintf("%8s", "USER"); break;
      case PLAYER_CMD: line += sprintf("%8s", "PLAYER"); break;
      case CODER_CMD: line += sprintf("%8s", "CODER"); break;
      case ADMIN_CMD: line += sprintf("%8s", "ADMIN"); break;
      default: line += sprintf("%8s", "UNKNOWN"); break;
    }

    line += " " + cmds[i];

    if (cmds[i] != aliases[cmds[i]])
      line += ", alias of " + aliases[cmds[i]];

    if (hash[aliases[cmds[i]]]["package"])
      line += " (package " + hash[aliases[cmds[i]]]["package"] + ")";

    line += "\n";

    lines += ({ line });
  }

  lines = sort_array(lines);

  write("List of available cmds:\n\n" + implode(lines, ""));
  return 1;
}
