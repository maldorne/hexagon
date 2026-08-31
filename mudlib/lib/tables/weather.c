// Climate zones: the shared base every game's table inherits.
//
// A zone is a stretch of world that shares one sky. This file holds the
// machinery -- how a place is matched to a zone, and how the handler asks for
// the data -- so a game's own table is nothing but the data:
//
//     inherit "/lib/tables/weather";
//
//     #define MARSHES "marshes"
//
//     void create()
//     {
//       ::create();
//       set_zones(([ MARSHES : ({ 60, 40, 30, 60, 40, 30, ({ }) }) ]));
//       set_areas(([ "areas/<area>" : MARSHES ]));
//     }
//
// A zone reads
//
//   ([ zone : ({ rain, wind, temperature,
//                rain average, wind average, temperature average,
//                ({ adjacent zones }) }) ])
//
// where 0 is the best weather and 100 the worst (more rain, more wind, colder).
// The three current values start equal to the three averages: the handler takes
// those as the resting point it drifts back towards. Adjacency is what lets a
// front travel between zones; a zone with no neighbours keeps its weather to
// itself.
//
// Zone names are internal keys, never shown to a player, so they are plain
// English ids like every other identifier in the mudlib.

// The zone a game answers with until it declares one of its own, and the one
// anywhere unplaced falls back to.
#define BASE "hexagon"

private mapping zones;
// Which zone each area belongs to, by game-relative directory. An entry covers
// everything beneath it, so "areas/<area>" also answers for its rooms, its npcs
// and any sub-area.
private mapping areas;

void create()
{
  zones = ([
    BASE : ({ 30, 30, 30,
              30, 30, 30,
              ({ }),
            }),
  ]);

  areas = ([ ]);
}

// A game's table declares its data from create(), after ::create().
static void set_zones(mapping m) { zones = m ? m : ([ ]); }
static void set_areas(mapping m) { areas = m ? m : ([ ]); }

// The zone a game-relative directory belongs to, walking up until a declared
// area answers. The handler passes the directory of the room or location
// asking, so "areas/<area>/rooms/<part>" is answered by an "areas/<area>" entry.
//
// The same directory serves a room and the location it was converted into: the
// handler reduces both to one key, so a game that never converts, or one with
// no locations at all, declares its zones the same way.
string query_zone(string dir)
{
  string * parts;

  if (!dir || !strlen(dir))
    return BASE;

  parts = explode(dir, "/") - ({ "" });

  while (sizeof(parts))
  {
    string where;

    where = implode(parts, "/");
    if (areas[where])
      return areas[where];

    if (sizeof(parts) == 1)
      break;
    parts = parts[0 .. sizeof(parts) - 2];
  }

  // Anywhere not on the map shares the default sky rather than having none.
  return BASE;
}

// The complete zone data, for the weather handler.
mixed query_zones()
{
  call_out("dest_me", 1);
  return zones;
}
