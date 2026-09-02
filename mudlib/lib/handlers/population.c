/*
 * Population handler — keeps every area's fauna topped up.
 *
 * One area per cron tick, at the DATA level: it never loads a location. For the
 * area whose turn it is, it compares each roster blueprint's cap against the
 * live count and, for any deficit, assigns new NPCs to random locations of the
 * area (area::assign_monster bumps a bucket, no object materialized). The NPC
 * becomes real when its location loads (area::restore_location_npcs). A death
 * frees a slot, so the next sweep refills it -- somewhere else.
 *
 * Only the anonymous half of the population is swept. People are staffed one by
 * one by their settlement and never scattered statistically.
 *
 * It keeps no register of its own: which areas exist is the areas handler's to
 * answer, read off the tree of the game this handler belongs to. Every game has
 * its own -- /games/<game>/handlers/population.c inherits this one and needs to
 * override nothing, since its own path says which world it sweeps.
 *
 * Called from the crontab, one line per game (update_population).
 */

#include <mud/config.h>
#include <areas/area.h>

inherit "/lib/core/object.c";

// how many NPCs to assign in a single sweep of one area, so a large deficit
// fills over several cron cycles instead of in one heavy tick
#define ASSIGN_PER_TICK 10

// Whose turn it is in the game's list of areas. Static: it is a position in a
// round, not something worth remembering across a reboot -- and it starts
// somewhere at random, so the same area is not always the first to be topped up
// after a restart.
static int next_area;

// The world this handler sweeps, which is the game its own file belongs to. The
// shared one belongs to none and sweeps nothing.
string query_game()
{
  return game_from_path(file_name(this_object()));
}

string * query_areas()
{
  return (string *)load_object(AREA_HANDLER)->query_area_paths(query_game());
}

void create()
{
  string * areas;

  areas = query_areas();
  next_area = sizeof(areas) ? random(sizeof(areas)) : 0;

  ::create();
}

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance loaded through handler() should
  // exist. The path is not compared, so a game's own subclass passes; any
  // actual clone (file_name suffixed with #N) is destroyed.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
  {
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }

  reset_get();
  set_name("population handler");
  set_short("Population handler");
  set_long("This is the NPC population handler!\n");
  add_alias("handler");
  add_plural("handlers");

  move("bing");
}

int move(mixed dest, varargs mixed messin, mixed messout)
{
  ::move(HANDLERS_HOME, messin, messout);
}

// One sweep: top up the next area of this game toward its caps. One area per
// call, taking them in turn.
int update_population()
{
  object area;
  string area_path;
  string * areas, * sources, * locs;
  mapping caps;
  int i, assigned;

  areas = query_areas();
  if (!sizeof(areas))
    return 0;

  if (next_area >= sizeof(areas))
    next_area = 0;

  area_path = areas[next_area];
  next_area++;

  // query_area (not create_area) so a path whose area.o has gone resolves to
  // nil rather than being silently recreated as an empty area by the sweep.
  area = load_object(AREA_HANDLER)->query_area(area_path);
  if (!area)
    return 0;

  caps = area->query_npc_caps();
  locs = map_indices(area->query_locations());
  if (!map_sizeof(caps) || !sizeof(locs))
    return 0;

  // only the anonymous half of the population is swept; citizens come from
  // their settlement's role board, never from a statistical topup
  sources = area->query_monster_sources();
  assigned = 0;

  for (i = 0; i < sizeof(sources) && assigned < ASSIGN_PER_TICK; i++)
  {
    mapping clones_here;
    string * files;
    int cap, deficit, j;

    // where this source was declared when the rooms were converted, and how
    // many. The caps are shared across a community, so an area tops up only
    // what its own locations asked for -- otherwise a town would sprinkle the
    // road's travellers through its streets, and every area of the community
    // would try to fill the whole count. A source with no provenance here
    // (added by hand, or declared in a sibling area) is left to whoever has it.
    clones_here = area->query_original_clone_counts(sources[i]);
    files = map_indices(clones_here);
    if (!sizeof(files))
      continue;

    cap = 0;
    for (j = 0; j < sizeof(files); j++)
      cap += clones_here[files[j]];

    deficit = cap - area->query_monster_live_count(sources[i]);

    for (j = 0; j < deficit && assigned < ASSIGN_PER_TICK; j++)
    {
      area->assign_monster(sources[i], files[random(sizeof(files))]);
      assigned++;
    }
  }

  return 1;
}
