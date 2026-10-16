// Temple component. Marks a location consecrated to a deity and answers for
// whether it is open. It has the same behaviour as the temple room
// (/lib/room/temple/temple-room.c), both from temple-actions.c: worship, offer,
// the sign put up on every load from the temples help document.
//
// A temple stays consecrated whoever holds the ground, but the citizenship
// holding it decides which faiths may be practised there. When the deity is not
// among them the place closes: nobody is taken on, nobody is replaced, nobody
// worships or offers, and the description says so. It opens again if the land
// changes hands back.
//
// The land is the nearest area up the chain that claims a citizenship, so a
// shrine on a road outside the walls still answers to whoever holds the road.

#include <room/location.h>
#include <areas/diplomacy.h>
#include <language.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/temple/temple-actions.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_TEMPLE);
  temple_deity = "";
}

void dest_me() {}

object query_temple_place() { return query_entity(); }

// The deity this place is given to, as the full path of its social object.
// Empty until it is consecrated, and an unconsecrated temple closes for nobody.
string query_deity() { return query_temple_deity(); }
void set_deity(string path) { set_temple_deity(path); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (loc)
    create_temple_sign(loc);
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_temple_sign();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "deity": query_temple_deity(), ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (args && stringp(args["deity"]))
    set_temple_deity(args["deity"]);
}

// The citizenship that holds the ground this temple stands on: its area's
// effective one, taken from the nearest area up the chain that claims one.
// Empty when nobody claims it.
private string _holding_citizenship()
{
  object area;

  if (!query_entity())
    return "";

  area = (object)query_entity()->query_area();
  return area ? (string)area->query_effective_citizenship() : "";
}

// Closed when the people who hold the ground will not have this god worshipped
// on it.
int query_closed()
{
  string held;

  if (!strlen(query_temple_deity()) || !query_entity())
    return 0;

  held = _holding_citizenship();
  if (!strlen(held))
    return 0;

  return !handler("diplomacy", query_entity())->accepts_deity(held,
                                                             query_temple_deity());
}

int query_temple_closed() { return query_closed(); }

string query_info()
{
  if (!strlen(query_temple_deity()))
    return "unconsecrated";
  return query_temple_deity() + (query_closed() ? " (closed)" : "");
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

mixed * stats()
{
  return component::stats() + actions::stats();
}
