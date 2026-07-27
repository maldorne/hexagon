
#include <room/room.h>

private static int room_create_time;  // time of creation
private static int room_init_time;    // time the room was last entered / used

void create()
{
  room_create_time = time();
  room_init_time = time();
}

void init()
{
  room_init_time = time();
}

// Kept for callers that refresh the "last used" stamp when the room is
// touched (exits.c calls this on movement). There is no per-room reaper
// call_out any more — the location cleaner owns reclamation now.
void update_clean_up()
{
  room_init_time = time();
}

// Seconds since the room was last entered / touched. An idle signal the
// cleaner can read.
int query_room_idle()
{
  return time() - room_init_time;
}

// Passive eviction entry point. The cleaner decides WHEN to reclaim a room;
// this method decides HOW, and may refuse. Returns 1 to keep the room, 0
// when it tore itself down. `flag` is accepted for signature compatibility
// with legacy callers.
int clean_up(varargs int flag)
{
  object * arr;
  int i;

  // pinned, or a corpse is resting here -> never reclaim
  if (this_object()->query_property(NO_CLEAN_UP_PROP))
    return 1;
  if (this_object()->query_property("corpse_here"))
    return 1;

  // never reclaim a room a real player is standing in
  arr = deep_inventory(this_object());
  for (i = 0; i < sizeof(arr); i++)
    if (arr[i] && userp(arr[i]))
      return 1;

  this_object()->dest_me();
  return 0;
}
