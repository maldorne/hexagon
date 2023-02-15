// look cmd from player.c, now external. Morgoth 2/Mar/95
// reviewed for ccmud, neverbot 4/03

#include <mud/cmd.h>

#include <common/properties.h>
#include <areas/weather.h>
#include <language.h>

#define DEFAULT_TIME 10

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return _LANG_CMD_LOOK_SYNTAX;
}

string query_help()
{
  return _LANG_CMD_LOOK_HELP;
}

static int cmd(string arg, object me, string verb)
{
  object here, *ob;
  int i, dark;
  string ret;

  ret = "";
  here = environment(me);

  if (!here)
  {
    write(_LANG_CMD_LOOK_NO_ENVIRONMENT);
    return 1;
  }

  if (me->query_timed_property(BLIND_PROP) && me->query_coder() == 0)
  {
    write(_LANG_CMD_LOOK_BLINDED);
    return 1;
  }

  // included from /global/pweath.c, now here, neverbot 11/03
  /*
  if ((arg == "sol") || (arg == "luna") || (arg == "lunas") || (arg == "clima") ||
  (arg == "tiempo")){
    // if ((string)environment()->query_property("location") != "outside")
    if (!here->query_outside())
    {
      notify_fail("Necesitas estar al aire libre para poder fijarte en el tiempo.\n");
      return 0;
    }

    if (arg == "sol")
      if (WEATHER->query_day())
      {
        write("¡Ouch, el sol es brillante!\n");
        return 1;
      }
      else
      {
        write("El sol no ha salido todavía, tendrás que esperar a que amanezca.\n");
        return 1;
      }
    if ((arg == "luna") || (arg == "lunas"))
      if (WEATHER->query_day())
      {
        write("Las lunas no han aparecido todavía, prueba más tarde.\n");
        return 1;
      }
      else if (WEATHER->moon_string() != "")
      {
        write(WEATHER->moon_string());
        return 1;
      }
      else
      {
        write("Las lunas no han aparecido todavía, prueba más tarde.\n");
        return 1;
      }
    write(WEATHER->weather_string(here));
    return 1;
  }
  */

  // enf of /global/pweath.c

  dark = me->check_dark((int)here->query_light());

  if (!stringp(arg) || !strlen(arg))
  {
    if (me->query_coder())
      ret = file_name(here) + ".c\n";

    if (!dark)
        ret += here->short(dark) + ".\n";

    ret += here->long(arg, dark); //+"\n";

    write(ret);

    return 1;
  }

  sscanf(arg, "a %s", arg);
  
  // not needed, find_match does it
  // arg = me->expand_nickname(arg);

  ob = find_match(arg, ({ me, here }), 1 );
  ret = "";

  if (!sizeof(ob))
  {
    notify_fail(_LANG_CMD_NOTHING_HERE);
    return 0;
  }

  // for (i = sizeof(ob)-1; i >= 0; i--)
  //   if (ob[i]->query_hide_shadow() && ob[i]->query_name()!=me->query_name())
  //     ob = delete(ob,i,1);

  if (living(ob[0]) &&
      ob[0]->query_coder() && ob[0]->query_invis() &&
      !me->query_coder())
  {
    notify_fail(_LANG_CMD_NOTHING_HERE);
    return 0;
  }

  if (pointerp(ob))
  {
    for (i = 0; i < sizeof(ob); i++)
    {
      if (!ob[i]->query_player())
        ret += capitalize(ob[i]->short())+".\n";

      ret += ob[i]->long(arg, dark);
    }
  }
  else
  {
    ret += ob->long(arg, dark);
  }

  me->user()->more_string(ret, capitalize(_LANG_CMD_LOOK));
  return 1;
}
