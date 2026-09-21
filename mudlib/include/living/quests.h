#ifndef QUESTS_H
#define QUESTS_H

#define QUESTS_HANDLER "quests"

// where a quest that is registered wrong is reported
#define QUESTS_LOG "quests"

// The base a quest definition inherits.
#define QUEST_BASE "/lib/quest.c"

// What an objective asks for. The target of an objective is always a stable
// identifier -- a template id or a file path -- never a name a player reads,
// so translating or renaming something cannot break a quest.
#define OBJECTIVE_KILL  "kill"
#define OBJECTIVE_GET   "get"
#define OBJECTIVE_REACH "reach"
#define OBJECTIVE_TALK  "talk"

// Keys of one objective.
#define OBJ_KIND   "kind"
#define OBJ_TARGET "target"
#define OBJ_COUNT  "count"
#define OBJ_TEXT   "text"

// What a quest pays. The handler grants these and nothing else, so a quest
// never hands out a reward by itself: whatever it pays is declared, and is only
// paid once the hand-in has been accepted.
#define REWARD_XP     "xp"
#define REWARD_JOB_XP "job-xp"
#define REWARD_MONEY  "money"
#define REWARD_ITEM   "item"
#define REWARD_TITLE  "title"
#define REWARD_SKILL  "skill"

// Keys of one reward.
#define REWARD_KIND  "kind"
#define REWARD_VALUE "value"

// Why a quest cannot be taken or handed in, as answered by the handler.
#define QUEST_OK           0
#define QUEST_UNKNOWN      1
#define QUEST_ALREADY      2
#define QUEST_DONE         3
#define QUEST_NEEDS        4
#define QUEST_INCOMPLETE   5
#define QUEST_WRONG_PLACE  6

#endif
