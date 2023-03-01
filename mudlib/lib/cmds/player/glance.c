// glance cmd from player.c, now external. Morgoth 2/Mar/95

#include <mud/cmd.h>

#include <common/properties.h>
#include <areas/weather.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_GLANCE_ALIAS);
  set_usage(_LANG_CMD_GLANCE_SYNTAX);
  set_help(_LANG_CMD_GLANCE_HELP);
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
    notify_fail(_LANG_CMD_LOOK_NO_ENVIRONMENT);
    return 0;
  }

  if (me->query_property(BLIND_PROP))
  {
    notify_fail(_LANG_CMD_LOOK_BLINDED);
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
        tell_object(me, _LANG_CMD_LOOK_TOO_DARK);
        break;

      case 6:
        tell_object(me, _LANG_CMD_LOOK_TOO_BRIGHT);
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
        // in this case we do not see the exits, but they do with "look"
        // because the look is more detailed
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
     notify_fail("Glance something!\n");
     return 0;
  }
  */ 
  ob = find_match (arg, ({ me, here }), 1 );

  if (sizeof(ob))
  {
    for (i = 0; i < sizeof(ob); i++)
      // Wonderflug - Nov '95
      if (me == ob[i]) 
        tell_object(me, capitalize(ob[i]->query_cap_name()) + _LANG_CMD_GLANCE_YOURSELF);
      else
        tell_object(me, ob[i]->short(dark) + ".\n");
    return 1;
  }
  else
  {
    tell_object(me, _LANG_CMD_NOTHING_HERE);
    // notify_fail(_LANG_CMD_NOTHING_HERE);
    return 1;
  }
  return 0;
}

