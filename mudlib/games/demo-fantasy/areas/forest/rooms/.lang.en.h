
// rooms

#define _LANG_FOREST_SHORT "%^GREEN%^Forest%^RESET%^"
#define _LANG_FOREST_LONG "You are in a giant fir tree forest. The trees " + \
    "block sunlight from fully entering the area, causing a feeling of " + \
    "mystery and danger. The silence of the forest worries you at times.\n"
#define _LANG_FOREST_TREE_ITEMS ({ "trees", "tree" })
#define _LANG_FOREST_TREE_DESC "Very big fir trees, partially covered with " + \
    "other smaller plants and some moss.\n"

#define _LANG_MALLORN_SHORT _LANG_FOREST_SHORT + ": Mallorn"
#define _LANG_MALLORN_LONG "A low hill hidden in the middle of the forest, " + \
    "without any other tree except for an old Mallorn. Besides the hill, a small river runs.\n"
#define _LANG_FOREST_RIVER_ITEMS ({ "river" })
#define _LANG_FOREST_RIVER_DESC "A small river with crystal clear waters.\n"

#define _LANG_FOREST_NEST_SHORT _LANG_FOREST_SHORT + ": %^ORANGE%^Spider Nest%^RESET%^"
#define _LANG_FOREST_NEST_LONG "You enter the darkest part of the forest, due to the " + \
    "numerous cobwebs woven to block the sunlight. Fear " + \
    "goes through your body when you see the place.\n"
#define _LANG_FOREST_WEB_ITEMS ({ "cobwebs", "cobweb" })
#define _LANG_FOREST_WEB_DESC "More robust than normal, indicating that you probably " + \
    "are in a nest.\n"

#define _LANG_DEEP_FOREST_SHORT "%^GREEN%^Deep Forest%^RESET%^"
#define _LANG_DEEP_FOREST_LONG "In this part of the forest the trees are more numerous. As " + \
     "you enter into the deep forest, you feel like the " + \
     "trees were cutting your way, as if you weren't able to move forward. " + \
     "You wonder when you will be able to get out.\n"

// river.c: the end of the demo

#define _LANG_RIVER_SHORT _LANG_FOREST_SHORT + ": River bank"
#define _LANG_RIVER_LONG "A bend of the river, below the Mallorn's hill. The " + \
    "current leaves here everything it carries: branches, leaves and, now and " + \
    "then, somebody unwary.\n"
#define _LANG_RIVER_PASSED_CLIMBING "You are up in the Mallorn, holding on to a branch.\n"
#define _LANG_RIVER_PASSED_UNCONSCIOUS "You are unconscious.\n"
#define _LANG_RIVER_BRANCH_ME "\nThe branch you are leaning on creaks under your " + \
    "weight. You reach for another, but not in time.\n"
#define _LANG_RIVER_FALL_ME "\nCRAAASH! You fall into the river with the branch and " + \
    "hit your head on a stone. The last thing you feel is the cold water carrying " + \
    "you off.\n"
#define _LANG_RIVER_FALL_ROOM "Something cracks among the Mallorn's branches: " + \
    player->query_cap_name() + " falls into the river with a branch and the " + \
    "current takes them away.\n"
#define _LANG_RIVER_CARRIED_ME "\nHalf asleep you feel the cold, the noise of the " + \
    "water and the knocks of branches and stones. You cannot tell how long it lasts.\n"
#define _LANG_RIVER_WAKE_ME "\nYou wake up lying on a bank, soaked and with a nasty " + \
    "bump. You do not quite remember how you got here.\n"
