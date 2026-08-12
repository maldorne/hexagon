// guardian.c -- the guard-role behaviour, as a reusable mixin.
//
// Watching an exit and stopping enemy citizenships is a role, not a race or a
// class, so it lives here as a small inheritable unit rather than on any NPC
// base -- the same split as /lib/room/guarded_exits.c on the room side. A base
// that wants the role (the persisted area guard /lib/guard.c, or a hand-made
// monster-based guard) inherits this; a plain monster does not, and blocks
// nobody.
//
// The composing base supplies query_city_ob() (every living has it); it is
// prototyped here so the mixin compiles on its own.

#include <areas/diplomacy.h>

// supplied by the living the mixin is composed into
string query_city_ob();

// The exit direction this guard watches. nil until posted (an unposted guard
// blocks nobody). Transient: whoever places the guard re-stamps it on every
// materialization, so it is static and never saved.
static string guardian_direction;

void set_guardian_direction(string dir) { guardian_direction = dir; }
string query_guardian_direction() { return guardian_direction; }

// Let `mover` through the watched exit? Blocks only citizenships this guard's
// own is at war with; its own citizens, allies and neutrals (no citizenship)
// pass. Diplomacy is the single oracle, so an invasion that flips relations
// changes who is stopped without touching the guard. With no direction posted
// yet, or no mover, let everyone through.
int guardian_check(object mover)
{
  string my_city_ob, mover_city_ob;
  object my_citizenship, mover_citizenship;

  if (!guardian_direction || !mover)
    return 1;

  // a mover with no citizenship is neutral and passes
  my_city_ob = query_city_ob();
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
// handler's default ("... te impide el paso.").
string guardian_message() { return nil; }
