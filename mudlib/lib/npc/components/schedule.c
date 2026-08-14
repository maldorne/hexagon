// Schedule component. Gives a settled NPC a daily routine: walk to its work
// location at dawn and home at nightfall. Work is set from the NPC's role when
// the area materializes it; home is whatever the housing system assigned the NPC
// (owner->query_home()), read live so a move picks up.
//
// It reacts to the calendar's day/night ticks: on attach it subscribes the NPC
// to the weather handler, and npc.c forwards those ticks to this component
// (event_weather). The actual walking is the NPC's paced, interruptible travel
// (travel_to) -- combat or conversation abandons the trip, and the next tick
// re-issues it.

#include <areas/weather.h>

inherit component "/lib/npc/component.c";

// Where this NPC works (a location file). Home is not stored here -- it lives on
// the NPC (query_home), because a family shares one and it may change.
string work;

void create()
{
  component::create();
  set_type("schedule");
  work = nil;
}

string query_work() { return work; }
void set_work(string f) { work = f; }

// Subscribe the owning NPC to the calendar's day/night ticks when attached.
void initialize(object npc)
{
  component::initialize(npc);
  if (npc)
    handler(WEATHER_HANDLER)->notify_me(npc);
}

// Forwarded from npc.c::event_weather via run_on_components. args = ({ who,
// flag }). Dawn -> head to work; nightfall -> head home. Travels only when a
// destination is set; the NPC's pacing walks it there one room at a time.
void event_weather(mixed * args)
{
  object npc;
  int flag;
  string dest;

  npc = query_owner();
  if (!npc || !args || sizeof(args) < 2)
    return;

  flag = args[1];

  if (flag == FLAG_DAWN)
    dest = work;
  else if (flag == FLAG_NIGHTFALL)
    dest = npc->query_home();
  else
    return;

  if (dest && strlen(dest))
    npc->travel_to(dest);
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([ "work": work ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (args && !undefinedp(args["work"]))
    work = args["work"];
}
