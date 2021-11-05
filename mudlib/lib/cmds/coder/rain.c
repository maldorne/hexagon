#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

string query_usage()
{
  return "rain <how many> <type of coin>";
}

string query_help() 
{
  return "Create a set of coins for you";
}

string get_type(string str) 
{      
  if (str == "c" || str == "cobre")   
    return "cobre";      
  if (str == "b" || str == "bronce")  
    return "bronce";       
  if (str == "s" || str == "plata")    
    return "plata";         
  if (str == "g" || str == "o" || str == "oro")    
    return "oro";         
  if (str == "p" || str == "platino")  
    return "platino";       

  return "none";          
} 

static int cmd(string str, object me, string verb) 
{
  string type, cktype;    
  int amt;         

  if ((!strlen(str)) || (sscanf(str, "%d %s", amt, type) != 2))
  {
    tell_object(this_player(), "Syntax: " + query_usage() + "\n");
    return 1;
  }   

  cktype = get_type(type);
  
  if (cktype == "none") 
  {     
    tell_object(this_player(), "Deberías usar un tipo de moneda de "+mud_name()+".\n"); 
    return 1;
  }  
  
  if (this_player()->adjust_money(amt, cktype)) 
  {
    tell_object(this_player(), "Te son concedidas "+amt+" monedas de "+cktype+".\n");

    tell_room(environment(this_player()), this_player()->query_cap_name() +
      " sostiene su monedero, pronuncia unas palabras y puedes ver como" +
      " el monedero se hace visiblemente más grande.\n", this_player());  
    return 1;
  } 
  else 
  {
    tell_object(this_player(),"¡No ocurrió nada! ¿Dónde está el fallo?\n");
    return 1;
  }

} 