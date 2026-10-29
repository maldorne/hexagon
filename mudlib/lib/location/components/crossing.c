// Crossing component. Attached during conversion when the source room
// answers query_crossing() with 1. A crossing leads astray whoever leaves it
// without keeping their bearings: the exit they typed is replaced by one of
// the location's visible exits chosen at random.
//
// Whoever passes freely, in this order:
//   - a living whose race id is in the list the room gave (free_races);
//   - a living whose orientation skill is above sure_orientation;
//   - a living that passes a perception roll (random(perception) > roll),
//     which also raises its orientation by one point.
// A mover without the orientation skill learns it here.
//
// Plugs into the location component hook system (see <room/location.h>).

#include <room/location.h>
#include <living/skills.h>
#include <language.h>
#include <translations/exits.h>

inherit component "/lib/location/component.c";

// race ids that never lose their way
private string * free_races;
// orientation above which nobody loses their way
private int sure_orientation;
// perception rolls above this pass (and raise the orientation)
private int roll;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_CROSSING);
  free_races = ({ });
  sure_orientation = 40;
  roll = 6;
}

void init() {}
void dest_me() {}

void set_free_races(string * races) { free_races = races ? ({ }) + races : ({ }); }
string * query_free_races() { return ({ }) + free_races; }

void set_sure_orientation(int value) { sure_orientation = value; }
int query_sure_orientation() { return sure_orientation; }

void set_roll(int value) { roll = value; }
int query_roll() { return roll; }

mapping query_hooks()
{
  return ([ "do_exit_command": HOOK_PRIORITY_TRANSFORM ]);
}

/**
 * Whether a living leaves the crossing by the exit it typed.
 *
 * @param  who  the living leaving
 * @return 1 if it keeps its bearings
 */
int query_keeps_bearings(object who)
{
  int ability;

  if (!who)
    return 1;

  if (member_array(who->query_race_id(), free_races) != -1)
    return 1;

  if (!who->query_known_skill(SKILL_ORIENTATION))
    who->add_known_skill(SKILL_ORIENTATION, 1);

  ability = who->query_skill_ability(SKILL_ORIENTATION);
  if (ability > sure_orientation)
    return 1;

  if (random(who->query_per()) > roll)
  {
    who->adjust_skill_ability(SKILL_ORIENTATION, 1, 1);
    return 1;
  }

  return 0;
}

/**
 * Pipeline contract for do_exit_command: args = ({ str, verb, ob }); returns
 * the (possibly transformed) args. A mover that loses its way gets another
 * of the visible exits of the location in place of the typed one.
 *
 * @param  args  ({ str, verb, ob })
 * @return the args, with the verb changed if the mover is led astray
 */
mixed * hook_do_exit_command(mixed * args)
{
  string * dirs, * open;
  object ob, place;
  int i;

  ob = args[2];
  place = query_entity();
  if (!ob || !place || !place->query_verb_exit(args[1]))
    return args;

  if (query_keeps_bearings(ob))
    return args;

  dirs = place->query_direc();
  open = ({ });
  for (i = 0; i < sizeof(dirs); i++)
    if (!place->query_sealed_exit(dirs[i]))
      open += ({ dirs[i] });

  if (!sizeof(open))
    return args;

  args[1] = open[random(sizeof(open))];
  tell_object(ob, (args[1] == DIR_UP || args[1] == DIR_DOWN ?
                   _LANG_CROSSING_LOST_VERTICAL : _LANG_CROSSING_LOST) +
                  args[1] + ".\n");
  return args;
}

// ************************************************************
//  Persistence
// ************************************************************

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "free_races": free_races, "sure_orientation": sure_orientation,
            "roll": roll ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (pointerp(args["free_races"]))
    free_races = args["free_races"];
  if (intp(args["sure_orientation"]))
    sure_orientation = args["sure_orientation"];
  if (intp(args["roll"]))
    roll = args["roll"];
}
