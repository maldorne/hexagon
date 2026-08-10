/*
 * Population handler — keeps every area's NPC population topped up.
 *
 * Same shape as /lib/handlers/ventures.c (a registry walked one item per cron
 * tick), but it works at the DATA level: it never loads a location. For the
 * current area it compares each roster blueprint's area cap against the live
 * census and, for any deficit, assigns new NPCs to random locations of the
 * area (area::assign_npc records a census entry, no object materialized). The
 * NPC becomes real when its location loads (area::restore_location_npcs). A
 * death frees a census slot, so the next sweep refills it -- somewhere else.
 *
 * Called from /lib/handlers/cron.c via the crontab (update_population).
 */

#include <mud/config.h>
#include <areas/area.h>

inherit "/lib/core/object.c";

#define SAVE_FILE "/save/population"

// how many NPCs to assign in a single sweep of one area, so a large deficit
// fills over several cron cycles instead of in one heavy tick
#define ASSIGN_PER_TICK 10

// area paths (the save-tree directory of each registered area)
string * areas;
int next_area;

string * query_areas() { return areas; }

void create()
{
  areas = ({ });
  next_area = 0;

  restore_object(SAVE_FILE, 1);

  ::create();
}

void setup()
{
  // anticloning, like the other handlers
  if (file_name(this_object()) != POPULATION_HANDLER)
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

void save_handler() { save_object(SAVE_FILE, 1); }

void dest_me()
{
  save_object(SAVE_FILE, 1);
  ::dest_me();
}

// Register an area to be swept. Called by area::set_location_npc_sources once
// the area has a roster.
int include_area(string area_path)
{
  if (!area_path || !strlen(area_path))
    return 0;

  if (member_array(area_path, areas) == -1)
  {
    areas += ({ area_path });
    save_handler();
  }

  return 1;
}

// One sweep: top up the next area toward its caps. Round-robins through the
// registry (one area per call, like the ventures handler).
int update_population()
{
  object area;
  string area_path;
  string * sources, * locs;
  mapping intended;
  int i, assigned;

  if (!sizeof(areas))
    return 0;

  if (next_area >= sizeof(areas))
    next_area = 0;

  area_path = areas[next_area];
  next_area++;

  // query_area (not create_area) so a registered area whose area.o has been
  // deleted resolves to nil and is dropped, rather than being silently
  // recreated as an empty area by the sweep.
  area = load_object(AREA_HANDLER)->query_area(area_path);
  if (!area)
  {
    // area gone -- drop it from the registry
    areas -= ({ area_path });
    save_handler();
    return 0;
  }

  intended = area->query_npc_intended();
  locs = map_indices(area->query_locations());
  if (!map_sizeof(intended) || !sizeof(locs))
  {
    save_handler();
    return 0;
  }

  sources = map_indices(intended);
  assigned = 0;

  for (i = 0; i < sizeof(sources) && assigned < ASSIGN_PER_TICK; i++)
  {
    int cap, deficit, j;

    cap = intended[sources[i]]["max"];
    deficit = cap - area->query_npc_live_count(sources[i]);

    for (j = 0; j < deficit && assigned < ASSIGN_PER_TICK; j++)
    {
      area->assign_npc(sources[i], locs[random(sizeof(locs))]);
      assigned++;
    }
  }

  save_handler();
  return 1;
}
