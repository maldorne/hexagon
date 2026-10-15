// Schedule component. Holds an NPC's daily routine as a timetable keyed on the
// game hour (0-23) and acts on it when told to. The areas handler decides which
// NPCs must act at each hour (from the area census) and calls do_schedule(hour)
// on them; the NPC then reads its own timetable and walks where it should be. So
// the area is the dispatcher ("your turn"), the NPC decides what to do and how.
//
//   timetable: ([ hour(0-23) : ([ "goto": "work" | "home" | "out" |
//                                         <location file>,
//                                 "leave": <text>, "arrive": <text> ]) ])
//
// "work" resolves to this NPC's work location, "home" to its home (read live, a
// family shares it and it can change), "out" to one of its area's points of
// interest picked at random, from where an NPC that wanders goes on wandering;
// a literal file lets a guard rotate posts. While the entry in effect is "home"
// and the NPC is there, it does not wander out of it.
// The walk is the NPC's paced, interruptible travel (travel_to) -- combat or
// conversation abandons the trip, and the next matching hour re-issues it.
//
// `leave` is told to the room the NPC sets off from, `arrive` to the one it
// reaches, once it has closed any door behind it. Both are optional souls
// templates written in the mud's language ($mcname$ = this NPC's kind/name,
// $mpronoun$/$mposs$/$mobj$, $mvocal$ = the o/a vowel for gender agreement),
// so a day of work and home carries four lines at most.

#include <areas/area.h>

inherit component "/lib/npc/component.c";

// The hour this NPC works from, resolving the "work" symbol in the timetable.
// Home is not stored here -- it lives on the NPC (query_home).
string work;
// The routine: game hour -> what to do that hour (see the header).
mapping timetable;
// what the last entry acted on said to do ("home" keeps the NPC in)
static string current_goto;
// the trip under way: where it ends and the line to tell when it does
static string trip_dest, trip_arrive;
// the arrival line waiting for the queued steps to run, and checks left
static string arrival_line;
static int arrive_tries;

void create()
{
  component::create();
  set_type("schedule");
  work = nil;
  timetable = ([ ]);
}

string query_work() { return work; }
void set_work(string f) { work = f; }
mapping query_timetable() { return timetable ? timetable : ([ ]); }
void set_timetable(mapping m) { timetable = m ? m : ([ ]); }
void add_entry(int hour, mapping entry) { timetable[hour] = entry; }

// The hours this NPC has something scheduled. The area uses these to build its
// hour index (which uuids to wake at each hour) without loading the NPC.
int * query_active_hours() { return map_indices(timetable); }

// Resolve an entry's "goto" symbol to a concrete location file.
// One of the points of interest of the area this NPC belongs to, at random:
// where somebody with nothing to do goes out to in the morning. Nil when the
// area marks none.
private string _somewhere_out(object npc)
{
  object area;
  string * places;

  if (!npc->query_npc_area_path())
    return nil;
  area = (object)load_object(AREA_HANDLER)->query_area(npc->query_npc_area_path());
  if (area && area->query_root_area())
    area = (object)area->query_root_area();
  if (!area)
    return nil;

  places = map_indices((mapping)area->query_pois());
  return sizeof(places) ? places[random(sizeof(places))] : nil;
}

/**
 * Renders a timetable line against the NPC, capitalised, as a leading $mcname$
 * expands lowercase. Nil when the entry carries no such line.
 */
private string _line(object npc, mixed text)
{
  if (!stringp(text) || !strlen(text))
    return nil;
  return capitalize(personalize_string(text, npc)) + "\n";
}

private string _resolve(object npc, string dest)
{
  if (dest == "work")
    return work;
  if (dest == "home")
    return npc->query_home();
  if (dest == "out")
    return _somewhere_out(npc);
  return dest;  // a literal location file (a guard's post)
}

// Whether the NPC should stay where it is rather than wander: the entry in
// effect sends it home and it is there.
int query_staying_put()
{
  object npc, here;
  string home;

  npc = query_entity();
  if (!npc || current_goto != "home")
    return 0;
  here = environment(npc);
  home = npc->query_home();
  return here && home && here->query_file_name() == home;
}

/**
 * Whether the NPC is on its rest shift: the entry in effect sends it home,
 * whether it is there yet or still on the way.
 */
int query_resting()
{
  return current_goto == "home";
}

// The entry governing `hour`: the latest one at or before it, wrapping past
// midnight, so an hour the timetable does not name is still covered by the one
// that last took effect. A routine of 6 -> work and 20 -> home therefore answers
// "work" for every hour from 6 to 19 and "home" for the rest.
private mapping _entry_in_effect(int hour)
{
  int * hours;
  int i, dist, best, best_dist;

  hours = map_indices(timetable);
  if (!sizeof(hours))
    return nil;

  best = -1;
  best_dist = 24;

  for (i = 0; i < sizeof(hours); i++)
  {
    dist = hour - hours[i];
    if (dist < 0)
      dist += 24;
    if (dist < best_dist)
    {
      best_dist = dist;
      best = hours[i];
    }
  }

  return best >= 0 ? timetable[best] : nil;
}

// Put a freshly materialized NPC back on its routine. A trip is never saved, so
// one that was walking when its location unloaded comes back standing wherever
// it got to, with no route and no goal, and the timetable would not speak to it
// again until the next hour it names -- half a day of standing in a field path.
// So on materialization ask which entry governs this hour and walk there.
void resume_schedule(int hour)
{
  object npc, here;
  mapping entry;
  string dest;

  npc = query_entity();
  if (!npc)
    return;

  entry = _entry_in_effect(hour);
  if (!entry)
    return;

  current_goto = entry["goto"];
  dest = _resolve(npc, entry["goto"]);
  if (!dest || !strlen(dest))
    return;

  here = environment(npc);
  if (here && here->query_file_name() == dest)
    return;

  // no leaving line: this is not the hour's departure that a room witnesses,
  // it is somebody quietly finishing a walk that was cut short. The arrival is
  // still seen where the walk ends.
  trip_dest = dest;
  trip_arrive = entry["arrive"];
  npc->travel_to(dest);
}

// The areas handler calls this on the NPC at hour H (forwarded here through
// npc.c::do_schedule). Look H up in the timetable and, if there is an entry,
// head to its destination unless already there.
void do_schedule(int hour)
{
  object npc, here;
  mapping entry;
  string dest;

  npc = query_entity();
  if (!npc)
    return;

  entry = timetable[hour];
  if (!entry)
    return;

  current_goto = entry["goto"];
  dest = _resolve(npc, entry["goto"]);
  if (!dest || !strlen(dest))
    return;

  // already at the destination this hour? nothing to do
  here = environment(npc);
  if (here && here->query_file_name() == dest)
    return;

  if (here && _line(npc, entry["leave"]))
    tell_room(here, _line(npc, entry["leave"]));

  trip_dest = dest;
  trip_arrive = entry["arrive"];
  npc->travel_to(dest);
}

/**
 * The NPC has moved (forwarded by npc.c). Reaching the end of the trip tells
 * its arrival line, once the steps still queued behind the move (closing the
 * door) have run.
 */
void event_moved(object ob, object from, object dest)
{
  object npc, here;

  npc = query_entity();
  here = npc ? environment(npc) : nil;
  if (!here || !trip_dest || here->query_file_name() != trip_dest)
    return;

  trip_dest = nil;
  arrival_line = trip_arrive;
  trip_arrive = nil;
  if (!arrival_line)
    return;

  arrive_tries = 5;
  call_out("announce_arrival", 1, here);
}

/**
 * Tells the arrival line in `where`, waiting a few beats for the NPC's queued
 * actions to run first. Dropped if the NPC has moved on meanwhile.
 */
void announce_arrival(object where)
{
  object npc;

  npc = query_entity();
  if (!npc || !where || environment(npc) != where || !arrival_line)
    return;

  if (npc->query_action_pending() && --arrive_tries > 0)
  {
    call_out("announce_arrival", 1, where);
    return;
  }

  tell_room(where, _line(npc, arrival_line));
  arrival_line = nil;
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
