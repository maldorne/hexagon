// The river below the Mallorn, where the end of the demo plays out. Nobody walks
// in: whoever the old man sent up the tree falls with a branch, is carried here
// by the current and wakes up on the bank, and that completes the quest that
// sent them up.
//
// The steps run as call_outs in this room, which stays loaded, rather than in
// the tree, which a reset may take away halfway. Each step checks the player is
// still where the previous one left them, and stops if not.

#include <common/properties.h>
#include <living/quests.h>
#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

#define QUEST_CLIMB "demo-fantasy:climb-the-mallorn"

void setup()
{
  set_short(_LANG_RIVER_SHORT);
  set_long(_LANG_RIVER_LONG);
  set_light(60);
  add_item(_LANG_FOREST_RIVER_ITEMS, _LANG_FOREST_RIVER_DESC);
  add_exit(DIR_NORTHEAST, ROOMS + "38.c", "forest");
}

// Up in the Mallorn, somebody on the quest has just reached the crown. The
// branch gives way a moment later.
void start_fall(object player, object from)
{
  player->add_timed_property(PASSED_OUT_PROP, _LANG_RIVER_PASSED_CLIMBING, 10);
  call_out("branch_breaks", 4, player, from);
}

void branch_breaks(object player, object from)
{
  if (!player || environment(player) != from)
    return;

  tell_object(player, _LANG_RIVER_BRANCH_ME);
  call_out("fall_into_river", 3, player, from);
}

void fall_into_river(object player, object from)
{
  if (!player || environment(player) != from)
    return;

  tell_object(player, _LANG_RIVER_FALL_ME);
  tell_room(from, _LANG_RIVER_FALL_ROOM, player);

  // out cold until the current leaves them somewhere
  player->add_timed_property(PASSED_OUT_PROP, _LANG_RIVER_PASSED_UNCONSCIOUS, 30);
  player->move(this_object());

  call_out("carried_away", 8, player);
}

void carried_away(object player)
{
  if (!player || environment(player) != this_object())
    return;

  tell_object(player, _LANG_RIVER_CARRIED_ME);
  call_out("wake_up", 8, player);
}

void wake_up(object player)
{
  if (!player || environment(player) != this_object())
    return;

  player->remove_timed_property(PASSED_OUT_PROP);
  tell_object(player, _LANG_RIVER_WAKE_ME);

  handler(QUESTS_HANDLER, player)->complete(player, QUEST_CLIMB);

  // queued, so it shows once the rest has been told
  player->do_look();
}
