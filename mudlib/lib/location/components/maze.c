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
#include <living/skills.h>

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
//
// The orientation skill gates the scramble: on a successful check
// (random(100) < ability) the mover keeps the typed direction and
// moves true. A mover without the skill has ability 0, so the check
// never passes and the maze scrambles as before. args[2] is the
// moving living (defaulted to this_player() by the orchestrator).
mixed * hook_do_exit_command(mixed * args)
{
  string * dirs;
  string typed, chosen;
  object ob;
  int ability;

  typed = args[1];
  ob = args[2];
  ability = 0;
  if (ob)
    ability = ob->query_skill_ability(SKILL_ORIENTATION);
  // A mover lacking the skills mixin returns nil; coerce to 0.
  if (!ability)
    ability = 0;

  if (random(100) < ability)
    return args;

  dirs = location->query_direc();
  if (dirs && sizeof(dirs) > 1)
  {
    chosen = dirs[random(sizeof(dirs))];
    args[1] = chosen;

    // Lucky hit: the scramble landed on the direction the mover typed,
    // so they moved true by chance. A mover who does not yet have the
    // orientation skill picks it up here — wandering the maze is itself
    // how a newbie first learns to orient. The starting ability (10%)
    // comes from the skills table.
    if (chosen == typed && ob &&
        !ob->query_known_skill(SKILL_ORIENTATION))
      ob->add_known_skill(SKILL_ORIENTATION);
  }

  return args;
}
