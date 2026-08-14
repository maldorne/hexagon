// guard.c -- the guard-role behaviour, as an NPC component.
//
// Watching an exit and stopping enemy citizenships is a role, not a race or a
// class, so it is a component an NPC can be given (a citizenship's guards get it
// when the area posts them) rather than a base class. This replaces the old
// /lib/npc/guardian.c mixin and its /lib/guard.c base: a guard is now a plain
// generic NPC carrying this component.
//
// The room side is unchanged -- the exit handler still calls
// guard_npc->guardian_check(mover) / guardian_message(); npc.c proxies those to
// this component. The composing NPC supplies query_city_ob() (its citizenship),
// which this component reads through query_owner().

#include <areas/diplomacy.h>

inherit component "/lib/npc/component.c";

// The exit direction this guard watches. nil until posted (an unposted guard
// blocks nobody). Persisted so a restored guard keeps its post; the area also
// re-stamps it on every materialization, so a changed guard_dir is picked up.
string guard_direction;

void set_direction(string dir) { guard_direction = dir; }
string query_direction() { return guard_direction; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "direction" : guard_direction ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (args && !undefinedp(args["direction"]))
    guard_direction = args["direction"];
}

// Let `mover` through the watched exit? Blocks only citizenships this guard's
// citizenship is at war with, so a diplomacy change flips who is stopped without
// touching the guard. With no direction posted, or either side lacking a
// citizenship, the mover passes.
int check(object mover)
{
  string my_city_ob, mover_city_ob;
  object me, my_citizenship, mover_citizenship;

  me = query_owner();
  if (!guard_direction || !mover || !me)
    return 1;

  // a mover with no citizenship is neutral and passes
  my_city_ob = me->query_city_ob();
  mover_city_ob = mover->query_city_ob();
  if (!my_city_ob || !mover_city_ob)
    return 1;

  // city_ob is the citizenship object's path; load it and read its name and
  // game -- diplomacy works in names, and resolves the parent cascade
  my_citizenship = load_object(my_city_ob);
  mover_citizenship = load_object(mover_city_ob);
  if (!my_citizenship || !mover_citizenship)
    return 1;

  return !DIPLOMACY_HANDLER->is_enemy(game_name(my_citizenship),
                                      my_citizenship->query_name(),
                                      mover_citizenship->query_name());
}

// Message shown when the guard stops someone. nil falls back to the exit
// handler's default.
string message() { return nil; }
