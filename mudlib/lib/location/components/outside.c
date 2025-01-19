
#include <areas/weather.h>

inherit component  "/lib/location/component.c";
inherit night      "/lib/room/outside-night.c";
inherit dark       "/lib/room/dark.c";

void create()
{
  night::create();
  dark::create();
  component::create();

  set_type("outside");
}

void init() 
{

}

void dest_me() 
{

}

// will be called after creation + init_auto_load_attributes,
// providing the parent location object
void initialize(object location)
{
  component::initialize(location);
}

int override_function(string func)
{
  if (func == "long")
  {
    int dark;
    dark = 0;

    if (this_player())
      dark = (int)this_player()->check_dark(query_my_location()->query_light());

    switch (dark) /* O.K. how much can we see at night */
    {
      default: /* can see anyway */
        // override only if it is night AND we have a special night long
        if (this_object()->query_night_long() && !handler(WEATHER_HANDLER)->query_day())
          return 1;
        break;

      case 1..3: /* too dark */
        return 1;
      case 4..6: /* too bright */
        return 1;
    }
  }

  // by default a component never overrides anything
  return 0;
}

string long(varargs string str, int dark)
{
  string s, ret;

  // looking at something
  if (str && strlen(str))
    return "";

  ret = "";

  switch (dark) /* O.K. how much can we see at night */
  {
    default: /* can see anyway */
      /* night... */
      if (this_object()->query_night_long() && !handler(WEATHER_HANDLER)->query_day())
        return this_object()->query_night_long();
      break;

    case 1..3: /* too dark */
      ret += query_dark_mess(dark);
      if (handler(WEATHER_HANDLER)->query_day())
        ret += "\nA pesar de esta oscuridad parece ser de día.\n";
      return ret;

    case 4..6: /* too bright */
      ret += query_dark_mess(dark);
      if (!handler(WEATHER_HANDLER)->query_day())
        ret += ".\nA pesar de esta luz parece ser de noche.\n";
      return ret;
  }

  /* if we got here we can see */
}

string extra_look()
{
  return (string)handler(WEATHER_HANDLER)->weather_string(this_object()) + "\n";
}

mixed * stats()
{
  return component::stats() +
        night::stats()+
        dark::stats();
}
