
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

// Read a room's Z coordinate, or a sentinel (-99999) when the room
// has no coordinate at all (legacy room, fresh location, etc). Used
// to decide whether two rooms live on the same horizontal plane and
// can therefore share a 2D slice of the map.
#define CART_NO_Z (-99999)
private int _z_of(object room)
{
  int * coords;

  if (!room)
    return CART_NO_Z;

  coords = room->query_coordinates();
  if (!coords || sizeof(coords) < 3)
    return CART_NO_Z;

  return coords[2];
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
 *     "rooms":     object **,           grid[y][x] == room/location or nil
 *   ])
 *
 * Or nil if the viewer has no environment or the environment refuses
 * to render a map (dungeon / underwater rooms).
 *
 * The two grids are aligned: `rooms[y][x]` holds the room object that
 * produced `cells[y][x]` for room cells, and is `nil` for empty or
 * exit-segment cells. Renderers that only care about cell types ignore
 * `rooms`; renderers that need coordinates (coord overlay, future
 * pathfinding) read it.
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
  object ** rooms;
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

  // initialise the grids with empty cells
  cells = allocate(height);
  rooms = allocate(height);
  for (i = 0; i < height; i++)
  {
    cells[i] = allocate_int(width);
    rooms[i] = allocate(width);
  }

  viewer_x = width / 2;
  viewer_y = height / 2;

  // viewer's z plane — only neighbours at the same z are drawn. CART_NO_Z
  // means "we cannot tell" (legacy room without coordinates), in which
  // case we do not filter and accept the previous behaviour of mixing
  // levels (best effort with no information).
  {
    int viewer_z;
    viewer_z = _z_of(env);

    // BFS from the viewer's environment outward through exits
    pending = ({ });
    revised = ({ });

    current = ({ viewer_x, viewer_y, env });
    pending += ({ current });
    cells[viewer_y][viewer_x] = _classify_room(env, viewer, deep);
    rooms[viewer_y][viewer_x] = env;

    while (sizeof(pending))
    {
      string * dest_dir;

      current = pending[0];
      dest_dir = current[2]->query_dest_dir();

      for (i = 0; i < sizeof(dest_dir); i += 2)
      {
        object new_room;
        int seg_x, seg_y;            // exit-segment cell coordinates
        int dest_x, dest_y;          // destination cell coordinates
        int seg_type;
        int has_segment, has_dest;
        int cx, cy;

        cx = current[0];
        cy = current[1];

        seg_x = seg_y = dest_x = dest_y = seg_type = 0;
        has_segment = has_dest = 0;

        switch (dest_dir[i])
        {
          case DIR_NORTH:
            seg_x = cx;     seg_y = cy - 1;
            dest_x = cx;    dest_y = cy - 2;
            seg_type = CART_VERTICAL_EXIT;
            has_segment = (seg_y >= 0);
            has_dest    = (dest_y >= 0);
            break;
          case DIR_SOUTH:
            seg_x = cx;     seg_y = cy + 1;
            dest_x = cx;    dest_y = cy + 2;
            seg_type = CART_VERTICAL_EXIT;
            has_segment = (seg_y < height);
            has_dest    = (dest_y < height);
            break;
          case DIR_EAST:
            seg_x = cx + 1; seg_y = cy;
            dest_x = cx + 2; dest_y = cy;
            seg_type = CART_HORIZONTAL_EXIT;
            has_segment = (seg_x < width);
            has_dest    = (dest_x < width);
            break;
          case DIR_WEST:
            seg_x = cx - 1; seg_y = cy;
            dest_x = cx - 2; dest_y = cy;
            seg_type = CART_HORIZONTAL_EXIT;
            has_segment = (seg_x >= 0);
            has_dest    = (dest_x >= 0);
            break;
          case DIR_NORTHWEST:
            seg_x = cx - 1; seg_y = cy - 1;
            dest_x = cx - 2; dest_y = cy - 2;
            seg_type = CART_BACKSLASH_EXIT;
            has_segment = (seg_x >= 0 && seg_y >= 0);
            has_dest    = (dest_x >= 0 && dest_y >= 0);
            break;
          case DIR_NORTHEAST:
            seg_x = cx + 1; seg_y = cy - 1;
            dest_x = cx + 2; dest_y = cy - 2;
            seg_type = CART_SLASH_EXIT;
            has_segment = (seg_x < width && seg_y >= 0);
            has_dest    = (dest_x < width && dest_y >= 0);
            break;
          case DIR_SOUTHWEST:
            seg_x = cx - 1; seg_y = cy + 1;
            dest_x = cx - 2; dest_y = cy + 2;
            seg_type = CART_SLASH_EXIT;
            has_segment = (seg_x >= 0 && seg_y < height);
            has_dest    = (dest_x >= 0 && dest_y < height);
            break;
          case DIR_SOUTHEAST:
            seg_x = cx + 1; seg_y = cy + 1;
            dest_x = cx + 2; dest_y = cy + 2;
            seg_type = CART_BACKSLASH_EXIT;
            has_segment = (seg_x < width && seg_y < height);
            has_dest    = (dest_x < width && dest_y < height);
            break;
          default:
            // up / down / non-cardinal — handled by the source room's
            // CART_UP_ROOM / CART_DOWN_ROOM marker, no segment to draw
            continue;
        }

        // Resolve destination first; if it lives on a different z plane
        // we draw nothing (no segment, no destination cell). The source
        // room still carries its CART_UP_ROOM / CART_DOWN_ROOM marker
        // for any vertical exit, which is the level-transition hint.
        new_room = _resolve_destination(dest_dir[i + 1], deep);
        if (!new_room)
          continue;

        if (viewer_z != CART_NO_Z)
        {
          int dest_z;
          dest_z = _z_of(new_room);
          if (dest_z != CART_NO_Z && dest_z != viewer_z)
            continue;
        }

        // skip rooms we already placed
        {
          int repeated;
          repeated = 0;
          for (j = 0; j < sizeof(revised); j++)
            if (revised[j][2] == new_room) { repeated = 1; break; }
          if (repeated)
            continue;
        }

        if (has_segment)
          cells[seg_y][seg_x] = seg_type;

        if (has_dest)
        {
          cells[dest_y][dest_x] = _classify_room(new_room, viewer, deep);
          rooms[dest_y][dest_x] = new_room;
          pending += ({ ({ dest_x, dest_y, new_room }) });
        }
      }

      revised += ({ current });
      pending -= ({ current });
    }
  }

  return ([
    "width":    width,
    "height":   height,
    "viewer_x": viewer_x,
    "viewer_y": viewer_y,
    "cells":    cells,
    "rooms":    rooms,
  ]);
}

// ---------------------------------------------------------------------------
// Renderers
//
// Each renderer takes a map view (the mapping returned by
// query_map_view) and returns a multi-line string with the bare grid
// content — no parchment frame, no legend. The caller (the player
// `map` command, an admin debug verb, etc.) wraps that string in
// whatever decoration its UI demands.
// ---------------------------------------------------------------------------

/**
 * Default renderer: the chunky `[ ]` boxes with `---`, `|`, `/`, `\`
 * exit segments. This is the look the original `map` command had and
 * what the player sees today.
 */
string render_ascii(mapping view)
{
  int width, height;
  int ** cells;
  int vx, vy;
  string out, line;
  int i, j, type;

  width  = view["width"];
  height = view["height"];
  cells  = view["cells"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      type = cells[i][j];

      if (i == vy && j == vx)
      {
        if (type == CART_COAST_ROOM)
          line += "%^BLUE%^[%^ORANGE%^*%^BLUE%^]%^RESET%^";
        else
          line += "[%^ORANGE%^*%^RESET%^]";
        continue;
      }

      switch (type)
      {
        case CART_ROOM:               line += "[ ]";                                 break;
        case CART_COAST_ROOM:         line += "%^BLUE%^[ ]%^RESET%^";                break;
        case CART_DOOR_ROOM:          line += "[D]";                                 break;
        case CART_UP_ROOM:            line += "[^]";                                 break;
        case CART_DOWN_ROOM:          line += "[v]";                                 break;
        case CART_FINISH_QUEST_ROOM:  line += "[%^BOLD%^YELLOW%^?%^RESET%^]";        break;
        case CART_QUEST_ROOM:         line += "[%^BOLD%^YELLOW%^!%^RESET%^]";        break;
        case CART_ADVENTURER_ROOM:    line += "[%^BOLD%^CYAN%^*%^RESET%^]";          break;
        case CART_GUARD_ROOM:         line += "[%^BOLD%^GREEN%^*%^RESET%^]";         break;
        case CART_ENEMY_ROOM:         line += "[%^BOLD%^RED%^*%^RESET%^]";           break;
        case CART_HORIZONTAL_EXIT:    line += "---";                                 break;
        case CART_VERTICAL_EXIT:      line += " | ";                                 break;
        case CART_SLASH_EXIT:         line += " / ";                                 break;
        case CART_BACKSLASH_EXIT:     line += " \\ ";                                break;
        default:                      line += "   ";                                 break;
      }
    }
    out += line + "\n";
  }

  return out;
}

/**
 * Compact renderer: one character per cell. Three times denser, fits
 * a much larger viewport in the same screen real estate. Roguelike
 * feel.
 *
 *   '@'  viewer
 *   '.'  empty cell
 *   '#'  plain room
 *   'D'  door room      '^'  up exit       'v'  down exit
 *   '~'  coast          '?'  finished quest waiting to turn in
 *   '!'  open quest     '*'  friendly group / guard       'X'  enemy
 *   '|'  vertical exit  '-'  horizontal exit  '/'  slash  '\\' backslash
 */
string render_compact(mapping view)
{
  int width, height;
  int ** cells;
  int vx, vy;
  string out, line;
  int i, j, type;

  width  = view["width"];
  height = view["height"];
  cells  = view["cells"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      type = cells[i][j];

      if (i == vy && j == vx)
      {
        line += "%^ORANGE%^@%^RESET%^";
        continue;
      }

      switch (type)
      {
        case CART_ROOM:               line += "#";                                   break;
        case CART_COAST_ROOM:         line += "%^BLUE%^~%^RESET%^";                  break;
        case CART_DOOR_ROOM:          line += "D";                                   break;
        case CART_UP_ROOM:            line += "^";                                   break;
        case CART_DOWN_ROOM:          line += "v";                                   break;
        case CART_FINISH_QUEST_ROOM:  line += "%^BOLD%^YELLOW%^?%^RESET%^";          break;
        case CART_QUEST_ROOM:         line += "%^BOLD%^YELLOW%^!%^RESET%^";          break;
        case CART_ADVENTURER_ROOM:    line += "%^BOLD%^CYAN%^*%^RESET%^";            break;
        case CART_GUARD_ROOM:         line += "%^BOLD%^GREEN%^*%^RESET%^";           break;
        case CART_ENEMY_ROOM:         line += "%^BOLD%^RED%^X%^RESET%^";             break;
        case CART_HORIZONTAL_EXIT:    line += "-";                                   break;
        case CART_VERTICAL_EXIT:      line += "|";                                   break;
        case CART_SLASH_EXIT:         line += "/";                                   break;
        case CART_BACKSLASH_EXIT:     line += "\\";                                  break;
        default:                      line += ".";                                   break;
      }
    }
    out += line + "\n";
  }

  return out;
}

/**
 * Coordinate overlay renderer: shows the world `(x,y)` of each
 * room/location cell instead of an opaque glyph. Intended for coders
 * and admins debugging conversions, sector indexes, or inferred
 * coordinates — not for player consumption. Cells without a backing
 * room (empty space, exit segments) are blank.
 *
 * Each room cell is rendered as a fixed-width "(x,y)" tag. The viewer
 * cell shows "(*x,y)" with the leading asterisk so it stays findable
 * in the grid.
 */
string render_coords(mapping view)
{
  int width, height;
  int ** cells;
  object ** rooms;
  int vx, vy;
  string out, line;
  int i, j;

  width  = view["width"];
  height = view["height"];
  cells  = view["cells"];
  rooms  = view["rooms"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      object room;
      int * coords;
      string tag;
      int is_viewer;

      room = rooms[i][j];
      is_viewer = (i == vy && j == vx);

      if (!room)
      {
        // exit-segment cells get a small marker too so the topology is
        // still readable without needing the room glyphs
        switch (cells[i][j])
        {
          case CART_HORIZONTAL_EXIT:  line += "  ----  "; break;
          case CART_VERTICAL_EXIT:    line += "    |   "; break;
          case CART_SLASH_EXIT:       line += "    /   "; break;
          case CART_BACKSLASH_EXIT:   line += "    \\   "; break;
          default:                    line += "        "; break;
        }
        continue;
      }

      coords = room->query_coordinates();
      if (!coords || sizeof(coords) < 2)
        tag = (is_viewer ? "(*?,?)" : "(?, ?) ");
      else
        tag = (is_viewer
               ? sprintf("(*%d,%d)", coords[0], coords[1])
               : sprintf("(%d,%d) ", coords[0], coords[1]));

      // pad/truncate to a fixed 8-char column so columns stay aligned
      line += sprintf("%-8s", tag);
    }
    out += line + "\n";
  }

  return out;
}
