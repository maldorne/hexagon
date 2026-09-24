// The old ranger asked to be told how the hunt was going, and he is somewhere
// out on the road by the forest. The errand is to find him, so the quest has no
// objectives: reaching him is all there is, and it is handed in to him. He
// repays the news by teaching what he knows best.

#include <living/quests.h>
#include <living/skills.h>
#include <language.h>

inherit QUEST_BASE;

void setup()
{
  set_id("demo-fantasy:news-for-the-old-man");
  set_title(_LANG_QUEST_NEWS_TITLE);
  set_description(_LANG_QUEST_NEWS_DESC);

  set_chain("demo-fantasy:the-climb", 2);

  // he is the one to be found, and the one it is handed in to
  set_hand_in("areas/road/npcs/old-man");

  // what he says as he takes the news, and teaches what he pays with
  set_complete_message(_LANG_QUEST_NEWS_COMPLETE);

  add_needs_quest("demo-fantasy:wasps-ruin-the-hunt");

  add_reward(REWARD_SKILL, SKILL_CLIMB);
  add_reward(REWARD_XP, 500);
}
