#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_COMMANDS_ALIAS);
  set_usage(_LANG_CMD_COMMANDS_SYNTAX);
  set_help(_LANG_CMD_COMMANDS_HELP);
}

static int cmd(string str, object me, string verb)
{
  object handler;
  string help, result, result_aux;
  string * categories;
  mapping cmds;
  int i, j, list_mode;

  list_mode = 0;
  categories = ({ });
  result = "";
  result_aux = "";
  handler = load_object(CMD_HANDLER);

  if (!handler)
    return 1;
  
  if (strlen(str) && str != "-l")
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_COMMANDS_SYNTAX + "\n");
    return 1;
  }
  
  if (str == "-l")
    list_mode = 1;
  
  cmds = handler->query_available_cmds_by_category(me);
  
  categories = keys(cmds);
  
  for (i = 0; i < sizeof(categories); i++)
  {
    result += _LANG_CMD_COMMANDS_CATEGORY_MSG;
    
    for (j = 0; j < sizeof(cmds[categories[i]]); j++)
    {
      mapping entry;
      string name;

      // each entry is the compact ([ "name", "help" ]) the handler built from
      // the cmd hash: the command's localized display name and its one-line
      // help. The player only ever types the localized word, and no command
      // object is loaded to list it.
      entry = cmds[categories[i]][j];
      name = entry["name"];

      if (list_mode)
      {
        help = entry["help"] ? entry["help"] : "";
        if (help != "")
          result += sprintf("  %13s: %s\n", name, help);
        else
          result += sprintf("  %13s\n", name);
      }
      else
      {
        result_aux += name + "\n";
      }
    }
    
    if (!list_mode)
    {
      result += sprintf("%#-*s\n", me->user()->query_cols(), result_aux);
      result_aux = "";
    }
  }
  
  // tell_object(me, result);
  me->user()->more_string(result + "\n", _LANG_CMD_COMMANDS_ALIAS[0]);
  return 1;
}

