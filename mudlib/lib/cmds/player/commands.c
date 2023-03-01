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
  object handler, command;
  string filename, help, result, result_aux;
  string * aux;
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
      aux = explode(cmds[categories[i]][j], "/");
      filename = aux[sizeof(aux) - 1];
      filename = explode(filename, ".")[0];

      if (list_mode)
      {
        catch(command = load_object(cmds[categories[i]][j]));
      
        if (!command)
          continue;
        
        if (command->query_help())
        {
          string * pieces;
          // help = command->query_help()[0..60];
          help = command->query_help();
          pieces = explode(help, "\n");
          if (sizeof(pieces))
            help = pieces[0];
        }
        else 
          help = "";
      
        if (help != "")
          result += sprintf("  %13s: %s\n", filename, help);
        else
          result += sprintf("  %13s\n", filename);
      }
      else
      {
        result_aux += filename + "\n";
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

