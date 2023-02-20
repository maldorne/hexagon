// glance cmd from player.c, now external. Morgoth 2/Mar/95

#include <mud/cmd.h>

#include <common/properties.h>
#include <areas/weather.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "ojear [<objeto>]";
}

string query_short_help()
{
  return "Devuelve una descripción corta de un objeto o del lugar donde estás.";     
}

static int cmd (string arg, object me, string verb)
{
  object here, *ob;
  int i, dark;
  string ret;
  me = me;
  here = environment(me);

  if (!here)
  { 
    notify_fail("Estás en el limbo, no puedes ver nada.\n");
    return 0;
  }

  if (me->query_property(BLIND_PROP))
  {
    notify_fail("¡Estás cegado, no puedes ver nada!\n");
    return 0;
  }

  dark = me->check_dark((int)here->query_light());

  if (!strlen(arg))
  {
    if (me->query_coder())
       tell_object(me, file_name(here) + "\n");

    ret = "";

    switch(dark)
    {
      case 1:
        tell_object(me, "Está demasiado oscuro como para ver algo.\n");
        break;

      case 6:
        tell_object(me, "Estás deslumbrado por la luz.\n");
        break;

      case 2:
      case 5:
        if (here->query_dirs_string() != "")
          ret = (string)here->short(dark) + ".\n";
        else
          ret = (string)here->short(dark);
        //  + (string)here->query_short_exit_string() +".\n";
        break;

      case 3:
      case 4:
        // En este caso NO se ven las salidas, pero con un 'mirar' si se ven
        //  (representa que el mirar es mas detallado!!)
        if (here->query_dirs_string() != "")
          ret = (string)here->short(dark) + ".\n";
        else
          ret = (string)here->short(dark);
        break;

      default:
        ret = (string)here->short(dark) + (string)here->query_short_exit_string() + ".\n" + 
              (string)here->query_contents();
        break;
    }
    
    tell_object(me, ret);
    return 1;
  }
  /*
  if (!sscanf(arg, "a %s", arg))
  {
     notify_fail("Glance a algo!\n");
     return 0;
  }
  */ 
  ob = find_match (arg, ({ me, here }), 1 );

  if (sizeof(ob))
  {
    for (i = 0; i < sizeof(ob); i++)
      // Wonderflug - Nov '95
      if (me == ob[i]) 
        tell_object(me, capitalize(ob[i]->query_cap_name())+
          ", ¡Eres tú mismo!\n");
      else
        tell_object(me, ob[i]->short(dark) + ".\n");
    return 1;
  }
  else
  {
    tell_object(me, "No puedes encontrar ningún '"+arg+"' por aquí.\n");
    // notify_fail("No puedes encontrar "+arg+" por aqui.\n");
    return 1;
  }
  return 0;
}

