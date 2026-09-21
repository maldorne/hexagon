// The old ranger of Naduk will teach somebody to climb, but only after they have
// been into the forest and come back: he wants wasps' nests brought to him, which
// means climbing for them. Handing it in is what teaches the skill.

#include <living/quests.h>
#include <living/skills.h>
#include <language.h>

inherit QUEST_BASE;

void setup()
{
  set_id("demo-fantasy:learn-to-climb");
  set_title(_LANG_QUEST_CLIMB_TITLE);
  set_description(_LANG_QUEST_CLIMB_DESC);

  // the old man takes it back himself
  set_hand_in("old-man");

  add_objective(OBJECTIVE_KILL, "wasp", 3, _LANG_QUEST_CLIMB_OBJECTIVE);

  add_reward(REWARD_SKILL, SKILL_CLIMB);
  add_reward(REWARD_XP, 500);
}
