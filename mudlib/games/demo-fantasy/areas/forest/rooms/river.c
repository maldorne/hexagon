// The river below the Mallorn, where the end of the demo plays out. Nobody walks
// in: whoever the old man sent up the tree is moved here from the top of it,
// and everything that happens from then on happens in this room -- the branch
// breaks, the fall, the current, waking up on the bank, which completes the
// quest that sent them up.
//
// Each step checks the one who entered is still here, and stops if not.
//
// There is no way back into the forest: from the bank the character leaves the
// demo for one of the games the games handler finds for it, and chooses which.
// Being here is being done with the demo, however one got here, so nothing in
// this room asks for the quest before letting anybody go on.

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
}

void init()
{
  ::init();
  add_action("do_choose", _LANG_RIVER_CHOOSE_VERBS);
}

// Tells the one on the bank where the river may take them, and how to choose.
void show_destinations(object who)
{
  object * games;
  string ret;
  int i, width;

  if (!who || environment(who) != this_object() || !who->user())
    return;

  games = handler("games")->query_transfer_destinations(who);

  if (!sizeof(games))
  {
    tell_object(who, _LANG_RIVER_NO_DESTINATIONS);
    return;
  }

  // the width left for a description once its indentation is taken out
  width = (who->user()->query_cols() ? who->user()->query_cols() : 80) - 16;
  ret = _LANG_RIVER_DESTINATIONS;

  for (i = 0; i < sizeof(games); i++)
    ret += "\n   %^BOLD%^" + (i + 1) + ") %^CYAN%^" +
           games[i]->query_game_name() + "%^RESET%^\n" +
           sprintf("      %-" + width + "s",
                   games[i]->query_game_short_description()) + "\n";

  ret += "\n" + _LANG_RIVER_DESTINATIONS_HINT;

  // on a line of its own, never after the prompt that came before it
  tell_object(who, "\n" + handler("frames")->frame(ret));
}

// With no number, the list again: whoever missed it, came back linkdead or
// just forgot can always ask for it.
int do_choose(string str)
{
  object * games;
  int i;

  if (!str || !strlen(str))
  {
    show_destinations(this_player());
    return 1;
  }

  games = handler("games")->query_transfer_destinations(this_player());

  if (sscanf(str, "%d", i) != 1 || i < 1 || i > sizeof(games))
  {
    notify_fail(_LANG_RIVER_CHOOSE_FAIL);
    return 0;
  }

  // told before the move, which is for good
  write(wrap(_LANG_RIVER_LEAVING_ME, this_user()->query_cols()));
  tell_room(this_object(), _LANG_RIVER_LEAVING_ROOM, this_player());

  // the destination's start room did not load: nothing has changed yet
  if (!handler("games")->transfer(this_player(), games[i - 1]))
    write(_LANG_RIVER_TRANSFER_FAILED);

  return 1;
}

// Whoever arrives from somewhere falls: coming back to the game after quitting
// here has no previous room, and that is not a fall -- that one wakes up on
// the bank straight away, wherever the fall had got to when they left.
void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  ::event_enter(who, msg, from, avoid);

  if (!living(who))
    return;

  if (!from)
  {
    call_out("on_the_bank", 1, who);
    return;
  }

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

// Where every way in ends: awake, done with the quest if it was still open,
// and told where the river may take them from here.
void on_the_bank(object who)
{
  if (!who || environment(who) != this_object())
    return;

  // a fall cut short by quitting may have left them out cold
  who->remove_timed_property(PASSED_OUT_PROP);

  // does nothing for somebody who was not on it
  handler(QUESTS_HANDLER, who)->complete(who, QUEST_CLIMB);

  call_out("show_destinations", 2, who);
}

void wake_up(object who)
{
  if (!who || environment(who) != this_object())
    return;

  tell_object(who, _LANG_RIVER_WAKE_ME);

  // queued, so it shows once the rest has been told
  who->do_look();

  on_the_bank(who);
}
