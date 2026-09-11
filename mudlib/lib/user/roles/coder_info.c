
#include <mud/secure.h>
#include <user/user.h>
#include <basic/actions.h>
#include <mud/cmd.h>

void create()
{
}

void role_commands()
{
  // add_action("do_gauge", "gauge");
  /* Added by Jada aug '94, blame him if it won't work */
  add_action("do_title", ({ "title", "titulo", "título" }));
  /* Adds for new commandsystem.. */
  add_action("comm_info", "cominfo");
} 


// int do_gauge(string str) 
// {
//   int eval_cost;

//   if (this_player(1) != this_object()) 
//     return 0;
  
//   if (!str) 
//   {
//     notify_fail("Uso: gauge <comando>.\n");
//     return 0;
//   }
  
//   eval_cost = command(str);
//   write("\nEl comando '" + str + "' usó: " + eval_cost + " ciclos de CPU.\n");
//   return 1;
// } 

int do_title(string str)
{
  string title;
  title = this_player()->query_title();
  
  if (!strlen(str))
  {
    if (title) 
      write("Tu título es: "+title+"\n");
    else
      write("No tienes título.\n");
      
    write("Sintaxis: titulo <texto>\n"+
          "          titulo -c para borrarlo\n");
  }
  else if (str == "-c")
  {
    write("Borrando tu título.\n");
    this_player()->set_title("");
    return 1;
  }
  else
    this_player()->set_title(str);

  return 1;
}

string _do_find_comm(string func, object ob) 
{
  string s, ping;
  object fish;
  
  s = "";
  
  if (ping = function_exists(func, ob))
    s += " found in " + ping;
  else
    s += " not found";
    
  fish = ob;
  
  while(fish = shadow(fish, 0))
    if (function_exists(func, fish))
      s += " shadowed por " + file_name(fish);
      
  s += ".\n";
  return s;
} 

// By Dyraen@Rod
int comm_info(string str) 
{
  object on;
  mixed * comms; 
  string xtra, s1, s2, text;
  int i, j, num_comms;

  num_comms = 0;

  if (sscanf(str, "%s %s", s1, s2) && strlen(s2)) 
  {
    s2 = this_player()->expand_nickname(s2);
    on = find_player(s2);
    str = s1;
  }

  if (on)
    // comms = on->query_commands();
    comms = commands(on);
  else
    comms = commands(this_player());

  text = "";
  
  if (!strlen(str))
  {
    for (i = 0; i < sizeof(comms); i++) 
    {
      xtra = _do_find_comm((string)comms[i][C_FUNC], (object)comms[i][C_OBJ]);
      text += (i+1) + ". " + comms[i][C_NAME] + " " + // "[" + comms[i][C_DATA] + "] " +
        base_name((object)comms[i][C_OBJ]) + "->" + comms[i][C_FUNC] + "()" +
        xtra;
    }

    num_comms = sizeof(comms);
    
    // add /cmds commands, neverbot 01/2010
    comms = CMD_HANDLER->query_available_cmds(this_player());
    num_comms += sizeof(comms);

    for (j = 0; j < sizeof(comms); j++) 
    {
      string * pieces;
      pieces = explode(comms[j], "/");      

      text += "" + (i+j+1) + ". " + pieces[sizeof(pieces)-1] + " " + comms[j] + 
        "->cmd() found in " +
        comms[j] + "\n"; 
    }
    
    i += j;
    
    // TODO add souls here
    // add souls, neverbot 01/2010
    /*
    comms = keys("/obj/handlers/soul.c"->query_soul_data());
    num_comms += sizeof(comms);
    
    for (j = 0; j < sizeof(comms); j++) 
    {
      text += "" + (i+j+1) + ". " + comms[j] + " soul command found in /obj/handlers/soul.c\n"; 
    }
    */
  
    if (on)
      write("Searching in " + on->query_name() + "... " + num_comms + " commands.\n");
    else
      write("Searching... " + num_comms + " commands.\n");
    
    this_user()->more_string(text);
  }
  else // looking for a specific command
  {
    for (i = 0; i < sizeof(comms); i++)
    {
      if (str == (string)comms[i][C_NAME]) 
      {
        xtra = _do_find_comm((string)comms[i][C_FUNC], (object)comms[i][C_OBJ]);
        text += comms[i][C_NAME] + " " + // "[" + comms[i][C_DATA] + "] " +
          base_name((object)comms[i][C_OBJ]) + "->" + comms[i][C_FUNC] + "()" +
          xtra;
        // break;
      }
    }
    
    num_comms = sizeof(comms);

    // add /cmds commands, neverbot 01/2010
    comms = CMD_HANDLER->query_available_cmds(this_player());
    num_comms += sizeof(comms);   
    
    // if we haven't found it, search in the cmds
    if (text == "")
    {     
      for (j = 0; j < sizeof(comms); j++) 
      {
        string * pieces;
        pieces = explode(comms[j], "/");

        if (str == pieces[sizeof(pieces)-1])
        {
          text = pieces[sizeof(pieces)-1] + " " + comms[j] + "->cmd() found in " +
            comms[j] + "\n"; 
          break;
        }
      }
    }
    
    // TODO add souls
    // add souls, neverbot 01/2010
    /*
    comms = keys("/obj/handlers/soul.c"->query_soul_data());
    num_comms += sizeof(comms);
        
    // if not found, search in the soul commands
    if (text == "")
    {     
      for (j = 0; j < sizeof(comms); j++) 
      {
        if (str == comms[j])
        {
          text += "" + (i+j) + ". " + comms[j] + " soul command found in /obj/handlers/soul.c\n"; 
          break;
        }
      }
    } 
    */

    if (on)
      write("Searching in " + on->query_name() + "... " + num_comms + " commands.\n");
    else
      write("Searching... " + num_comms + " commands.\n");
    
    write(text);  
  }
  return 1;
}

mixed * stats() 
{
  return ({ 
          });
}
