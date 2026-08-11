/*
 * Static guard placement for rooms.
 *
 * A room declares its guards in setup() with add_guard(file, howmany,
 * direction); reset() clones them into the room and registers each with the
 * shared exit-protection layer (/lib/room/guarded_exits.c), so the exit
 * handler blocks the guarded direction (guardian_check). This is the room-only
 * half (the cloning) of the old /lib/room/guard.c; the enforcement half moved
 * to guarded_exits.c so locations can share it without the cloning.
 *
 *   Folken 15/04/2009 (split into guarded_exits + room_guards, neverbot 2026)
 */

inherit guarded "/lib/room/guarded_exits.c";

// the static declaration: ({ ({ file_name, howmany, direction }) })
static mixed * guard_files;
// clones we created, as ({ ({ clone, file, direction }) }), so reset can top
// up only the ones that died and dest_me can clean them up
static mixed * guard_clones;

void create()
{
  guarded::create();
  guard_files = ({ });
  guard_clones = ({ });
}

mixed * query_guard_files() { return guard_files; }

// Declare howmany guards of file_name protecting an exit direction. The guards
// are cloned later, on reset (room load), not here.
int add_guard(string file_name, int howmany, string direction)
{
  if (!howmany)
    return 0;

  if (!stringp(file_name) || !load_object(file_name))
  {
    log_file("room.log", "(add_guard) bad guard file: " + file_name
        + ", " + file_name(this_object())
        + " [" + ctime(time()) + "]\n");
    return 0;
  }

  guard_files += ({ ({ file_name, howmany, direction }) });
  return howmany;
}

// Clone the declared guards into the room and register each as protecting its
// direction. Called on room load / reset. Prunes clones that have died, then
// tops each declaration back up to its count, so a killed guard is replaced on
// the next reset (matching the old behaviour).
void reset()
{
  int i, j, live;
  mixed * kept;

  // drop dead clones (a destructed object reads back as nil in the array)
  kept = ({ });
  for (i = 0; i < sizeof(guard_clones); i++)
    if (guard_clones[i][0])
      kept += ({ guard_clones[i] });
  guard_clones = kept;

  for (i = 0; i < sizeof(guard_files); i++)
  {
    string file, dir;
    int howmany;

    file = guard_files[i][0];
    howmany = guard_files[i][1];
    dir = guard_files[i][2];

    // how many of this file+direction are already standing here
    live = 0;
    for (j = 0; j < sizeof(guard_clones); j++)
      if (guard_clones[j][1] == file && guard_clones[j][2] == dir)
        live++;

    for (j = live; j < howmany; j++)
    {
      object g;
      g = clone_object(file);
      if (!g)
        continue;
      g->move(this_object());
      guard_clones += ({ ({ g, file, dir }) });
      register_guard(g, dir);
    }
  }
}

void dest_me()
{
  int i;

  // destruct the guards we cloned (a location's guards are not ours to destruct
  // -- but a room owns its clones)
  for (i = 0; i < sizeof(guard_clones); i++)
    if (guard_clones[i][0] && guard_clones[i][0]->query_npc())
      guard_clones[i][0]->dest_me();

  guard_clones = ({ });
  guard_files = ({ });
  clear_guards();
}

mixed * stats()
{
  return guarded::stats() +
         ({ ({ "Guard files", guard_files, }),
            ({ "Guard clones", guard_clones, }) });
}
