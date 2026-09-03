
// Daily schedules: which of the area's NPCs act at each game hour.
//
// The area does not decide what an NPC does -- it only knows who is due. The
// areas handler asks for the uuids scheduled at the current hour and releases
// them staggered; each one is woken here (materialized at its census position
// if it was unloaded) and then told the hour, so the NPC can act on its own
// timetable. Keeping the index on the area is what lets a whole settlement's
// routine be dispatched without loading a single NPC.
//
// The index looks after itself. Everything that knows an NPC has stopped being
// due says so (index_schedule_hours with no hours: it dies, it leaves the
// census, its type stops naming hours), but nothing outside has to walk it to
// keep it honest: an entry that turns out to be unusable when its hour comes --
// no census row left, no position, a position that no longer loads -- is
// dropped there and then. So a room that is cleaned away, or a savefile edited
// by hand, needs no notification of any kind; the round finds it and the index
// settles by itself, one wasted wake per stale entry and no more.

#include <room/location.h>
#include <areas/area.h>
#include <areas/vacancy.h>

// Which census NPCs have something scheduled at each game hour, so the areas
// handler can wake and dispatch exactly the ones due without scanning or loading
// the rest: ([ hour(0-23) : ({ uuids }) ]). Populated when a schedule is
// attached; the destination itself is read live from the NPC when it acts.
mapping schedule_index;


void create()
{
  schedule_index = ([ ]);
}

// Record which game hours a scheduled NPC acts, keyed by its uuid, so the areas
// handler can find exactly who is due at an hour without loading the census's
// NPCs. Called when a schedule component is attached (see npc_restore), and
// with no hours at all when one is taken away -- an NPC that dies, leaves the
// census or stops keeping a timetable. The destination is not stored here -- it
// is read live from the NPC when it acts.
//
// `hours` is the whole truth about that uuid, not an addition to it: it is
// dropped from every hour the list does not name. An index that only ever grew
// would keep waking the dead.
void index_schedule_hours(string uuid, int * hours)
{
  int * known;
  int i, changed;

  if (!schedule_index)
    schedule_index = ([ ]);

  known = map_indices(schedule_index);
  for (i = 0; i < sizeof(known); i++)
  {
    if (member_array(known[i], hours) >= 0)
      continue;
    if (member_array(uuid, schedule_index[known[i]]) < 0)
      continue;

    schedule_index[known[i]] -= ({ uuid });
    if (!sizeof(schedule_index[known[i]]))
      map_delete(schedule_index, known[i]);
    changed = 1;
  }

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
  {
    this_object()->save_me();

    // and tell the areas handler at which hours we have somebody due, so its
    // hourly round restores only the areas that have work. It gets our whole
    // set, not the hours of this one NPC: the last one to leave an hour is what
    // takes the area out of it.
    AREA_HANDLER->note_schedule_hours(
      (string)this_object()->query_area_path(), map_indices(schedule_index));
  }
}

// The current game hour for this area, read from its game's weather handler:
// each game keeps its own clock, and a game with no handler of its own falls
// back to the base weather.
int query_game_hour()
{
  string game, wpath;

  game = game_from_path((string)this_object()->query_area_path());
  wpath = "/games/" + game + "/handlers/weather";
  if (file_size(wpath + ".c") < 0)
    wpath = "/lib/handlers/weather";

  return load_object(wpath)->query_date_data()[0];
}

// The census uuids with something scheduled at `hour` (loaded or not).
string * hour_actor_uuids(int hour)
{
  return (schedule_index && schedule_index[hour]) ? schedule_index[hour] : ({ });
}

// A live NPC of this area by uuid, or nil if it is not currently materialized.
// The areas handler calls it on each loaded area to build a world-wide lookup
// (find_live_npc).
object live_npc(string uuid)
{
  object * live;
  int i;

  live = (object *)this_object()->query_live_npcs();
  for (i = 0; i < sizeof(live); i++)
    if (live[i] && live[i]->query_npc_uuid() == uuid)
      return live[i];
  return nil;
}

// Wake a scheduled NPC and hand it its hour: if it is already in the world use
// it as is, otherwise materialize it (and its location) at its census position;
// then call do_schedule so it acts on its own timetable. The areas handler calls
// this, staggered, for each uuid due this hour. Checking "already live" first
// avoids cloning a duplicate when the NPC has wandered off its census position.
// This is also where the index cleans itself: a uuid that cannot be woken -- no
// census row, no position, a position that no longer loads -- comes out of it
// here, because trying again next hour would fail the same way.
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
    {
      index_schedule_hours(uuid, ({ }));
      return;
    }
    // no position, or one that no longer loads: it cannot be woken, and trying
    // again every hour will not change that, so it comes out of the index. The
    // census row stays -- whoever placed it is who gets to place it again.
    locfile = entry[CENSUS_LOCATION];
    if (!locfile || !strlen(locfile))
    {
      index_schedule_hours(uuid, ({ }));
      return;
    }

    // load its census-position location and materialize the census NPCs there
    // (idempotent) so an unloaded NPC comes back before it acts
    loc = load_object(LOCATION_HANDLER)->load_location(locfile);
    if (!loc)
    {
      index_schedule_hours(uuid, ({ }));
      return;
    }
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
