// The last step: the old man sends whoever he taught to the one tree worth
// climbing. The Mallorn itself is what takes this quest back, at the top, and
// what happens up there is the end of the demo.

#include <living/quests.h>
#include <language.h>

inherit QUEST_BASE;

void setup()
{
  set_id("demo-fantasy:climb-the-mallorn");
  set_title(_LANG_QUEST_MALLORN_TITLE);
  set_description(_LANG_QUEST_MALLORN_DESC);

  set_chain("demo-fantasy:the-climb", 4);

  // the tree itself is what closes it, where it stands
  set_completed_by("areas/forest/items/mallorn");

  add_needs_quest("demo-fantasy:the-rangers-rope");

  add_reward(REWARD_XP, 1000);
}
