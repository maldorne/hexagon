// Schedule component. Holds an NPC's daily routine as a timetable keyed on the
// game hour (0-23) and acts on it when told to. The areas handler decides which
// NPCs must act at each hour (from the area census) and calls do_schedule(hour)
// on them; the NPC then reads its own timetable and walks where it should be. So
// the area is the dispatcher ("your turn"), the NPC decides what to do and how.
//
//   timetable: ([ hour(0-23) : ([ "goto": "work" | "home" | <location file> ]) ])
//
// "work" resolves to this NPC's work location, "home" to its home (read live, a
// family shares it and it can change); a literal file lets a guard rotate posts.
// The walk is the NPC's paced, interruptible travel (travel_to) -- combat or
// conversation abandons the trip, and the next matching hour re-issues it.

inherit component "/lib/npc/component.c";

// The hour this NPC works from, resolving the "work" symbol in the timetable.
// Home is not stored here -- it lives on the NPC (query_home).
string work;
// The routine: game hour -> what to do that hour (see the header).
mapping timetable;

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
private string _resolve(object npc, string dest)
{
  if (dest == "work")
    return work;
  if (dest == "home")
    return npc->query_home();
  return dest;  // a literal location file (a guard's post)
}

// The areas handler calls this on the NPC at hour H (forwarded here through
// npc.c::do_schedule). Look H up in the timetable and, if there is an entry,
// head to its destination unless already there. args = ({ hour }).
void do_schedule(mixed * args)
{
  object npc, here;
  int hour;
  mapping entry;
  string dest;

  npc = query_owner();
  if (!npc || !args || !sizeof(args))
    return;

  hour = args[0];
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

  // Optional flavour line: a builder-authored message on the entry, written in
  // the mud's language (like a POI label -- no lang file), announced to the room
  // as the NPC sets off. Absent on most entries; the departure is silent then.
  //
  // The message may carry personalization wildcards ($mcname$ = this NPC's
  // kind/name, $mpronoun$/$mposs$/$mobj$, $mvocal$ = the o/a vowel for gender
  // agreement, ...), resolved against THIS npc by the personalize_string efun --
  // so one generic message reads correctly for a male or a female citizen
  // instead of a fixed, possibly mis-gendered line. Capitalise the result, as a
  // leading $mcname$ expands lowercase.
  if (here && stringp(entry["msg"]) && strlen(entry["msg"]))
    tell_room(here, capitalize(personalize_string(entry["msg"], npc)) + "\n");

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
