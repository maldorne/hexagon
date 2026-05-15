
/*
 * Cartography handler.
 *
 * Builds a presentation-agnostic map view around a viewer object.
 * Consumers (the player `map` command, future web/JSON renderers,
 * pathfinding, etc.) ask for the structured grid here and render it
 * however they like.
 */

#include <cartography.h>
#include <translations/exits.h>
#include <room/location.h>

// Compute the highest-priority cell type for `room` from the viewer's
// perspective. In `deep` mode the function inspects the room's full
// inventory (which can be expensive — for legacy rooms, walking the
// inventory may force NPCs to spawn). In lazy mode (`deep == 0`) the
// inventory pass is skipped on rooms that are not already loaded with
// live contents; topology and door/up/down/coast still resolve, but
// per-room markers (quest, enemy, guard, ...) only show up for rooms
// the viewer has already activated.
private int _classify_room(object room, object viewer, int deep)
{
  object * inv;
  object * enemies;
  string * exits;
  int i;

  if (!room)
    return CART_EMPTY;

  // inventory-driven markers: skip in lazy mode for rooms whose
  // contents are not already populated. We approximate "populated" by
  // checking whether anything sits in the inventory at all — for an
  // unvisited room with no clones yet this returns ({ }), and we skip
  // the marker pass without forcing a load.
  inv = all_inventory(room);

  if (deep || sizeof(inv))
  {
    enemies = viewer->query_attacker_list() + viewer->query_call_outed();

    for (i = 0; i < sizeof(inv); i++)
    {
      if ((member_array(inv[i], enemies) != -1) && !inv[i]->query_hidden())
        return CART_ENEMY_ROOM;
      if (inv[i]->query_quest_object() && inv[i]->check_player_finished(viewer))
        return CART_FINISH_QUEST_ROOM;
      if (inv[i]->query_quest_object() && inv[i]->check_player(viewer))
        return CART_QUEST_ROOM;
      if (viewer->query_adventurer() &&
          (viewer->query_adventurer_group_name() ==
           inv[i]->query_adventurer_group_name()))
        return CART_ADVENTURER_ROOM;
      if (inv[i]->query_guard() &&
          (inv[i]->query_citizenship() == viewer->query_citizenship()))
        return CART_GUARD_ROOM;
    }
  }

  // structural markers: cheap, depend only on the room's own data.
  exits = room->query_direc();

  for (i = 0; i < sizeof(exits); i++)
  {
    if (member_array(room->query_ex_type(exits[i]),
                     ({ "door", "gate" })) != -1)
      return CART_DOOR_ROOM;
    if (exits[i] == DIR_UP)
      return CART_UP_ROOM;
    if (exits[i] == DIR_DOWN)
      return CART_DOWN_ROOM;
  }

  if (room->query_coast())
    return CART_COAST_ROOM;

  return CART_ROOM;
}

// Resolve an exit destination string to a room/location object. In
// lazy mode (`deep == 0`) only objects already in memory are returned;
// in deep mode the destination is loaded on demand. Locations live as
// `.o` files so they always go through LOCATION_HANDLER.
private object _resolve_destination(string dest, int deep)
{
  object ob;

  if (!dest || !strlen(dest))
    return nil;

  if (dest[strlen(dest) - 2 ..] == ".o")
    return load_object(LOCATION_HANDLER)->load_location(dest);

  ob = find_object(dest);
  if (ob)
    return ob;

  if (deep)
    catch(ob = load_object(dest));

  return ob;
}

/**
 * Build the map view for `viewer`. Returns a mapping shaped:
 *
 *   ([
 *     "width":     int,                 viewport width in cells
 *     "height":    int,                 viewport height in cells
 *     "viewer_x":  int,                 viewer column inside the grid
 *     "viewer_y":  int,                 viewer row inside the grid
 *     "cells":     int **,              grid[y][x] == one of CART_*
 *   ])
 *
 * Or nil if the viewer has no environment or the environment refuses
 * to render a map (dungeon / underwater rooms).
 *
 * `options` (mapping, all optional):
 *   "deep":   1 to force loading every reachable room (slow on legacy
 *             room-based areas; safe on locations). Default 0.
 *   "width":  viewport width in cells. Default CART_DEFAULT_WIDTH.
 *   "height": viewport height in cells. Default CART_DEFAULT_HEIGHT.
 */
mapping query_map_view(object viewer, varargs mapping options)
{
  object env;
  int deep, width, height;
  int ** cells;
  mixed * pending;
  mixed * revised;
  mixed current;
  int viewer_x, viewer_y;
  int i, j;

  if (!viewer)
    return nil;

  env = environment(viewer);
  if (!env)
    return nil;

  if (env->query_dungeon_room() || env->query_water_environment())
    return nil;

  if (!options)
    options = ([ ]);

  deep   = (intp(options["deep"])   ? options["deep"]   : 0);
  width  = (intp(options["width"])  ? options["width"]  : CART_DEFAULT_WIDTH);
  height = (intp(options["height"]) ? options["height"] : CART_DEFAULT_HEIGHT);

  // initialise the grid with empty cells
  cells = allocate(height);
  for (i = 0; i < height; i++)
    cells[i] = allocate_int(width);

  viewer_x = width / 2;
  viewer_y = height / 2;

  // BFS from the viewer's environment outward through exits
  pending = ({ });
  revised = ({ });

  current = ({ viewer_x, viewer_y, env });
  pending += ({ current });
  cells[viewer_y][viewer_x] = _classify_room(env, viewer, deep);

  while (sizeof(pending))
  {
    string * dest_dir;

    current = pending[0];
    dest_dir = current[2]->query_dest_dir();

    for (i = 0; i < sizeof(dest_dir); i += 2)
    {
      object new_room;
      int add, x, y;
      int cx, cy;

      cx = current[0];
      cy = current[1];
      add = 0;

      switch (dest_dir[i])
      {
        case DIR_NORTH:
          if (cy - 1 >= 0)
            cells[cy - 1][cx] = CART_VERTICAL_EXIT;
          if (cy - 2 >= 0) { add = 1; x = cx;     y = cy - 2; }
          break;
        case DIR_SOUTH:
          if (cy + 1 < height)
            cells[cy + 1][cx] = CART_VERTICAL_EXIT;
          if (cy + 2 < height) { add = 1; x = cx;     y = cy + 2; }
          break;
        case DIR_EAST:
          if (cx + 1 < width)
            cells[cy][cx + 1] = CART_HORIZONTAL_EXIT;
          if (cx + 2 < width) { add = 1; x = cx + 2; y = cy;     }
          break;
        case DIR_WEST:
          if (cx - 1 >= 0)
            cells[cy][cx - 1] = CART_HORIZONTAL_EXIT;
          if (cx - 2 >= 0) { add = 1; x = cx - 2; y = cy;     }
          break;
        case DIR_NORTHWEST:
          if (cx - 1 >= 0 && cy - 1 >= 0)
            cells[cy - 1][cx - 1] = CART_BACKSLASH_EXIT;
          if (cx - 2 >= 0 && cy - 2 >= 0) { add = 1; x = cx - 2; y = cy - 2; }
          break;
        case DIR_NORTHEAST:
          if (cx + 1 < width && cy - 1 >= 0)
            cells[cy - 1][cx + 1] = CART_SLASH_EXIT;
          if (cx + 2 < width && cy - 2 >= 0) { add = 1; x = cx + 2; y = cy - 2; }
          break;
        case DIR_SOUTHWEST:
          if (cx - 1 >= 0 && cy + 1 < height)
            cells[cy + 1][cx - 1] = CART_SLASH_EXIT;
          if (cx - 2 >= 0 && cy + 2 < height) { add = 1; x = cx - 2; y = cy + 2; }
          break;
        case DIR_SOUTHEAST:
          if (cx + 1 < width && cy + 1 < height)
            cells[cy + 1][cx + 1] = CART_BACKSLASH_EXIT;
          if (cx + 2 < width && cy + 2 < height) { add = 1; x = cx + 2; y = cy + 2; }
          break;
        default:
          break;
      }

      if (!add)
        continue;

      new_room = _resolve_destination(dest_dir[i + 1], deep);
      if (!new_room)
        continue;

      // skip rooms we already placed
      {
        int repeated;
        repeated = 0;
        for (j = 0; j < sizeof(revised); j++)
          if (revised[j][2] == new_room) { repeated = 1; break; }
        if (repeated)
          continue;
      }

      pending += ({ ({ x, y, new_room }) });
      cells[y][x] = _classify_room(new_room, viewer, deep);
    }

    revised += ({ current });
    pending -= ({ current });
  }

  return ([
    "width":    width,
    "height":   height,
    "viewer_x": viewer_x,
    "viewer_y": viewer_y,
    "cells":    cells,
  ]);
}
