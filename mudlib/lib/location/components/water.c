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

object query_water_place() { return query_my_location(); }

mapping query_hooks()
{
  return ([
    "do_exit_command":   HOOK_PRIORITY_GATE,
    "dirs_string":       HOOK_PRIORITY_STRUCTURE,
    "short_exit_string": HOOK_PRIORITY_STRUCTURE,
  ]);
}

// Pipeline contract for do_exit_command: args = ({ str, verb, ob }). Nobody
// walks out of the water: the move is aborted (verb 0) with the reason.
mixed * hook_do_exit_command(mixed * args)
{
  string refusal;

  refusal = query_walk_refusal(args[2],
              query_my_location()->query_verb_exit(args[1]));
  if (refusal)
  {
    notify_fail(refusal);
    args[1] = 0;
  }

  return args;
}

// The exit lines list where one can swim instead of the exits.
mixed hook_dirs_string(mixed * args)
{
  return ({ HOOK_EXCLUSIVE, query_water_dirs_string(this_player()) });
}

mixed hook_short_exit_string(mixed * args)
{
  return ({ HOOK_EXCLUSIVE, query_water_short_exit_string(this_player()) });
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
