
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
#define _LANG_SKILL_CLIMB_ALIASES ({ })
#define _LANG_SKILL_CLIMB_HELP "With 'climb <something>' you try to get up a tree, " + \
      "a wall or anything else you can get a grip on. The better your skill, the " + \
      "less likely you are to slip, and some things are easier to climb than " + \
      "others.\n"
#define _LANG_SKILL_CLIMB_START "You look for somewhere to start up."
#define _LANG_SKILL_CLIMB_NOT_CLIMBABLE "There is nothing there to climb.\n"
#define _LANG_SKILL_CLIMB_ROUND1 "You get a good grip and start up.\n"
#define _LANG_SKILL_CLIMB_ROUND1_ROOM caster->query_cap_name() + \
      " starts climbing " + target->query_short() + ".\n"
#define _LANG_SKILL_CLIMB_SUCCESS "You get to the top with no trouble, have a look " + \
      "around and climb back down.\n"
#define _LANG_SKILL_CLIMB_FAIL "You slip and are back on the ground before " + \
      "getting very high.\n"

#define _LANG_SKILL_REPAIR_NAME "repair"
#define _LANG_SKILL_REPAIR_ALIASES ({ "mend" })
#define _LANG_SKILL_REPAIR_HELP "With 'repair <item>' you work a metal weapon, " + \
      "armour or shield you are carrying at a smithy's forge, to take the wear " + \
      "out of it. It can only be done while the forge is lit, and you pay for " + \
      "the metal and the coal it uses. The better your skill, the more wear you " + \
      "take out at once and the less the materials cost; if you fail, you only " + \
      "lose the effort.\n"
#define _LANG_SKILL_REPAIR_START "You set up the anvil and stir the forge's fire."
#define _LANG_SKILL_REPAIR_NO_FORGE "To repair you need a forge and an anvil: " + \
      "look for a smithy.\n"
#define _LANG_SKILL_REPAIR_FORGE_COLD "The forge is out; with nobody tending it " + \
      "you cannot work the metal.\n"
#define _LANG_SKILL_REPAIR_NOT_CARRIED "You have to be carrying what you want to repair.\n"
#define _LANG_SKILL_REPAIR_NOT_GEAR "Only weapons, armour and shields can be repaired.\n"
#define _LANG_SKILL_REPAIR_NOT_METAL "That is not metal: a forge will not mend it.\n"
#define _LANG_SKILL_REPAIR_NOTHING_TO_DO "It is in perfect condition; there is nothing to repair.\n"
#define _LANG_SKILL_REPAIR_TOO_POOR "You do not carry enough money to pay for the materials.\n"
#define _LANG_SKILL_REPAIR_ROUND1 "You heat " + target->query_short() + \
      " in the embers until the metal glows red.\n"
#define _LANG_SKILL_REPAIR_ROUND1_ROOM caster->query_cap_name() + \
      " heats " + target->query_short() + " in the forge.\n"
#define _LANG_SKILL_REPAIR_SUCCESS "You hammer the metal on the anvil and leave " + \
      target->query_short() + " in better shape. The materials cost you " + \
      handler("money")->money_string(money) + ".\n"
#define _LANG_SKILL_REPAIR_SUCCESS_ROOM caster->query_cap_name() + \
      " hammers " + target->query_short() + " on the anvil and leaves it as good as new.\n"
#define _LANG_SKILL_REPAIR_FAIL "The metal cools too soon and you mend nothing.\n"
#define _LANG_SKILL_REPAIR_FAIL_ROOM caster->query_cap_name() + \
      " strikes the anvil without much success.\n"

#define _LANG_SKILL_SLICE_NAME "slice"
#define _LANG_SKILL_SLICE_ALIASES ({ })
#define _LANG_SKILL_SLICE_HELP "With 'slice <someone>' you spin your two bladed " + \
      "weapons and loose on your opponent a series of cuts as fast as lightning. " + \
      "You need to wield two slashing weapons. The more dexterity and level you " + \
      "have, the more cuts each series carries; dexterity and how well you know " + \
      "the skill improve your aim and your damage, and bright light hinders you. " + \
      "Every series of cuts costs energy.\n"
#define _LANG_SKILL_SLICE_START "You spin your weapons, looking for a gap in your opponent's guard."
#define _LANG_SKILL_SLICE_START_ROOM "spins their weapons."
#define _LANG_SKILL_SLICE_NO_WEAPONS "You need to wield two bladed weapons to slice.\n"
#define _LANG_SKILL_SLICE_LOST_WEAPONS "You get your weapons in a tangle and stop slicing.\n"
#define _LANG_SKILL_SLICE_TIRED "You have no strength left to go on slicing.\n"
#define _LANG_SKILL_SLICE_ME "You slice " + target->query_cap_name() + \
      " with a series of quick cuts.\n"
#define _LANG_SKILL_SLICE_TARGET caster->query_cap_name() + " spins their weapons " + \
      "and slices you with a series of quick cuts.\n"
#define _LANG_SKILL_SLICE_ROOM caster->query_cap_name() + " spins their weapons and " + \
      "slices " + target->query_cap_name() + " with a series of cuts as fast as " + \
      "lightning.\n"
