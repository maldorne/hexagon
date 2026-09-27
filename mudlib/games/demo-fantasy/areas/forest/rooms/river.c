// The river below the Mallorn, where the end of the demo plays out. Nobody walks
// in: whoever the old man sent up the tree is moved here from the top of it,
// and everything that happens from then on happens in this room -- the branch
// breaks, the fall, the current, waking up on the bank, which completes the
// quest that sent them up.
//
// Each step checks the one who entered is still here, and stops if not.

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

// Whoever arrives from somewhere falls: coming back to the game after quitting
// here has no previous room, and that is not a fall.
void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  ::event_enter(who, msg, from, avoid);

  if (!living(who) || !from)
    return;

  who->add_timed_property(PASSED_OUT_PROP, _LANG_RIVER_PASSED_CLIMBING, 10);
  call_out("branch_breaks", 2, who, from);
}

void branch_breaks(object who, object from)
{
  if (!who || environment(who) != this_object())
    return;

  tell_object(who, _LANG_RIVER_BRANCH_ME);
  call_out("fall_into_river", 3, who, from);
}

void fall_into_river(object who, object from)
{
  if (!who || environment(who) != this_object())
    return;

  tell_object(who, _LANG_RIVER_FALL_ME);

  // whoever stayed below sees the branch come down
  if (from)
    tell_room(from, _LANG_RIVER_FALL_ROOM);

  // out cold until the current leaves them on the bank
  who->add_timed_property(PASSED_OUT_PROP, _LANG_RIVER_PASSED_UNCONSCIOUS, 30);

  call_out("carried_away", 8, who);
}

void carried_away(object who)
{
  if (!who || environment(who) != this_object())
    return;

  tell_object(who, _LANG_RIVER_CARRIED_ME);
  call_out("wake_up", 8, who);
}

void wake_up(object who)
{
  if (!who || environment(who) != this_object())
    return;

  who->remove_timed_property(PASSED_OUT_PROP);
  tell_object(who, _LANG_RIVER_WAKE_ME);

  handler(QUESTS_HANDLER, who)->complete(who, QUEST_CLIMB);

  // queued, so it shows once the rest has been told
  who->do_look();
}
