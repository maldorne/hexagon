// The old ranger's climbing rope is still in the forest, in the web of the
// spider that drove him out of it years ago. Bringing that fight to an end is
// what earns his teaching: he cannot climb any more, and what he knows would
// otherwise die with him.

#include <living/quests.h>
#include <living/skills.h>
#include <language.h>

inherit QUEST_BASE;

void setup()
{
  set_id("demo-fantasy:the-rangers-rope");
  set_title(_LANG_QUEST_ROPE_TITLE);
  set_description(_LANG_QUEST_ROPE_DESC);

  set_chain("demo-fantasy:the-climb", 3);

  // he asks for it and he takes it back
  set_hand_in("areas/road/npcs/old-man");
  set_complete_message(_LANG_QUEST_ROPE_COMPLETE);

  add_needs_quest("demo-fantasy:news-for-the-old-man");

  add_objective(OBJECTIVE_KILL, "areas/forest/npcs/spider-queen", 1,
                _LANG_QUEST_ROPE_OBJECTIVE);

  add_reward(REWARD_SKILL, SKILL_CLIMB);
  add_reward(REWARD_XP, 500);
}
