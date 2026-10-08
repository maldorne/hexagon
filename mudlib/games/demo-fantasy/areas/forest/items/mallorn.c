// Edited by Lummen 21-9-97
// Rewritten for Hexagon, neverbot 09/2026: climbed with the climb skill, and the
//   ending of the demo starts here for whoever is on the quest that sends them up

#include <living/quests.h>
#include <language.h>
#include "../path.h"

inherit "/lib/item.c";

#define QUEST_CLIMB "demo-fantasy:climb-the-mallorn"
#define RIVER       ROOMS + "river.c"

void setup()
{
  set_name("mallorn");
  set_short("%^GREEN%^BOLD%^Mallorn%^RESET%^");
  add_alias(_LANG_MALLORN_ALIASES);

  set_main_plural("%^ORANGE%^Mallorns%^RESET%^");
  add_plural(_LANG_MALLORN_PLURALS);

  set_long(_LANG_MALLORN_LONG);
  reset_get();
}

// What the climb skill asks of anything it is used on.
int query_climbable() { return 1; }

// Its branches reach the ground and hold anybody: the way out of the demo does
// not depend on a roll.
int query_climb_modifier(object climber) { return 100; }

void event_climbed(object climber)
{
  string game;

  game = game_name(climber);

  // whoever already finished the quest falls again, so the way out of the demo
  // stays open if something brought them back to the forest
  if (!climber->is_doing_quest(game, QUEST_CLIMB) &&
      !climber->has_completed_quest(game, QUEST_CLIMB))
  {
    tell_object(climber, _LANG_MALLORN_VIEW);
    return;
  }

  // for whoever the old man sent, this is the end of the demo
  tell_object(climber, _LANG_MALLORN_CLIMB_ME);
  tell_room(environment(climber), _LANG_MALLORN_CLIMB_ROOM, climber);

  // up there is the objective of the quest that sent them
  handler(QUESTS_HANDLER, climber)->reached(climber, "areas/forest/items/mallorn");

  // the fall is told by the river, where they end up
  climber->move(RIVER);
}
