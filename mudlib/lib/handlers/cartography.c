
/*
 * Cartography handler.
 *
 * Builds a presentation-agnostic map view around a viewer object.
 * Consumers (the player `map` command, future web/JSON renderers,
 * pathfinding, etc.) ask for the structured grid here and render it
 * however they like.
 */

#include <cartography.h>
#include <living/quests.h>
#include <translations/exits.h>
#include <room/location.h>

// Compute the highest-priority type for the location `room` from the viewer's
// perspective. In `deep` mode the function inspects the room's full
// inventory for per-room markers (quest, enemy, guard, ...). Walking the
// inventory here is a pure read and never spawns anything; any NPC
// population is a side effect of a room being LOADED for the first time
// (its create/reset), which happens in _resolve_destination when a cold
// room is pulled in — not here, and never for a room already resident.
// In lazy mode (`deep == 0`) the inventory pass is skipped on rooms with
// no live contents, so markers only show for rooms already populated;
// topology and door/up/down/coast still resolve.
private int _classify_room(object room, object viewer, int deep)
{
  object * inv;
  object * enemies;
  object giver;
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
      // the quest handler says who answers for whatever is standing here: the
      // creature itself, or the component it carries
      giver = handler(QUESTS_HANDLER, viewer)->giver_of(inv[i]);
      if (giver && giver->check_player_finished(viewer))
        return CART_FINISH_QUEST_ROOM;
      if (giver && giver->check_player(viewer))
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

  // a dwelling (a location carrying a home component) shows as a house,
  // whatever its exits. Non-locations return nil here and fall through.
  if (room->query_component_by_type("home"))
    return CART_HOME_ROOM;

  // structural markers: cheap, depend only on the room's own data.
  exits = room->query_direc();

  // a door is no longer marked on the room; it crosses the exit segment
  // instead (see the view builder's door-segment upgrade), so only up / down
  // remain as room-level structural markers here.
  for (i = 0; i < sizeof(exits); i++)
  {
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
 *     "width":     int,                 viewport width in grid slots
 *     "height":    int,                 viewport height in grid slots
 *     "viewer_x":  int,                 viewer column inside the grid
 *     "viewer_y":  int,                 viewer row inside the grid
 *     "locations": int **,              grid[y][x] == one of CART_*
 *     "rooms":     object **,           grid[y][x] == room/location or nil
 *   ])
 *
 * Or nil if the viewer has no environment or the environment refuses
 * to render a map (dungeon / underwater rooms).
 *
 * The two grids are aligned: `rooms[y][x]` holds the object that produced
 * `locations[y][x]` where a location is drawn, and is `nil` where nothing is
 * or where the slot holds the link between two locations. Renderers that
 * only care about what each location is ignore `rooms`; renderers that need
 * coordinates (coord overlay, future pathfinding) read it.
 *
 * `options` (mapping, all optional):
 *   "deep":   1 to force loading every reachable room (slow on legacy
 *             room-based areas; safe on locations). Default 0.
 *   "width":  viewport width in grid slots. Default CART_DEFAULT_WIDTH.
 *   "height": viewport height in grid slots. Default CART_DEFAULT_HEIGHT.
 */
mapping query_map_view(object viewer, varargs mapping options)
{
  object env;
  int deep, width, height;
  int ** locations;
  object ** rooms;
  mixed * pending;
  mixed * revised;
  mixed current;
  int viewer_x, viewer_y;
  int i, j, repeated, viewer_z;

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

  // initialise the grids with empty locations
  locations = allocate(height);
  rooms = allocate(height);
  for (i = 0; i < height; i++)
  {
    locations[i] = allocate_int(width);
    rooms[i] = allocate(width);
  }

  viewer_x = width / 2;
  viewer_y = height / 2;

  // viewer's z plane — only neighbours at the same z are drawn. CART_NO_Z
  // means "we cannot tell" (legacy room without coordinates), in which
  // case we do not filter and accept the previous behaviour of mixing
  // levels (best effort with no information).
  viewer_z = _z_of(env);

  // BFS from the viewer's environment outward through exits
  pending = ({ });
  revised = ({ });

  current = ({ viewer_x, viewer_y, env });
  pending += ({ current });
  locations[viewer_y][viewer_x] = _classify_room(env, viewer, deep);
  rooms[viewer_y][viewer_x] = env;

  while (sizeof(pending))
  {
    string * dest_dir;

    current = pending[0];
    dest_dir = current[2]->query_dest_dir();

    for (i = 0; i < sizeof(dest_dir); i += 2)
    {
      object new_room;
      int seg_x, seg_y;            // where the link to it is drawn
      int dest_x, dest_y;          // where the destination is drawn
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

      // a door / gate on this exit crosses its segment glyph instead of
      // marking the room: upgrade the plain segment to its door variant
      if (member_array(current[2]->query_ex_type(dest_dir[i]),
                       ({ "door", "gate" })) != -1)
      {
        if (seg_type == CART_VERTICAL_EXIT)        seg_type = CART_VERTICAL_DOOR;
        else if (seg_type == CART_HORIZONTAL_EXIT) seg_type = CART_HORIZONTAL_DOOR;
        else if (seg_type == CART_SLASH_EXIT)      seg_type = CART_SLASH_DOOR;
        else if (seg_type == CART_BACKSLASH_EXIT)  seg_type = CART_BACKSLASH_DOOR;
      }

      // Resolve destination first; if it lives on a different z plane
      // we draw nothing (no segment, no destination). The source
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
      repeated = 0;
      for (j = 0; j < sizeof(revised); j++)
        if (revised[j][2] == new_room) { repeated = 1; break; }
      if (repeated)
        continue;

      if (has_segment)
        locations[seg_y][seg_x] = seg_type;

      // Maze locations are deliberately opaque to cartography: we
      // draw the connecting segment and a '?' ghost where the
      // maze begins, but never enqueue the maze room itself. The
      // map intentionally hides the labyrinth's layout — knowing
      // it would defeat the point, since movement inside is
      // randomised by the maze component anyway. Legacy rooms do
      // not implement query_maze(), so the call returns nil there.
      if (new_room->query_maze())
      {
        if (has_dest)
        {
          locations[dest_y][dest_x] = CART_MAZE_ROOM;
          rooms[dest_y][dest_x] = new_room;
        }
        continue;
      }

      if (has_dest)
      {
        locations[dest_y][dest_x] = _classify_room(new_room, viewer, deep);
        rooms[dest_y][dest_x] = new_room;
        pending += ({ ({ dest_x, dest_y, new_room }) });
      }
    }

    revised += ({ current });
    pending -= ({ current });
  }

  return ([
    "width":    width,
    "height":   height,
    "viewer_x": viewer_x,
    "viewer_y": viewer_y,
    "locations":    locations,
    "rooms":    rooms,
  ]);
}

/**
 * Pure graph reachability from `start`, following every exit up to
 * `max_depth` steps. Unlike query_map_view this is not projected onto a
 * 2D grid: it walks vertical (up / down) and diagonal exits too, does
 * not filter by z plane, and returns a flat, deduped array of the
 * reachable room / location objects (start included).
 *
 * `deep` (default 0) is passed straight to the same destination
 * resolution query_map_view uses: legacy `.c` rooms are only loaded on
 * demand when `deep` is set, while `.o` locations always resolve through
 * LOCATION_HANDLER. Callers that pass `deep = 1` own the tick budget —
 * a single synchronous sweep over a large radius can be expensive, which
 * is why the location cleaner warms cold neighbourhoods through its own
 * chunked worker rather than one deep call here.
 *
 * Maze locations are boundaries: the maze entry is included in the set
 * but its interior is never walked (its layout is randomised, so walking
 * it is meaningless — same rationale as the map's ghost).
 */
object * walk_reachable(object start, int max_depth, varargs int deep)
{
  mapping visited;
  mixed * queue;              // ({ ({ room, depth }), ... }), FIFO via head
  int head;

  if (!start || max_depth < 0)
    return ({ });

  visited = ([ start : 1 ]);
  queue = ({ ({ start, 0 }) });
  head = 0;

  while (head < sizeof(queue))
  {
    object room;
    int depth;
    string * dest_dir;
    int i;

    room  = queue[head][0];
    depth = queue[head][1];
    head++;

    if (depth >= max_depth)
      continue;

    dest_dir = room->query_dest_dir();
    if (!dest_dir)
      continue;

    // query_dest_dir returns ({ dir, dest, dir, dest, ... }) pairs
    for (i = 0; i < sizeof(dest_dir); i += 2)
    {
      object dest;

      dest = _resolve_destination(dest_dir[i + 1], deep);
      if (!dest || visited[dest])
        continue;

      visited[dest] = 1;

      // maze boundary: keep the entry, do not enqueue its interior
      if (dest->query_maze())
        continue;

      queue += ({ ({ dest, depth + 1 }) });
    }
  }

  return map_indices(visited);
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
  int ** locations;
  int vx, vy;
  string out, line;
  int i, j, type;

  width  = view["width"];
  height = view["height"];
  locations = view["locations"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      type = locations[i][j];

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
        case CART_HOME_ROOM:          line += "[" + CART_HOME_GLYPH + "]";           break;
        case CART_COAST_ROOM:         line += "%^BLUE%^[ ]%^RESET%^";                break;
        case CART_DOOR_ROOM:          line += "[D]";                                 break;
        case CART_UP_ROOM:            line += "[^]";                                 break;
        case CART_DOWN_ROOM:          line += "[v]";                                 break;
        case CART_FINISH_QUEST_ROOM:  line += "[%^BOLD%^YELLOW%^?%^RESET%^]";        break;
        case CART_QUEST_ROOM:         line += "[%^BOLD%^YELLOW%^!%^RESET%^]";        break;
        case CART_ADVENTURER_ROOM:    line += "[%^BOLD%^CYAN%^*%^RESET%^]";          break;
        case CART_GUARD_ROOM:         line += "[%^BOLD%^GREEN%^*%^RESET%^]";         break;
        case CART_ENEMY_ROOM:         line += "[%^BOLD%^RED%^*%^RESET%^]";           break;
        case CART_MAZE_ROOM:          line += "[%^BOLD%^MAGENTA%^?%^RESET%^]";       break;
        case CART_HORIZONTAL_EXIT:    line += "---";                                 break;
        case CART_VERTICAL_EXIT:      line += " | ";                                 break;
        case CART_SLASH_EXIT:         line += " / ";                                 break;
        case CART_BACKSLASH_EXIT:     line += " \\ ";                                break;
        case CART_HORIZONTAL_DOOR:    line += "-" + CART_DOOR_GLYPH_H + "-";          break;
        case CART_VERTICAL_DOOR:      line += " " + CART_DOOR_GLYPH_V + " ";          break;
        case CART_SLASH_DOOR:         line += " " + CART_DOOR_GLYPH_SLASH + " ";      break;
        case CART_BACKSLASH_DOOR:     line += " " + CART_DOOR_GLYPH_BSLASH + " ";     break;
        default:                      line += "   ";                                 break;
      }
    }
    out += line + "\n";
  }

  return out;
}

/**
 * Compact renderer: one character per location. Three times denser, fits
 * a much larger viewport in the same screen real estate. Roguelike
 * feel.
 *
 *   '@'  viewer
 *   '.'  no location here
 *   '#'  plain room
 *   'D'  door room      '^'  up exit       'v'  down exit
 *   '~'  coast          '?'  finished quest waiting to turn in
 *   '!'  open quest     '*'  friendly group / guard       'X'  enemy
 *   '|'  vertical exit  '-'  horizontal exit  '/'  slash  '\\' backslash
 */
string render_compact(mapping view)
{
  int width, height;
  int ** locations;
  int vx, vy;
  string out, line;
  int i, j, type;

  width  = view["width"];
  height = view["height"];
  locations = view["locations"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      type = locations[i][j];

      if (i == vy && j == vx)
      {
        line += "%^ORANGE%^@%^RESET%^";
        continue;
      }

      switch (type)
      {
        case CART_ROOM:               line += "#";                                   break;
        case CART_HOME_ROOM:          line += CART_HOME_GLYPH;                        break;
        case CART_COAST_ROOM:         line += "%^BLUE%^~%^RESET%^";                  break;
        case CART_DOOR_ROOM:          line += "D";                                   break;
        case CART_UP_ROOM:            line += "^";                                   break;
        case CART_DOWN_ROOM:          line += "v";                                   break;
        case CART_FINISH_QUEST_ROOM:  line += "%^BOLD%^YELLOW%^?%^RESET%^";          break;
        case CART_QUEST_ROOM:         line += "%^BOLD%^YELLOW%^!%^RESET%^";          break;
        case CART_ADVENTURER_ROOM:    line += "%^BOLD%^CYAN%^*%^RESET%^";            break;
        case CART_GUARD_ROOM:         line += "%^BOLD%^GREEN%^*%^RESET%^";           break;
        case CART_ENEMY_ROOM:         line += "%^BOLD%^RED%^X%^RESET%^";             break;
        case CART_MAZE_ROOM:          line += "%^BOLD%^MAGENTA%^?%^RESET%^";         break;
        case CART_HORIZONTAL_EXIT:    line += "-";                                   break;
        case CART_VERTICAL_EXIT:      line += "|";                                   break;
        case CART_SLASH_EXIT:         line += "/";                                   break;
        case CART_BACKSLASH_EXIT:     line += "\\";                                  break;
        case CART_HORIZONTAL_DOOR:    line += CART_DOOR_GLYPH_H;                      break;
        case CART_VERTICAL_DOOR:      line += CART_DOOR_GLYPH_V;                      break;
        case CART_SLASH_DOOR:         line += CART_DOOR_GLYPH_SLASH;                  break;
        case CART_BACKSLASH_DOOR:     line += CART_DOOR_GLYPH_BSLASH;                 break;
        default:                      line += ".";                                   break;
      }
    }
    out += line + "\n";
  }

  return out;
}

/**
 * Coordinate overlay renderer: shows the world `(x,y)` of each
 * location instead of an opaque glyph. Intended for coders
 * and admins debugging conversions, sector indexes, or inferred
 * coordinates — not for player consumption. Slots without a backing
 * room (empty space, exit segments) are blank.
 *
 * Each location is rendered as a fixed-width "(x,y)" tag. The viewer's
 * location shows "(*x,y)" with the leading asterisk so it stays findable
 * in the grid.
 */
string render_coords(mapping view)
{
  int width, height;
  int ** locations;
  object ** rooms;
  int vx, vy;
  string out, line;
  int i, j;

  width  = view["width"];
  height = view["height"];
  locations = view["locations"];
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
        // the links between locations get a small marker too so the topology is
        // still readable without needing the room glyphs
        switch (locations[i][j])
        {
          case CART_HORIZONTAL_EXIT:  line += "  ----  "; break;
          case CART_VERTICAL_EXIT:    line += "    |   "; break;
          case CART_SLASH_EXIT:       line += "    /   "; break;
          case CART_BACKSLASH_EXIT:   line += "    \\   "; break;
          case CART_HORIZONTAL_DOOR:  line += "  --" + CART_DOOR_GLYPH_H + "-  "; break;
          case CART_VERTICAL_DOOR:    line += "    " + CART_DOOR_GLYPH_V + "   "; break;
          case CART_SLASH_DOOR:       line += "    " + CART_DOOR_GLYPH_SLASH + "   "; break;
          case CART_BACKSLASH_DOOR:   line += "    " + CART_DOOR_GLYPH_BSLASH + "   "; break;
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

/**
 * Box-drawing Unicode renderer: similar density to render_compact but
 * with proper line glyphs (─ │ ╱ ╲) and filled square glyphs for
 * rooms (▢ ◉ for the viewer, ▦ for doors, ▲/▼ for stairs, ◯ for
 * coast). Requires a UTF-8 capable client; falls back gracefully on
 * 8-bit terminals (the bytes are still printable, just not pretty).
 */
string render_unicode(mapping view)
{
  int width, height;
  int ** locations;
  int vx, vy;
  string out, line;
  int i, j, type;

  width  = view["width"];
  height = view["height"];
  locations = view["locations"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      type = locations[i][j];

      if (i == vy && j == vx)
      {
        // ◉ U+25C9 fisheye (viewer)
        line += "%^ORANGE%^" + chr(226) + chr(151) + chr(137) + "%^RESET%^";
        continue;
      }

      switch (type)
      {
        case CART_ROOM:
          // ▢ U+25A2 white square with rounded corners
          line += chr(226) + chr(150) + chr(162);                                     break;
        case CART_HOME_ROOM:
          // ⌂ U+2302 house
          line += CART_HOME_GLYPH;                                                     break;
        case CART_COAST_ROOM:
          // ◯ U+25EF large circle
          line += "%^BLUE%^" + chr(226) + chr(151) + chr(175) + "%^RESET%^";          break;
        case CART_DOOR_ROOM:
          // ▦ U+25A6 square with horizontal fill
          line += chr(226) + chr(150) + chr(166);                                     break;
        case CART_UP_ROOM:
          // ▲ U+25B2 black up-pointing triangle
          line += chr(226) + chr(150) + chr(178);                                     break;
        case CART_DOWN_ROOM:
          // ▼ U+25BC black down-pointing triangle
          line += chr(226) + chr(150) + chr(188);                                     break;
        case CART_FINISH_QUEST_ROOM:
          line += "%^BOLD%^YELLOW%^" + chr(226) + chr(150) + chr(162) + "%^RESET%^"; break;
        case CART_QUEST_ROOM:
          // ▣ U+25A3 square containing white square
          line += "%^BOLD%^YELLOW%^" + chr(226) + chr(150) + chr(163) + "%^RESET%^"; break;
        case CART_ADVENTURER_ROOM:
          line += "%^BOLD%^CYAN%^"   + chr(226) + chr(150) + chr(163) + "%^RESET%^"; break;
        case CART_GUARD_ROOM:
          line += "%^BOLD%^GREEN%^"  + chr(226) + chr(150) + chr(163) + "%^RESET%^"; break;
        case CART_ENEMY_ROOM:
          line += "%^BOLD%^RED%^"    + chr(226) + chr(150) + chr(163) + "%^RESET%^"; break;
        case CART_MAZE_ROOM:
          // ? — plain question mark, intentionally not a unicode glyph
          line += "%^BOLD%^MAGENTA%^?%^RESET%^";                                      break;
        case CART_HORIZONTAL_EXIT:
          // ─ U+2500 box drawings light horizontal
          line += chr(226) + chr(148) + chr(128);                                     break;
        case CART_VERTICAL_EXIT:
          // │ U+2502 box drawings light vertical
          line += chr(226) + chr(148) + chr(130);                                     break;
        case CART_SLASH_EXIT:
          // ╱ U+2571 box drawings light diagonal upper right to lower left
          line += chr(226) + chr(149) + chr(177);                                     break;
        case CART_BACKSLASH_EXIT:
          // ╲ U+2572 box drawings light diagonal upper left to lower right
          line += chr(226) + chr(149) + chr(178);                                     break;
        case CART_HORIZONTAL_DOOR:    line += CART_DOOR_GLYPH_H;                       break;
        case CART_VERTICAL_DOOR:      line += CART_DOOR_GLYPH_V;                       break;
        case CART_SLASH_DOOR:         line += CART_DOOR_GLYPH_SLASH;                   break;
        case CART_BACKSLASH_DOOR:     line += CART_DOOR_GLYPH_BSLASH;                  break;
        default:
          line += " ";                                                                break;
      }
    }
    out += line + "\n";
  }

  return out;
}

/**
 * Color-by-area renderer: same chunky `[ ]` glyphs as render_ascii,
 * but each location is tinted with a palette colour assigned to its
 * `query_area_name()`. Lets the player see at a glance the boundary
 * between areas without losing any of the marker information.
 *
 * The palette is fixed (8 colours); if more areas appear in the same
 * view the palette wraps around. Areas are sorted by first appearance
 * in the grid so the colour mapping is stable from one view to the
 * next as long as the player is in the same neighbourhood.
 */
string render_color_by_area(mapping view)
{
  int width, height;
  int ** locations;
  object ** rooms;
  int vx, vy;
  string out, line;
  string * palette;
  mapping area_to_color;
  int i, j, type, palette_idx;
  string viewer_marker;

  width  = view["width"];
  height = view["height"];
  locations = view["locations"];
  rooms  = view["rooms"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  // 8 distinct colours; non-bold + bold cycle. Avoid orange (used for
  // the viewer) and red (used for enemies) so meaning stays clear.
  palette = ({
    "%^CYAN%^",
    "%^GREEN%^",
    "%^YELLOW%^",
    "%^MAGENTA%^",
    "%^BLUE%^",
    "%^BOLD%^CYAN%^",
    "%^BOLD%^GREEN%^",
    "%^BOLD%^MAGENTA%^",
  });

  area_to_color = ([ ]);
  palette_idx   = 0;

  out = "";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      object room;
      string area_name, tint;

      type = locations[i][j];
      room = rooms[i][j];

      // pick a tint for the location based on its area
      tint = "";
      if (room)
      {
        area_name = room->query_area_name();
        if (area_name)
        {
          if (!area_to_color[area_name])
          {
            area_to_color[area_name] = palette[palette_idx % sizeof(palette)];
            palette_idx++;
          }
          tint = area_to_color[area_name];
        }
      }

      if (i == vy && j == vx)
      {
        // the viewer keeps its orange asterisk on top of the area tint
        if (strlen(tint))
          line += tint + "[%^RESET%^%^ORANGE%^*%^RESET%^" + tint + "]%^RESET%^";
        else
          line += "[%^ORANGE%^*%^RESET%^]";
        continue;
      }

      switch (type)
      {
        case CART_ROOM:
          line += (strlen(tint) ? tint + "[ ]%^RESET%^" : "[ ]");
          break;
        case CART_HOME_ROOM:
          line += (strlen(tint)
                   ? tint + "[" + CART_HOME_GLYPH + "]%^RESET%^"
                   : "[" + CART_HOME_GLYPH + "]");
          break;
        case CART_COAST_ROOM:
          line += "%^BLUE%^[ ]%^RESET%^";
          break;
        case CART_DOOR_ROOM:
          line += (strlen(tint) ? tint + "[D]%^RESET%^" : "[D]");
          break;
        case CART_UP_ROOM:
          line += (strlen(tint) ? tint + "[^]%^RESET%^" : "[^]");
          break;
        case CART_DOWN_ROOM:
          line += (strlen(tint) ? tint + "[v]%^RESET%^" : "[v]");
          break;
        case CART_FINISH_QUEST_ROOM:
          line += "[%^BOLD%^YELLOW%^?%^RESET%^]";
          break;
        case CART_QUEST_ROOM:
          line += "[%^BOLD%^YELLOW%^!%^RESET%^]";
          break;
        case CART_ADVENTURER_ROOM:
          line += "[%^BOLD%^CYAN%^*%^RESET%^]";
          break;
        case CART_GUARD_ROOM:
          line += "[%^BOLD%^GREEN%^*%^RESET%^]";
          break;
        case CART_ENEMY_ROOM:
          line += "[%^BOLD%^RED%^*%^RESET%^]";
          break;
        case CART_MAZE_ROOM:
          line += "[%^BOLD%^MAGENTA%^?%^RESET%^]";
          break;
        case CART_HORIZONTAL_EXIT:
          line += "---";
          break;
        case CART_VERTICAL_EXIT:
          line += " | ";
          break;
        case CART_SLASH_EXIT:
          line += " / ";
          break;
        case CART_BACKSLASH_EXIT:
          line += " \\ ";
          break;
        case CART_HORIZONTAL_DOOR:
          line += "-" + CART_DOOR_GLYPH_H + "-";
          break;
        case CART_VERTICAL_DOOR:
          line += " " + CART_DOOR_GLYPH_V + " ";
          break;
        case CART_SLASH_DOOR:
          line += " " + CART_DOOR_GLYPH_SLASH + " ";
          break;
        case CART_BACKSLASH_DOOR:
          line += " " + CART_DOOR_GLYPH_BSLASH + " ";
          break;
        default:
          line += "   ";
          break;
      }
    }
    out += line + "\n";
  }

  return out;
}
