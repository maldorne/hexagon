// The swarms nesting near the forest paths are driving the game away and the
// hunters cannot work. The village tavern keeper hands this out and takes it
// back; it is the first half of how somebody ends up learning to climb.

#include <living/quests.h>
#include <language.h>

inherit QUEST_BASE;

void setup()
{
  set_id("demo-fantasy:wasps-ruin-the-hunt");
  set_title(_LANG_QUEST_WASPS_TITLE);
  set_description(_LANG_QUEST_WASPS_DESC);

  add_objective(OBJECTIVE_KILL, "areas/forest/npcs/wasp", 3, _LANG_QUEST_WASPS_OBJECTIVE);

  add_reward(REWARD_XP, 300);
  add_reward(REWARD_MONEY, ({ 20, "copper" }));
}
