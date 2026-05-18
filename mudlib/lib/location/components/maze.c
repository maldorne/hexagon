// Maze component. Attached during conversion when the source room
// has the MAZE_PROP property set. Behaviour: when a player issues a
// movement command, the component randomises the verb among the
// location's declared exits before the actual move runs. The location
// lives with real coordinates and real declared exits like any other;
// the maze feel comes from the run-time swap, not from a deliberately
// bogus exit graph.
//
// See muds/notes/hexagon/dev/location-hook-system.md for the hook
// system this plugs into.

#include <room/location.h>
#include <room/location_hooks.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_MAZE);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

// Register as a movement-pipeline transform.
mapping query_hooks()
{
  return ([ "do_exit_command": HOOK_PRIORITY_TRANSFORM ]);
}

// Pipeline contract for do_exit_command:
//   args = ({ str, verb, ob })
//   returns the (possibly transformed) ({ str, verb, ob })
//
// Pick a random direction among the location's declared exits and
// substitute it in. The inherited exit dispatcher looks up the verb
// in dest_direc, so any registered direction is valid. If there is
// only one exit (or none), nothing to randomise.
mixed * hook_do_exit_command(mixed * args)
{
  string * dirs;

  dirs = location->query_direc();
  if (dirs && sizeof(dirs) > 1)
    args[1] = dirs[random(sizeof(dirs))];

  return args;
}
