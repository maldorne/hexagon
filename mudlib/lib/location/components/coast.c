// Coast component. Attach to any location at a land–water boundary:
// a sandy beach, a rocky sea cliff, a fishing dock, a bustling
// harbour, a river bank. Contributes to the sector-level "coast" tally
// so the world map can render the sector as shoreline.
//
// Classifier `kind` names the shape of the interface. No implicit
// ordering — pick whichever fits:
//   beach | cliff | port | harbor | bank
//
// The edge of water also works as one: the rules are in
// /lib/room/coast.c (exits into water are swum, not walked), and a room
// built on /lib/coast.c becomes this component when it is converted.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit coast     "/lib/room/coast.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_COAST);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

object query_coast_place() { return query_my_location(); }

string query_kind() { return query_coast_kind(); }
void set_kind(string s) { set_coast_kind(s); }

mapping query_hooks()
{
  return ([
    "do_exit_command": HOOK_PRIORITY_GATE,
    "extra_look":      HOOK_PRIORITY_STRUCTURE,
  ]);
}

// Pipeline contract for do_exit_command: args = ({ str, verb, ob }). Nobody
// walks into the water: the move is aborted (verb 0) with the reason.
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

string hook_extra_look(mixed * args)
{
  return query_coast_look(this_player());
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  coast_event_enter(ob);
}

string query_info() { return query_coast_kind(); }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "kind": query_coast_kind(), ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["kind"]))
    set_coast_kind(args["kind"]);
}

mixed * stats()
{
  return component::stats() + coast_stats();
}
