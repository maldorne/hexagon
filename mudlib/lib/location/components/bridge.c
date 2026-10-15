// Bridge component: a location that is a bridge and water at once. The rules
// are in /lib/room/bridge.c (it takes the place of the water component, with
// the water's settings); a room built on /lib/bridge.c becomes this component
// when it is converted.
//
// Plugs into the location component hook system (see <room/location.h>).

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit bridge    "/lib/room/bridge.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_BRIDGE);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

object query_water_place() { return query_entity(); }

mapping query_hooks()
{
  return ([
    "dirs_string":       HOOK_PRIORITY_STRUCTURE,
    "short_exit_string": HOOK_PRIORITY_STRUCTURE,
    "glance_string":     HOOK_PRIORITY_STRUCTURE,
    "extra_look":        HOOK_PRIORITY_STRUCTURE,
  ]);
}

// From the water there are no exit lines: in their place, where the shore is.
// On the bridge the exits are listed as in any place.
mixed hook_dirs_string(mixed * args)
{
  string look;

  if (!query_in_water(this_player()))
    return nil;

  look = query_water_look(this_player());
  return ({ HOOK_EXCLUSIVE, look[0 .. strlen(look) - 2] });
}

mixed hook_short_exit_string(mixed * args)
{
  if (!query_in_water(this_player()))
    return nil;
  return ({ HOOK_EXCLUSIVE, "" });
}

string hook_glance_string(mixed * args)
{
  return query_bridge_glance(this_player());
}

string hook_extra_look(mixed * args)
{
  return query_bridge_hint(this_player());
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  bridge_event_enter(ob, from);
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  bridge_event_exit(ob);
}

string query_info()
{
  return "current " + (query_water_current() ? query_water_current() : "none") +
         ", strength " + query_water_strength() +
         (query_bridge_reachable() ? "" : ", unreachable");
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + query_bridge_settings();
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  set_bridge_settings(args);
}

mixed * stats()
{
  return component::stats() + bridge_stats();
}
