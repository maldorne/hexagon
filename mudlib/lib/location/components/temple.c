// Temple component. Marks a location consecrated to a deity and answers for
// whether it is open.
//
// A temple stays consecrated whoever holds the ground, but the citizenship
// holding it decides which faiths may be practised there. When the deity is not
// among them the place closes: nobody is taken on, nobody is replaced, and the
// description says so. It opens again if the land changes hands back.
//
// The land is the nearest area up the chain that claims a citizenship, so a
// shrine on a road outside the walls still answers to whoever holds the road.

#include <room/location.h>
#include <areas/diplomacy.h>
#include <language.h>

inherit component "/lib/location/component.c";

// The deity this place is given to, as the path of its social object. Empty
// until it is consecrated, and an unconsecrated temple closes for nobody.
private string deity;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_TEMPLE);
  deity = "";
}

void init() {}
void dest_me() {}

string query_deity() { return deity; }
void set_deity(string path) { deity = path ? path : ""; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "deity": deity, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (args && stringp(args["deity"]))
    deity = args["deity"];
}

// The citizenship that holds the ground this temple stands on, taken from the
// nearest area up the chain that claims one. Empty when nobody claims it.
private string _holding_citizenship()
{
  object area;
  int steps;

  if (!query_my_location())
    return "";

  area = (object)query_my_location()->query_area();

  for (steps = 0; area && steps < 16; steps++)
  {
    string held;

    held = (string)area->query_citizenship();
    if (held && strlen(held))
      return held;

    area = (object)area->query_parent_area();
  }

  return "";
}

// Closed when the people who hold the ground will not have this god worshipped
// on it.
int query_closed()
{
  string held, game;

  if (!strlen(deity) || !query_my_location())
    return 0;

  held = _holding_citizenship();
  if (!strlen(held))
    return 0;

  return !handler("diplomacy", query_my_location())->accepts_deity(held, deity);
}

string query_info()
{
  if (!strlen(deity))
    return "unconsecrated";
  return deity + (query_closed() ? " (closed)" : "");
}

mapping query_hooks()
{
  return ([ "venue_closed": HOOK_PRIORITY_GATE,
            "long":         HOOK_PRIORITY_ATMOSPHERE ]);
}

mixed hook_venue_closed(mixed * args)
{
  return query_closed();
}

// A closed temple says so where it is looked at; an open one has nothing to add
// that its own description does not already say better.
mixed hook_long(mixed * args)
{
  if (args && sizeof(args) && args[0] && strlen(args[0]))
    return "";

  return query_closed() ? _LANG_TEMPLE_CLOSED : "";
}
