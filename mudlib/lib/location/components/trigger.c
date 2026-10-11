// Trigger component: something happens to whoever enters the location
// carrying a given mark, a timed property some other action put on them
// (filling a cup, pulling a lever...). A moment after it arrives, a trigger
// removes the mark, tells the living a message and, if it names an exit,
// takes the living through it with force_exit, so a sealed exit opens for
// that one living alone.
//
// A trigger is a mapping: "mark", "message", and optionally "exit" and
// "room_message" (read by the others after the living's name). Rooms
// declare them with add_entry_trigger (/lib/room/trigger.c).

#include <room/location.h>

inherit component "/lib/location/component.c";

private mixed * trigger_list;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_TRIGGER);
  trigger_list = ({ });
}

void set_triggers(mixed * list) { trigger_list = list ? ({ }) + list : ({ }); }
mixed * query_triggers() { return ({ }) + trigger_list; }

string query_info()
{
  string * marks;
  int i;

  marks = ({ });
  for (i = 0; i < sizeof(trigger_list); i++)
    marks += ({ "" + trigger_list[i]["mark"] });
  return implode(marks, ", ");
}

// Fire a trigger on a living that came in with its mark, once its arrival has
// been shown: the messages, then the trip through the exit, if any.
void fire(object who, mapping trigger)
{
  object place;

  place = query_my_location();
  if (!who || !place || environment(who) != place)
    return;

  if (stringp(trigger["message"]))
    tell_object(who, trigger["message"]);
  if (stringp(trigger["room_message"]))
    tell_room(place, who->query_cap_name() + " " + trigger["room_message"],
              who);
  if (stringp(trigger["exit"]))
    place->force_exit(trigger["exit"], who);
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  int i;

  if (!ob || !living(ob))
    return;

  // the mark goes at once, so the trigger fires once for each mark
  for (i = 0; i < sizeof(trigger_list); i++)
    if (stringp(trigger_list[i]["mark"]) &&
        ob->query_timed_property(trigger_list[i]["mark"]))
    {
      ob->remove_timed_property(trigger_list[i]["mark"]);
      call_out("fire", 1, ob, trigger_list[i]);
      return;
    }
}

// ************************************************************
//  Persistence
// ************************************************************

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "trigger_list": trigger_list ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (pointerp(args["trigger_list"]))
    trigger_list = args["trigger_list"];
}
