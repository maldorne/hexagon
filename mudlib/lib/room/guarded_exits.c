/*
 * Exit protection, shared between rooms and locations.
 *
 * Holds the registry of which live guard NPCs protect which exit direction,
 * and exposes query_guards() in the shape the exit handler consumes
 * (exit_handler.c reads room_ob->query_guards() and calls each guard's
 * guardian_check(ob) when someone tries to leave through the guarded
 * direction). Nothing here clones or places the guards: that is
 * placement-specific. A static room declares and clones them at load (see
 * /lib/room/room_guards.c); a location's diplomacy system materialises them
 * and registers them as it goes.
 *
 * Extracted from the old /lib/room/guard.c so both /lib/room.c and
 * /lib/location.c can inherit the enforcement half without the room-only
 * cloning half.
 */

// flat and interleaved: ({ guard0, dir0, guard1, dir1, ... })
static mixed * guarded_exit_slots;

void create()
{
  guarded_exit_slots = ({ });
}

// Drop guards that have been destructed or moved out of this room/location, so
// the registry never points at a guard that is no longer standing here.
private void _prune_guards()
{
  mixed * out;
  int i;

  if (!guarded_exit_slots)
  {
    guarded_exit_slots = ({ });
    return;
  }

  out = ({ });
  for (i = 0; i < sizeof(guarded_exit_slots); i += 2)
    if (guarded_exit_slots[i] &&
        environment(guarded_exit_slots[i]) == this_object())
      out += ({ guarded_exit_slots[i], guarded_exit_slots[i + 1] });

  guarded_exit_slots = out;
}

// Register a live guard NPC as protecting `direction`. Idempotent per guard.
void register_guard(object guard, string direction)
{
  if (!guard || !direction)
    return;

  _prune_guards();

  // a guard protects one direction; skip if already registered
  if (member_array(guard, guarded_exit_slots) == -1)
    guarded_exit_slots += ({ guard, direction });
}

// Remove a guard from the registry (it left, died, or was reassigned).
void unregister_guard(object guard)
{
  mixed * out;
  int i;

  out = ({ });
  for (i = 0; i < sizeof(guarded_exit_slots); i += 2)
    if (guarded_exit_slots[i] != guard)
      out += ({ guarded_exit_slots[i], guarded_exit_slots[i + 1] });

  guarded_exit_slots = out;
}

// The structure exit_handler consumes: a list of guard groups, each a flat
// ({ obj, dir, obj, dir, ... }). We expose every live guard as a single group;
// the handler iterates the groups and steps k += 2 within each, so one group
// with all the pairs matches its enforcement loop.
mixed * query_guards()
{
  _prune_guards();

  if (!sizeof(guarded_exit_slots))
    return ({ });

  return ({ guarded_exit_slots });
}

// Forget the registry. Named clear_guards (not dest_me) so it does not join
// the inheritor's dest_me resolution -- location.c/room.c call it explicitly
// from their own dest_me. The guards themselves are owned by whoever placed
// them (a room destructs its clones in room_guards.c; a location's guards are
// area-managed NPCs, drained on unload).
void clear_guards()
{
  guarded_exit_slots = ({ });
}

mixed * stats()
{
  return ({ ({ "Guarded exit slots", guarded_exit_slots, }) });
}
