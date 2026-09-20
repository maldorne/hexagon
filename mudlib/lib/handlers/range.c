// range.c — how far apart two things are, and whether that is close enough.
//
// The single place anything asks about distance: effects with a range, ranged
// weapons, whatever tracks somebody down. Callers never choose a method, they
// name the two objects; this handler reads where each of them stands and picks:
//
//   - both in places with coordinates on the same map -> the coordinate graph
//     (/lib/handlers/pathfinding.c), which walks the sector index without
//     loading a single location.
//   - anything else (a rooms game, or one end in a room) -> a breadth-first
//     walk of the exits, level by level, over rooms that are already loaded.
//
// Distances are counted in steps: 0 is the same place, and -1 is "further than
// the limit given", not "unknown".
//
// The exit walk comes from the range handler of the MudOS Ciudad Capital
// (Folken, 08/2008), rewritten for Hexagon.

#include <sector/sector.h>

// Nothing is ever measured further than this, however large a range asks: every
// extra step multiplies the places to visit, and a ranged weapon or a tracking
// skill may ask on every heart_beat.
#define MAX_RANGE 10

// prototypes
int query_distance(object me, object him, varargs int max_range);

private object * exits_from(object where);

// Livings only: the exit walk looks for somebody, not for something.
private int is_living(object ob) { return living(ob); }

// The places reachable in one step from here, as objects. Only what is already
// loaded counts: measuring a distance must never drag half a world into memory,
// and a place nobody has visited holds nobody to find. A closed door is a wall.
private object * exits_from(object where)
{
  mixed * exits;
  object * destinations;
  object door, ob;
  int i;

  destinations = ({ });
  exits = where->query_dest_dir();

  if (!arrayp(exits))
    return destinations;

  // ({ direction, destination, direction, destination, ... })
  for (i = 1; i < sizeof(exits); i += 2)
  {
    door = where->query_door_ob(exits[i - 1]);

    if (door && !door->query_status())
      continue;

    if (!stringp(exits[i]))
      continue;

    ob = find_object(exits[i]);

    if (ob)
      destinations += ({ ob });
  }

  return destinations;
}

// Steps between two places by walking the exits, level by level, out to the
// range asked. -1 when the second one is not found within it.
private int walked_distance(object from, object to, int max_range)
{
  object * checked, * to_check, * next, * people;
  object current;
  int depth, i;

  if (from == to)
    return 0;

  checked = ({ });
  to_check = ({ from });
  depth = 0;

  while (depth <= max_range && sizeof(to_check))
  {
    next = ({ });

    for (i = 0; i < sizeof(to_check); i++)
    {
      current = to_check[i];
      people = filter_array(all_inventory(current), "is_living");

      if (member_array(to, people) != -1)
        return depth;

      checked += ({ current });
      next += exits_from(current) - (checked + to_check);
    }

    depth++;
    to_check = (depth <= max_range) ? next : ({ });
  }

  return -1;
}

// Steps between whoever holds these two objects: 0 in the same place, -1 when
// they are further apart than the range asked (or not connected at all).
int query_distance(object me, object him, varargs int max_range)
{
  object here, there;
  int steps;

  if (!me || !him)
    return -1;

  if (max_range <= 0 || max_range > MAX_RANGE)
    max_range = MAX_RANGE;

  here = environment(me);
  there = environment(him);

  if (!here || !there)
    return -1;

  if (here == there)
    return 0;

  // the cheapest question first: with coordinates on both ends, anything
  // further than the range in a straight line is out of it for good
  steps = PATHFIND_HANDLER->query_min_distance(here, there);

  if (steps > max_range)
    return -1;

  // places with coordinates are measured on the coordinate graph, which reads
  // the sector index and loads nothing
  if (steps >= 0)
  {
    steps = PATHFIND_HANDLER->query_distance(here, there);
    return (steps >= 0 && steps <= max_range) ? steps : -1;
  }

  return walked_distance(here, there, max_range);
}

// Whether the second one is close enough to be reached from the first
int check_in_range(object me, object him, int max_range)
{
  return (query_distance(me, him, max_range) >= 0);
}
