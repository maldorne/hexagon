
// Daily schedules: which of the area's NPCs act at each game hour.
//
// The area does not decide what an NPC does -- it only knows who is due. The
// areas handler asks for the uuids scheduled at the current hour and releases
// them staggered; each one is woken here (materialized at its census position
// if it was unloaded) and then told the hour, so the NPC can act on its own
// timetable. Keeping the index on the area is what lets a whole settlement's
// routine be dispatched without loading a single NPC.
//
// Inherited by /lib/location/area.c. Variables cannot be shared upward between
// inherited files, so what this file needs from the area is reached through the
// accessors prototyped below and resolved by the inheriting object.

#include <room/location.h>
#include <areas/area.h>

// Which census NPCs have something scheduled at each game hour, so the areas
// handler can wake and dispatch exactly the ones due without scanning or loading
// the rest: ([ hour(0-23) : ({ uuids }) ]). Populated when a schedule is
// attached; the destination itself is read live from the NPC when it acts.
mapping schedule_index;

// Calls into the rest of the area go through this_object(): an area is a single
// object carrying the whole inheritance tree, so the call resolves at run time
// against the complete program. That avoids declaring prototypes here for
// functions that live in a sibling file. Only public functions are reachable
// this way, and the result comes back as mixed, hence the casts.

void create()
{
  schedule_index = ([ ]);
}

// Record which game hours a scheduled NPC acts, keyed by its uuid, so the areas
// handler can find exactly who is due at an hour without loading the census's
// NPCs. Called when a schedule component is attached (see npc_restore). The
// destination is not stored here -- it is read live from the NPC when it acts.
void index_schedule_hours(string uuid, int * hours)
{
  int i, changed;

  if (!schedule_index)
    schedule_index = ([ ]);

  for (i = 0; i < sizeof(hours); i++)
  {
    if (!schedule_index[hours[i]])
      schedule_index[hours[i]] = ({ });
    if (member_array(uuid, schedule_index[hours[i]]) < 0)
    {
      schedule_index[hours[i]] += ({ uuid });
      changed = 1;
    }
  }

  if (changed)
    this_object()->save_me();
}

// The census uuids with something scheduled at `hour` (loaded or not).
string * hour_actor_uuids(int hour)
{
  return (schedule_index && schedule_index[hour]) ? schedule_index[hour] : ({ });
}

// A live NPC of this area by uuid, or nil if it is not currently materialized.
private object _live_npc_by_uuid(string uuid)
{
  object * live;
  int i;

  live = (object *)this_object()->query_live_npcs();
  for (i = 0; i < sizeof(live); i++)
    if (live[i] && live[i]->query_npc_uuid() == uuid)
      return live[i];
  return nil;
}

// Public wrapper: this area's live NPC with `uuid`, or nil. The areas handler
// calls it on each loaded area to build a world-wide lookup (find_live_npc).
object live_npc(string uuid) { return _live_npc_by_uuid(uuid); }

// Wake a scheduled NPC and hand it its hour: if it is already in the world use
// it as is, otherwise materialize it (and its location) at its census position;
// then call do_schedule so it acts on its own timetable. The areas handler calls
// this, staggered, for each uuid due this hour. Checking "already live" first
// avoids cloning a duplicate when the NPC has wandered off its census position. A
// stale index entry (uuid no longer in the census) is skipped.
void wake_and_schedule(string uuid, int hour)
{
  mapping entry;
  string locfile;
  object loc, npc;
  object * inv;
  int i;

  // already materialized somewhere in the world -- use it, do not clone another
  // (a roamer may have wandered into another area, so the lookup is global)
  npc = AREA_HANDLER->find_live_npc(uuid);

  if (!npc)
  {
    entry = ((mapping)this_object()->query_npc_census())[uuid];
    if (!entry)
      return;
    locfile = entry["location"];
    if (!locfile || !strlen(locfile))
      return;

    // load its census-position location and materialize the census NPCs there
    // (idempotent) so an unloaded NPC comes back before it acts
    loc = load_object(LOCATION_HANDLER)->load_location(locfile);
    if (!loc)
      return;
    this_object()->restore_location_npcs(loc);

    inv = all_inventory(loc);
    for (i = 0; i < sizeof(inv); i++)
      if (inv[i] && inv[i]->query_npc() && inv[i]->query_npc_uuid() == uuid)
      {
        npc = inv[i];
        break;
      }
  }

  if (npc)
    npc->do_schedule(hour);
}
