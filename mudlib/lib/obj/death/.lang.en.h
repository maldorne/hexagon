
// corpse.c

#define _LANG_CORPSE_SOMEBODY "somebody"
#define _LANG_CORPSE_NAME "corpse"
#define _LANG_CORPSE_ALIASES ({ "corpse", "body", "remains" })
#define _LANG_CORPSE_PLURALS ({ "corpses", "bodies", "remains" })
#define _LANG_CORPSE_DESC "It is a corpse.\n"

#define _LANG_CORPSE_OF "corpse of "
#define _LANG_CORPSES_OF "corpses of "
#define _LANG_CORPSE_DESC_OF "This is the dead body of "+capitalize(n)+".\n"
#define _LANG_CORPSE_RAISE_MSG "You hear the howling of a tortured spirit.\n" + \
    "Suddenly, a " + cow->query_name() + " rises from the dead.\n"
#define _LANG_CORPSE_UNKNOWN_BEIGN "unknown beign"
#define _LANG_CORPSE_REMAINS "remains of "+query_name()
#define _LANG_CORPSE_ROTTEN_REMAINS "rotten remains of a "+query_name()
#define _LANG_CORPSE_REMAINS_DESC "This is the dead body of a "+race_name+".\n"

// base-undead.c

#define _LANG_UNDEAD_FRIEND_MSG "The " + this_object()->query_cap_name() + \
    " sighs and returns to the undead world.\n"

#define _LANG_SKELETON_NAME "skeleton"
#define _LANG_SKELETON_DESC "Nothing more than an animate skeleton, this fiend has returned to " + \
       "life to visit woe on the living.\n"

#define _LANG_ZOMBIE_NAME "zombie"
#define _LANG_ZOMBIE_DESC "Nothing more than an animate skeleton, this fiend has returned to " + \
       "life to visit woe on the living.\n"

#define _LANG_SPECTRE_NAME "spectre"
#define _LANG_SPECTRE_DESC "Nothing more than a moaning patch of air with a bad attitude, this fiend has returned to " + \
       "life to visit woe on the living.\n"

#define _LANG_WRAITH_NAME "wraith"
#define _LANG_WRAITH_DESC "A misty shape reaches out for you, unpleasant memories " + \
       "roil up in your mind as this undead thing tries to drag you down.\n"

#define _LANG_LICH_NAME "lich"
#define _LANG_LICH_DESC "You can see nothing but a dark cloak like area with  " + \
       "glowing eyes that follow you as you move.\n"

// body-part.c

#define _LANG_BODY_PART_UNKNOWN_SOURCE "an unknown being"
#define _LANG_BODY_PART_SHORT capitalize(name) + " of " + source
#define _LANG_BODY_PART_PLURAL_SHORT capitalize(plural) + " of " + source
#define _LANG_BODY_PART_LONG (query_plural_noun() ? "These are " : "This is ") + \
      query_article() + " " + name + " of " + source + ", cut from the body.\n"
#define _LANG_BODY_PART_STATE_VERB (plural ? "They are" : "It is")
#define _LANG_BODY_PART_STATE_FRESH _LANG_BODY_PART_STATE_VERB + " fresh.\n"
#define _LANG_BODY_PART_STATE_STALE (plural ? "They begin" : "It begins") + " to smell.\n"
#define _LANG_BODY_PART_STATE_ROTTEN _LANG_BODY_PART_STATE_VERB + " rotten.\n"
