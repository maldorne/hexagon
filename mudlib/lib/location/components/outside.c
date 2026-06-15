
#include <areas/weather.h>
#include <room/location.h>

inherit component  "/lib/location/component.c";
inherit night      "/lib/room/outside-night.c";
inherit dark       "/lib/room/dark.c";

void create()
{
  night::create();
  dark::create();
  component::create();

  set_type(LOCATION_COMPONENT_OUTSIDE);
}

void init() {}
void dest_me() {}

// will be called after creation + init_auto_load_attributes,
// providing the parent location object
void initialize(object loc)
{
  component::initialize(loc);
}

mapping query_hooks()
{
  return ([
    "long":       HOOK_PRIORITY_ATMOSPHERE,
    "extra_look": HOOK_PRIORITY_ATMOSPHERE,
  ]);
}

// Reduce contract for long: receive ({ str, dark }), return either a
// plain string piece (concatenated by the location's combinator) or
// ({ HOOK_EXCLUSIVE, str }) to replace whatever the chain has built
// so far.
//
// Outside takes authority on two cases that override the base
// description entirely:
//   - night, when the room defines a night-specific long
//   - too dark / too bright, when the visual is whatever the dark
//     subsystem chooses to say (and nothing else makes sense to read)
// Otherwise contributes nothing — the base long stays.
mixed hook_long(mixed * args)
{
  string str;
  int dark;
  string ret;

  str = args[0];
  dark = args[1];

  // looking at a specific item — no atmospheric contribution
  if (str && strlen(str))
    return "";

  switch (dark)
  {
    default: /* normal vision */
      if (this_object()->query_night_long() &&
          !handler(WEATHER_HANDLER)->query_day())
        return ({ HOOK_EXCLUSIVE, this_object()->query_night_long() });
      return "";

    case 1..3: /* too dark */
      ret = query_dark_mess(dark);
      if (handler(WEATHER_HANDLER)->query_day())
        ret += "\nA pesar de esta oscuridad parece ser de día.\n";
      return ({ HOOK_EXCLUSIVE, ret });

    case 4..6: /* too bright */
      ret = query_dark_mess(dark);
      if (!handler(WEATHER_HANDLER)->query_day())
        ret += ".\nA pesar de esta luz parece ser de noche.\n";
      return ({ HOOK_EXCLUSIVE, ret });
  }
}

string hook_extra_look(mixed * args)
{
  return (string)handler(WEATHER_HANDLER)->weather_string(query_my_location())
         + "\n";
}

mixed * stats()
{
  return component::stats() +
        night::stats()+
        dark::stats();
}
