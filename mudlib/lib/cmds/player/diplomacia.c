

#include <standard.h>
#include <cmd.h>
#include <diplomacy.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "diplomacia";
}

string query_short_help()
{
  return "Obtiene información sobre las relaciones diplomáticas de tu ciudadanía y del territorio en el que te encuentras.";
}

protected int cmd (string str, object me, string verb)
{
  string ret = "";
  string citizenship = me->query_citizenship();
  string zone = "";
  
  object handler = load_object(DIPLOMACY_HANDLER);
  mixed * info;
  
  if (!handler)
  {
    notify_fail("Hay algún problema con las ciudadanías, consulta con un administrador.\n");
    return 0;
  }

  // Info about the player diplomacy

  if (member_array(citizenship, ZONES) == -1)
  {
    notify_fail("Hay algún problema con tu ciudadanía, consulta con un administrador.\n");
    return 0;
  }

  if (citizenship != "")
  {
    info = handler->query_diplomacy_info(citizenship);

    ret += "Eres " + me->query_numeral() + " ciudadan" + me->query_vocal() + " de " + citizenship;

    if (info[0] != citizenship)
      ret += ", parte del Reino de " + info[0] + ".\n\n";
    else
      ret += ".\n\n";

    if (sizeof(info[1]))
      ret += " * Los aliados de " + citizenship + " son: " + query_multiple_short(info[1]) + ".\n";
    if (sizeof(info[2]))
      ret += " * Los enemigos de " + citizenship + " son: " + query_multiple_short(info[2]) + ".\n";

    if (sizeof(info[1]) || sizeof(info[2]))
      ret += "\n";
  }  

  tell_object(me, ret);

  // Info about the environment diplomacy

  ret = "";

  // Only rooms and directories in domains will have diplomacy data
  if (!is_in_game(environment(me)))
    return 1;

  zone = handler->query_diplomacy_name(base_name(environment(me)));

  if (member_array(zone, ZONES) == -1)
  {
    notify_fail("Hay algún problema con la diplomacia de esta zona, consulta con un administrador.\n");
    return 0;
  }  

  if (zone == NEUTRAL)
  {
    ret += "Te encuentras en territorio neutral.\n";
  }
  else
  {
    info = handler->query_diplomacy_info(zone);

    if (!info)
    {
      notify_fail("Hay algún problema con la diplomacia de esta zona, consulta con un administrador.\n");
      return 0;
    }  

    ret += "Te encuentras en el territorio de " + zone;

    if (info[0] != zone)
      ret += ", parte del Reino de " + info[0] + ".\n\n";
    else
      ret += ".\n\n";

    if (zone != citizenship)
    {
      if (sizeof(info[1]))
        ret += " * Los aliados de " + zone + " son: " + query_multiple_short(info[1]) + ".\n";
      if (sizeof(info[2]))
        ret += " * Los enemigos de " + zone + " son: " + query_multiple_short(info[2]) + ".\n";
    }
  }

  tell_object(me, ret);

  return 1;

}


