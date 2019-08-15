// Extraido de player.c para reducir complejidad, Folken 4/2003
// Aqui realizamos todas las comprobaciones necesarias antes de retirar
// al jugador, y borramos los detalles que no requieren seguridad (el
// resto se hace desde el try_retire de /secure/master

#include <standard.h>
#include <cmd.h>

#include <player.h>

inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
  int res = 1;
  
  // Añadido por Folken, no puedo creer que esto no estuviera puesto :P
  if (me->query_property(GUEST_PROP)){
    notify_fail("Eres un invitado en "+mud_name()+".\n"+
                "¿Aún no tienes un personaje y ya estás pensando en retirarte?\n");
    return 0;
  }

  // should probably do a more secure check, but the retire has a
  // password.
  res = "/secure/master"->try_retire(me);

  /*
  "/obj/handlers/god_handler"->remove_high_priest(
                                       this_object()->query_name(),
                                       this_object()->query_deity());
  */
  return res;
}

