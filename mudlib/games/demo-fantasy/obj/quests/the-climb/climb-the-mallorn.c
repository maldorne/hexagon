// The last step: the old man sends whoever he taught to the one tree worth
// climbing. Getting to the top is the objective; what happens up there is the
// end of the demo, and the river where the player wakes up completes it. Nobody
// takes it back, so it cannot be delivered without the fall.

#include <living/quests.h>
#include <language.h>

inherit QUEST_BASE;

void setup()
{
  set_id("demo-fantasy:climb-the-mallorn");
  set_title(_LANG_QUEST_MALLORN_TITLE);
  set_description(_LANG_QUEST_MALLORN_DESC);

  set_chain("demo-fantasy:the-climb", 4);

  add_needs_quest("demo-fantasy:the-rangers-rope");

  add_objective(OBJECTIVE_REACH, "areas/forest/items/mallorn", 1,
                _LANG_QUEST_MALLORN_OBJECTIVE);

  add_reward(REWARD_XP, 1000);
}
