// Maze room base for the demo-fantasy forest. demo-fantasy runs on
// plain rooms that are never converted to locations, so the maze
// behaviour cannot come from the maze location component; it lives
// here instead. A forest room becomes a maze simply by inheriting
// this file instead of /lib/room.c.
//
// On a movement command the orientation skill gates the scramble:
//   - random(100) < ability  -> the typed direction stands (true move)
//   - otherwise               -> a random declared exit is substituted
// A mover with no orientation skill has ability 0, so the maze always
// scrambles for them. If the scramble happens to land on the direction
// they typed, a mover who does not yet know orientation learns it here
// at the 10% floor.

#include <translations/skills.h>

inherit room "/lib/room.c";

int do_exit_command(string str, varargs mixed verb, object ob)
{
  string * dirs;
  string typed, chosen;
  int ability;

  // Same defaulting the exits handler applies, so the orientation
  // check always sees a real mover and verb.
  if (!verb)
    verb = query_verb();
  if (!ob)
    ob = this_player();

  typed = verb;
  ability = 0;
  if (ob)
    ability = ob->query_skill_ability(_LANG_SKILL_ORIENTATION_NAME);
  // A mover lacking the skills mixin returns nil; coerce to 0.
  if (!ability)
    ability = 0;

  // Failed orientation check: substitute a random declared exit.
  if (random(100) >= ability)
  {
    dirs = query_direc();
    if (dirs && sizeof(dirs) > 1)
    {
      chosen = dirs[random(sizeof(dirs))];
      verb = chosen;

      // Lucky hit: the scramble landed on the direction the mover
      // typed. A mover without the skill picks it up here; the starting
      // ability (10%) comes from the skills table.
      if (chosen == typed && ob &&
          !ob->query_known_skill(_LANG_SKILL_ORIENTATION_NAME))
        ob->add_known_skill(_LANG_SKILL_ORIENTATION_NAME);
    }
  }

  return room::do_exit_command(str, verb, ob);
}
