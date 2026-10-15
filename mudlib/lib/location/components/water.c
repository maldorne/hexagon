// Water component: a location where one swims instead of walking, a river, a
// lake, the sea, an underground stream. The rules are in /lib/room/water.c;
// a room built on /lib/water.c becomes this component when it is converted,
// with the same settings.
//
// Plugs into the location component hook system (see <room/location.h>).

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit water     "/lib/room/water.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_WATER);
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

// No exit lines in water: in their place, where the shore is.
mixed hook_dirs_string(mixed * args)
{
  string look;

  look = query_water_look(this_player());
  return ({ HOOK_EXCLUSIVE, look[0 .. strlen(look) - 2] });
}

mixed hook_short_exit_string(mixed * args)
{
  return ({ HOOK_EXCLUSIVE, "" });
}

string hook_glance_string(mixed * args)
{
  return query_water_look(this_player());
}

string hook_extra_look(mixed * args)
{
  return query_water_hint(this_player());
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  water_event_enter(ob);
}

string query_info()
{
  return "current " + (query_water_current() ? query_water_current() : "none") +
         ", strength " + query_water_strength();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + query_water_settings();
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  set_water_settings(args);
}

mixed * stats()
{
  return component::stats() + water_stats();
}
