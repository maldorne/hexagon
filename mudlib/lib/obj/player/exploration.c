// Per-player exploration savefile helper. Cloned on demand by the player's
// exploration component to read or write /save/players/<l>/<name>/exploration.o,
// then destructed. Keeping the disk data in a standalone object lets the
// player component hold it in a static (unsaved) in-memory mapping, so the
// exploration data never bloats player.o.

#include <user/exploration.h>

inherit "/lib/core/object";

mapping data;

void create()
{
  ::create();
  data = ([ ]);
}

mapping query_data() { return data; }
void set_data(mapping m) { data = m; }

// Restore this player's exploration savefile into `data`. Returns 1 on success.
int restore_data(string name)
{
  return restore_object(player_save_dir(name) + EXPLORATION_SAVE, 1);
}

// Persist `data` to this player's exploration savefile. save_object does not
// create directories, so ensure the per-player folder exists first.
int save_data(string name)
{
  mkdir(player_save_dir(name));
  return save_object(player_save_dir(name) + EXPLORATION_SAVE, 1);
}
