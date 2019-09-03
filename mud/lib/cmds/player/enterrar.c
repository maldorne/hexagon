/*
 * Enterrar sacado del std/command para que consuma menos memoria Errante
 * Adaptado a Cc, Folken 20/04/2003
 */

#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){
    position = 0;
}

string query_usage()
{
  return "enterrar";
}

string query_short_help()
{ 
  return "Este comando entierra todos los cuerpos en la habitación. Nada de los cuerpos enterrados podrá ser recuperado."; 
}

protected int cmd (string str, object me, string verb)
{
  object *target;
  int counter,i;
 
  if (!str || str == "") 
    str = "cuerpo";

  if(me->query_dead())
  {
    tell_object(me,"Tú, un espíritu, ¿quieres enterrar cadáveres?\n");
    return 1;
  }

  if (environment(me) && environment(me)->query_water_environment())
  {
    tell_object(me,"¿Quieres enterrar algo estando en el agua?\n");
    return 1;
  }

  target = all_inventory(environment(me));

  counter = 0;
  for (i=0;i<sizeof(target);i++)
    if( target[i]->query_corpse() )
    {
      target[i]->dest_me(); // Allow them to clean up themselves.
      counter++;
    }

  if (counter)
    tell_object(me,"Entierras "+(counter == 1?"":"los cuerpos")+
      (counter > 1?"":"el cuerpo")+
      " bajo tierra.\n");
  else
    tell_object(me,"¡No has encontrado ningún cuerpo!\n");

  return 1;
}

