
#include <areas/weather.h>
#include <room/location.h>

inherit component  "/lib/location/component.c";
inherit night      "/lib/room/outside-night.c";

void create()
{
  night::create();
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
    "long":        HOOK_PRIORITY_ATMOSPHERE,
    "extra_look":  HOOK_PRIORITY_ATMOSPHERE,
    "query_light": HOOK_PRIORITY_ATMOSPHERE,
  ]);
}

// Reduce contract for long: receive ({ str, dark }), return either a
// plain string piece (concatenated by the location's combinator) or
// ({ HOOK_EXCLUSIVE, str }) to replace whatever the chain has built
// so far.
//
// Outside takes authority at night, when the place defines a
// night-specific long. Darkness itself is the location's business (it
// replaces the description with the darkness message whatever components
// the place has), so here the night long only applies to normal sight.
// Otherwise contributes nothing: the base long stays.
mixed hook_long(mixed * args)
{
  string str;
  int dark;

  str = args[0];
  dark = args[1];

  // looking at a specific item, or too dark or bright to see the place
  if ((str && strlen(str)) || dark)
    return "";

  if (this_object()->query_night_long() &&
      !handler("weather", query_my_location())->query_day())
    return ({ HOOK_EXCLUSIVE, this_object()->query_night_long() });
  return "";
}

// Reduce contract for query_light: the light of an outdoor place is its own
// light by the percentage the hour and the weather allow, plus whatever is
// carried in (a torch), which shines the same at any hour.
mixed hook_query_light(mixed * args)
{
  object loc;
  int percent;

  loc = query_my_location();
  if (!loc)
    return 0;

  percent = (int)handler("weather", loc)->query_darkness(loc);
  return ({ HOOK_EXCLUSIVE,
            loc->query_my_light() * percent / 100 + loc->query_int_light() });
}

// A component belongs to no game, so the weather handler is resolved against
// its location: a game that overrides the handler answers for its own places.
string hook_extra_look(mixed * args)
{
  // weather_string already ends in a newline, so do not add another or the
  // description gains a blank line under the weather report.
  return (string)handler("weather", query_my_location())->weather_string(query_my_location());
}

mixed * stats()
{
  return component::stats() +
        night::stats();
}
