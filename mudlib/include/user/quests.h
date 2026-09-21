#ifndef USER_QUESTS_H
#define USER_QUESTS_H

// Per-player quest state: the quests being done and the ones handed in, per
// game. Stored in its own savefile (see lib/player/quests.c and
// lib/obj/player/quests.c), separate from player.o, because it has no ceiling:
// a repeatable quest is handed in as many times as the player cares to.

#define QUESTS_OB   "/lib/obj/player/quests.c"
#define QUESTS_SAVE "quests.o"

// Keys of the per-game sub-mapping.
#define QUESTS_ACTIVE "active"
#define QUESTS_DONE   "done"

// Keys of one active quest.
#define QUEST_PROGRESS "progress"
#define QUEST_SOURCE   "source"
#define QUEST_STARTED  "started"

// Keys of one handed-in quest.
#define QUEST_TIMES "times"
#define QUEST_LAST  "last"

#endif
