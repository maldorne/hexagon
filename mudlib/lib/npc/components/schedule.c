// Schedule component. Gives a settled NPC a daily routine keyed on the game hour
// (0-23), not on dawn/nightfall: a farmer leaves for the fields at 6 and heads
// home at 20, a guard rotates posts at 16 and 0. The NPC subscribes to the
// weather handler's hourly tick; on every hour it looks this hour up in its
// timetable and, if there is an entry, walks to that destination. The walk is the
// NPC's paced, interruptible travel (travel_to) -- combat or conversation
// abandons the trip, and the next matching hour re-issues it.
//
// An "approx" entry delays the departure by random(spread) beats (a per-NPC
// countdown in the heart_beat, no call_out) so a crowd ordered off at the same
// hour staggers out instead of stepping in lockstep. An "exact" entry leaves at
// the hour precisely (a bell, a gate).

#include <areas/weather.h>

inherit component "/lib/npc/component.c";

// The daily routine: game hour (0-23) -> what to do that hour. Each entry is
//   ([ "goto": "work" | "home" | <location file>,
//      "mode": "exact" | "approx",  (default exact)
//      "spread": <beats> ])         (approx only; the max random delay)
// "work" resolves to this NPC's work location, "home" to its home (read live, a
// family shares it and it can change); a literal file lets a guard rotate posts.
mapping timetable;
// This NPC's work location (resolves the "work" symbol in the timetable).
string work;

void create()
{
  component::create();
  set_type("schedule");
  timetable = ([ ]);
  work = nil;
}

string query_work() { return work; }
void set_work(string f) { work = f; }
mapping query_timetable() { return timetable ? timetable : ([ ]); }
void set_timetable(mapping m) { timetable = m ? m : ([ ]); }
void add_entry(int hour, mapping entry) { timetable[hour] = entry; }

// Subscribe the owning NPC to the weather handler's hourly tick when attached.
void initialize(object npc)
{
  component::initialize(npc);
  if (npc)
    handler(WEATHER_HANDLER)->notify_me(npc);
}

// Resolve an entry's "goto" symbol to a concrete location file.
private string _resolve(object npc, string dest)
{
  if (dest == "work")
    return work;
  if (dest == "home")
    return npc->query_home();
  return dest;  // a literal location file (a guard's post)
}

// Forwarded from npc.c::event_weather on every hourly tick (dawn / nightfall /
// check -- the flag itself does not matter, only the hour). Look this game hour
// up in the timetable and head to its destination if there is an entry, unless
// already there. args = ({ who, flag }).
void event_weather(mixed * args)
{
  object npc, here;
  int hour;
  mapping entry;
  string dest;

  npc = query_owner();
  if (!npc)
    return;

  hour = handler(WEATHER_HANDLER)->query_date_data()[0];
  entry = timetable[hour];
  if (!entry)
    return;

  dest = _resolve(npc, entry["goto"]);
  if (!dest || !strlen(dest))
    return;

  // already at the destination this hour? nothing to do
  here = environment(npc);
  if (here && here->query_file_name() == dest)
    return;

  // approx staggers the departure by a random delay; exact leaves at once
  if (entry["mode"] == "approx" && entry["spread"] > 0)
    npc->travel_to_after(dest, random(entry["spread"]));
  else
    npc->travel_to(dest);
}

// Persistence: work + timetable ride in the npc.o.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "work": work, "timetable": timetable ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (args && !undefinedp(args["work"]))
    work = args["work"];
  if (args && !undefinedp(args["timetable"]))
    timetable = args["timetable"];
}
