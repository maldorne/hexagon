
// Per-skill display strings (English). The learning key is the English id
// in <living/skills.h>; these are only the player-visible name and help.

#define _LANG_SKILL_ORIENTATION_NAME "orientation"
#define _LANG_SKILL_ORIENTATION_ALIASES ({ })
#define _LANG_SKILL_ORIENTATION_HELP "This skill measures your ability to " + \
    "orient yourself and find your way, so you reach your destination safe " + \
    "and sound.\n"

#define _LANG_SKILL_SEARCH_NAME "search"
#define _LANG_SKILL_SEARCH_ALIASES ({ })
#define _LANG_SKILL_SEARCH_HELP "This skill lets you search your surroundings " + \
    "for hidden things. The better you are at it, the more you will find.\n"
#define _LANG_SKILL_SEARCH_START "You search your surroundings carefully, " + \
    "trying to find something hidden.\n"
#define _LANG_SKILL_SEARCH_ROOM " searches around.\n"

#define _LANG_SKILL_HIDE_NAME "hide"
#define _LANG_SKILL_HIDE_ALIASES ({ })
#define _LANG_SKILL_HIDE_HELP "This skill lets you take a quick look around and try " + \
    "to slip out of sight behind whatever cover you find. The better your skill, " + \
    "the easier it is to find a hiding spot, and darkness helps. Succeeding costs " + \
    "extra effort.\n"
#define _LANG_SKILL_HIDE_START "You quickly look for a place to hide."
#define _LANG_SKILL_HIDE_ROUND1 "You try to find somewhere to conceal yourself.\n"
#define _LANG_SKILL_HIDE_SUCCESS "You slip quietly out of sight.\n"
#define _LANG_SKILL_HIDE_FAIL "You cannot find anywhere to hide.\n"

// hide shadow (placed on a hidden character)
#define _LANG_HIDE_REVEALED_ROOM_PRE "\n\t%^BOLD%^You notice "
#define _LANG_HIDE_REVEALED_ROOM_POST " moving around... they were hiding!%^RESET%^\n"
#define _LANG_HIDE_REVEALED_YOU "You have been discovered!\n"
#define _LANG_HIDE_SEARCH_FOUND_PRE "You search around and spot "
#define _LANG_HIDE_SEARCH_FOUND_POST ", though they have not noticed you yet.\n"

#define _LANG_SKILL_CLIMB_NAME "climb"
#define _LANG_SKILL_CLIMB_ALIASES ({ "climbing" })
#define _LANG_SKILL_CLIMB_HELP "This skill measures how well you climb a wall, " + \
      "a tree or anything else you can get a grip on. The better you are at it, " + \
      "the less likely you are to fall.\n"
