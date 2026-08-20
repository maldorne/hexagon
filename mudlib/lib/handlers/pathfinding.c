// pathfinding.c — the fine (room-to-room) pathfinder.
//
// A* over the movement graph the sectors already index. Each occupied
// coordinate is a node (sector.c::query_nodes, "x_y_z" -> { file, maze }) and
// each traversable exit is an edge (sector.c::query_edges, "x_y_z" -> [ dir :
// neighbour-coord ]). The edge set is the full graph, cross-sector links
// included, so A* walks it uniformly: whenever it needs a coordinate's edges it
// fetches that coordinate's sector on demand (SECTORS_HANDLER::query_sector_for_coord),
// crossing sector borders without any special case.
//
// The result is the sequence of canonical directions ("north", "up", ...) a
// walker follows to get from one location to another -- exactly what the NPC
// mover feeds to its step-by-step movement.

#include <maps/maps.h>
#include <maps/sector.h>
#include <room/location.h>

// Safety cap on node expansions so a malformed or disconnected graph can never
// spin forever; a single settlement is a few dozen nodes, so this is orders of
// magnitude above any real path.
#define MAX_EXPANSIONS 5000

// Parse a "x_y_z" coordinate key into its three integers.
private int * parse_key(string key)
{
  string * parts;

  parts = explode(key, "_");
  if (sizeof(parts) != 3)
    return nil;
  return ({ (int)parts[0], (int)parts[1], (int)parts[2] });
}

// Admissible heuristic: the cheapest conceivable step count between two coords.
// The plane allows the eight compass moves (a diagonal costs the same one step
// as a cardinal), so the plane distance is the Chebyshev distance max(|dx|,|dy|);
// vertical moves (up/down) have no diagonal, so |dz| adds on top. Never
// overestimates the true cost, which keeps A* optimal.
private int heuristic(string a_key, string b_key)
{
  int * a, * b;
  int dx, dy, dz;

  a = parse_key(a_key);
  b = parse_key(b_key);
  if (!a || !b)
    return 0;

  dx = a[0] - b[0]; if (dx < 0) dx = -dx;
  dy = a[1] - b[1]; if (dy < 0) dy = -dy;
  dz = a[2] - b[2]; if (dz < 0) dz = -dz;

  return (dx > dy ? dx : dy) + dz;
}

// The edges leaving a coordinate ([ dir : neighbour-coord ]), fetched from
// whatever sector owns that coordinate. Empty when the coord is unindexed.
private mapping edges_at(string game_slug, string map_name, string key)
{
  int * c;
  object sector;
  mapping e;

  c = parse_key(key);
  if (!c)
    return ([ ]);

  sector = SECTORS_HANDLER->query_sector_for_coord(game_slug, map_name,
                                                c[0], c[1], c[2]);
  if (!sector)
    return ([ ]);

  e = sector->query_edges()[key];
  return mappingp(e) ? e : ([ ]);
}

// Whether a coordinate is a real indexed node (a location sits there). An edge
// may point at a coordinate that was never indexed (a dead exit); those are not
// walkable and must not enter the frontier.
private int node_exists(string game_slug, string map_name, string key)
{
  int * c;
  object sector;

  c = parse_key(key);
  if (!c)
    return 0;

  sector = SECTORS_HANDLER->query_sector_for_coord(game_slug, map_name,
                                                c[0], c[1], c[2]);
  if (!sector)
    return 0;

  return !undefinedp(sector->query_nodes()[key]);
}

// The open-set member with the lowest f-score, found by linear scan. The
// frontier of a settlement-sized search stays small, so a scan beats the
// bookkeeping of a heap here.
private string lowest_f(string * open, mapping f_score)
{
  string best;
  int best_f, i;

  best = open[0];
  best_f = f_score[best];
  for (i = 1; i < sizeof(open); i++)
    if (f_score[open[i]] < best_f)
    {
      best = open[i];
      best_f = f_score[best];
    }
  return best;
}

// Walk the came_from chain back from the goal to the start, collecting the
// direction taken into each node, then reverse to get start -> goal order.
private string * reconstruct(mapping came_from, string goal_key)
{
  string * dirs;
  mixed step;
  string cursor;

  dirs = ({ });
  cursor = goal_key;
  // came_from[key] = ({ previous-key, direction-taken-to-reach-key })
  while ((step = came_from[cursor]))
  {
    dirs = ({ step[1] }) + dirs;
    cursor = step[0];
  }
  return dirs;
}

// Core A* on coordinate keys. Returns the list of canonical directions from
// start_key to goal_key, ({ }) when they are the same node, or nil when no
// path exists (or the search hits its expansion cap).
string * find_path_coords(string game_slug, string map_name,
                          string start_key, string goal_key)
{
  string * open;
  mapping g_score, f_score, came_from, closed;
  int expansions;

  if (start_key == goal_key)
    return ({ });

  if (!node_exists(game_slug, map_name, start_key) ||
      !node_exists(game_slug, map_name, goal_key))
    return nil;

  open = ({ start_key });
  g_score = ([ start_key : 0 ]);
  f_score = ([ start_key : heuristic(start_key, goal_key) ]);
  came_from = ([ ]);
  closed = ([ ]);

  while (sizeof(open) && expansions++ < MAX_EXPANSIONS)
  {
    string current;
    mapping neighbours;
    string * dirs;
    int i, current_g;

    current = lowest_f(open, f_score);
    if (current == goal_key)
      return reconstruct(came_from, goal_key);

    open -= ({ current });
    closed[current] = 1;
    current_g = g_score[current];

    neighbours = edges_at(game_slug, map_name, current);
    dirs = map_indices(neighbours);
    for (i = 0; i < sizeof(dirs); i++)
    {
      string dir, next;
      int tentative_g;

      dir = dirs[i];
      next = neighbours[dir];

      // skip nodes already finalised and edges that lead off the indexed map
      if (closed[next])
        continue;
      if (!node_exists(game_slug, map_name, next))
        continue;

      // every step costs one move
      tentative_g = current_g + 1;

      // record this route only if it is the first or a cheaper way to reach next
      if (undefinedp(g_score[next]) || tentative_g < g_score[next])
      {
        came_from[next] = ({ current, dir });
        g_score[next] = tentative_g;
        f_score[next] = tentative_g + heuristic(next, goal_key);
        if (member_array(next, open) < 0)
          open += ({ next });
      }
    }
  }

  return nil;
}

// Convenience entry point: the direction list to walk from one location to
// another. Both must sit in the same game and map (the coordinate graph is
// per-map); returns ({ }) when they are the same coordinate and nil when there
// is no route or the endpoints are not indexed.
string * find_path(object from, object to)
{
  int * fc, * tc;
  string game_slug, map_name, start_key, goal_key;

  if (!from || !to)
    return nil;

  fc = from->query_coordinates();
  tc = to->query_coordinates();
  if (!fc || !tc || sizeof(fc) != 3 || sizeof(tc) != 3)
    return nil;

  if (from->query_map_name() != to->query_map_name())
    return nil;
  map_name = from->query_map_name();

  game_slug = game_from_path(from->query_file_name());
  if (game_slug != game_from_path(to->query_file_name()))
    return nil;

  start_key = "" + fc[0] + "_" + fc[1] + "_" + fc[2];
  goal_key  = "" + tc[0] + "_" + tc[1] + "_" + tc[2];

  return find_path_coords(game_slug, map_name, start_key, goal_key);
}
