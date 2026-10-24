
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
      " starts climbing " + ob->query_short() + ".\n"
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

#define _LANG_SKILL_SWIM_NAME "swim"
#define _LANG_SKILL_SWIM_ALIASES ({ "dive" })
#define _LANG_SKILL_SWIM_HELP "With 'swim' alone you get into the water from the " + \
      "shore, and with 'swim <direction>' you move through it; in the water, " + \
      "'swim' alone tells you where you see the shore. Nobody walks in water: every stroke is rolled against your skill, easier with the " + \
      "current than against it or in deep water, and harder the more you carry. " + \
      "If you fail you swallow water, and a current sweeps you away. You learn to " + \
      "swim when you reach the water.\n"
#define _LANG_SKILL_SWIM_NO_WATER "There is no water here to swim in.\n"
#define _LANG_SKILL_SWIM_NOT_THERE "You cannot swim that way.\n"
#define _LANG_SKILL_SWIM_WHICH_WAY "There is water more than one way: swim <direction>.\n"
#define _LANG_SKILL_SWIM_START "You start swimming."
#define _LANG_SKILL_SWIM_START_ROOM "starts swimming."

#define _LANG_SKILL_FOLLOW_NAME "follow"
#define _LANG_SKILL_FOLLOW_ALIASES ({ })
#define _LANG_SKILL_FOLLOW_HELP "With 'follow <somebody>' you go wherever they go, " + \
      "by the same exit and right behind, whatever happens. If they let anybody " + \
      "follow them, you just do; if not, your skill is rolled against their evade " + \
      "skill, and on a failure they shake you off. Somebody hidden cannot be " + \
      "followed: they have to be found first. You stop following with 'stop " + \
      "following'.\n"
#define _LANG_SKILL_FOLLOW_NOBODY "You see nobody like that to follow.\n"
#define _LANG_SKILL_FOLLOW_ALREADY "You are already following them.\n"
#define _LANG_SKILL_FOLLOW_CIRCLE "You cannot follow somebody who is already following you.\n"
#define _LANG_SKILL_FOLLOW_ME "You start following " + target->query_cap_name() + ".\n"
#define _LANG_SKILL_FOLLOW_TARGET caster->query_cap_name() + " starts following you.\n"
#define _LANG_SKILL_FOLLOW_ROOM caster->query_cap_name() + " starts following " + \
      target->query_cap_name() + ".\n"
#define _LANG_SKILL_FOLLOW_FAIL_ME target->query_cap_name() + " shakes you off.\n"
#define _LANG_SKILL_FOLLOW_FAIL_TARGET caster->query_cap_name() + \
      " tries to follow you, but you shake them off.\n"

#define _LANG_SKILL_EVADE_NAME "evade"
#define _LANG_SKILL_EVADE_ALIASES ({ "lose" })
#define _LANG_SKILL_EVADE_HELP "With 'evade' you try to shake off those who follow you " + \
      "and are with you, and with 'evade <somebody>' only that one. Your skill is " + \
      "rolled against their follow skill. The better you evade, the harder you are " + \
      "to follow.\n"
#define _LANG_SKILL_EVADE_NOBODY "Nobody here is following you.\n"
#define _LANG_SKILL_EVADE_ME "You shake off " + follower->query_cap_name() + ".\n"
#define _LANG_SKILL_EVADE_FOLLOWER caster->query_cap_name() + \
      " shakes you off and you lose their trail.\n"
#define _LANG_SKILL_EVADE_FAIL_ME follower->query_cap_name() + " does not lose sight of you.\n"

#define _LANG_SKILL_SWIM_ON_BRIDGE "You are on the bridge; if you want to get into the water, jump.\n"
#define _LANG_SKILL_CLIMB_WHAT "Climb what?\n"

#define _LANG_SKILL_STEAL_NAME "steal"
#define _LANG_SKILL_STEAL_ALIASES ({ })
#define _LANG_SKILL_STEAL_HELP "With 'steal <item> from <someone>' you try to take " + \
      "something from another person without them noticing; with 'steal coins " + \
      "from <someone>' (or 'money') you take a few coins, and with 'steal <item> " + \
      "in <container> from <someone>' you take something out of an open " + \
      "container they carry. Your dexterity, level and skill are rolled against " + \
      "the victim's attention: heavy things are harder, being hidden helps, and " + \
      "someone alert or fighting is harder to fool. If you are noticed you keep " + \
      "what you took but they know; if you are caught you get nothing, stop " + \
      "being hidden and monsters will attack you. You cannot steal what someone " + \
      "wears or wields, nor from low level players, nor where fighting is " + \
      "forbidden, nor while you fight.\n"
#define _LANG_SKILL_STEAL_START "You edge closer, keeping your eyes on your mark."
#define _LANG_SKILL_STEAL_ROUND1 "You reach out very carefully...\n"
#define _LANG_SKILL_STEAL_SYNTAX "Syntax: steal <item> from <someone>, steal coins from <someone> " + \
      "or steal <item> in <container> from <someone>.\n"
#define _LANG_SKILL_STEAL_VICTIM_WORD " from "
#define _LANG_SKILL_STEAL_CONTAINER_WORD " in "
#define _LANG_SKILL_STEAL_COIN_WORDS ({ "coins", "coin", "money" })
#define _LANG_SKILL_STEAL_NO_VICTIM "You see nobody like that here.\n"
#define _LANG_SKILL_STEAL_SELF "Stealing from yourself is no fun.\n"
#define _LANG_SKILL_STEAL_NOT_LIVING "That has no pockets to empty.\n"
#define _LANG_SKILL_STEAL_DEAD "The dead have nothing left to steal.\n"
#define _LANG_SKILL_STEAL_IMMORTAL "Better not pick the pockets of those you should not.\n"
#define _LANG_SKILL_STEAL_NOSTEAL "You cannot steal that.\n"
#define _LANG_SKILL_STEAL_PROTECTED "They are still too young for you to prey on their pockets.\n"
#define _LANG_SKILL_STEAL_SAFE_PLACE "Such things are not allowed here.\n"
#define _LANG_SKILL_STEAL_DARK "You cannot see well enough to reach into a pocket.\n"
#define _LANG_SKILL_STEAL_NO_ITEM "You cannot find that on them.\n"
#define _LANG_SKILL_STEAL_NO_CONTAINER "You cannot find a container like that on them.\n"
#define _LANG_SKILL_STEAL_CLOSED "That container is closed.\n"
#define _LANG_SKILL_STEAL_NO_ITEM_IN "You cannot find that inside the container.\n"
#define _LANG_SKILL_STEAL_IN_USE "They wear or wield that, you cannot steal it.\n"
#define _LANG_SKILL_STEAL_MONEY_ITEM "To take coins, say 'steal coins from <someone>'.\n"
#define _LANG_SKILL_STEAL_NO_COINS "They carry not a single coin.\n"
#define _LANG_SKILL_STEAL_GONE "It is no longer where it was.\n"
#define _LANG_SKILL_STEAL_CANNOT_CARRY "You cannot hold on to $item$.\n"
#define _LANG_SKILL_STEAL_CLEAN_ME "You lift $item$ from $hcname$ without being noticed.\n"
#define _LANG_SKILL_STEAL_NOTICED_ME "You lift $item$ from $hcname$, but they notice.\n"
#define _LANG_SKILL_STEAL_NOTICED_VICTIM "$mcname$ has just stolen $item$ from you!\n"
#define _LANG_SKILL_STEAL_NOTICED_ROOM "$mcname$ steals $item$ from $hcname$.\n"
#define _LANG_SKILL_STEAL_CAUGHT_ME "$hcname$ catches you with your hand in their pocket.\n"
#define _LANG_SKILL_STEAL_CAUGHT_VICTIM "You catch $mcname$ trying to rob you!\n"
#define _LANG_SKILL_STEAL_CAUGHT_ROOM "$mcname$ tries to rob $hcname$, but is caught.\n"

#define _LANG_SKILL_FOCUS_NAME "focus"
#define _LANG_SKILL_FOCUS_ALIASES ({ })
#define _LANG_SKILL_FOCUS_OFF_WORDS ({ "off", "stop" })
#define _LANG_SKILL_FOCUS_HELP "With 'focus <someone>' you put all your blows on a " + \
      "single one of your opponents, even if the fight would take you to another. " + \
      "If somebody protects them, you will have to strike the protector first. " + \
      "With 'focus off' you stop. Your opponent grows more intent on you: twice " + \
      "as much if your combat stance is defensive, and not at all if it is " + \
      "supportive. It costs a point of energy for each round, and you lose your " + \
      "concentration after two turns for each of your levels.\n"
#define _LANG_SKILL_FOCUS_SYNTAX "Syntax: focus <target>, focus off\n"
#define _LANG_SKILL_FOCUS_NOT_FOCUSED "You are not focusing your blows on anybody.\n"
#define _LANG_SKILL_FOCUS_NOT_FIGHTING "You are not fighting that.\n"
#define _LANG_SKILL_FOCUS_OFF "You stop focusing your blows.\n"
#define _LANG_SKILL_FOCUS_START_ME "You start to focus your blows on " + \
      ob->query_cap_name() + ".\n"
#define _LANG_SKILL_FOCUS_START_ROOM caster->query_cap_name() + " focuses their " + \
      "blows on " + ob->query_cap_name() + ".\n"
#define _LANG_SKILL_FOCUS_TARGET_GONE "Your target is no longer in the fight, and " + \
      "you stop focusing your blows.\n"
#define _LANG_SKILL_FOCUS_LOST "You lose your mental concentration.\n"
#define _LANG_SKILL_FOCUS_TIRED "You are too tired to keep focusing your blows.\n"

#define _LANG_SKILL_PROTECT_NAME "protect"
#define _LANG_SKILL_PROTECT_ALIASES ({ })
#define _LANG_SKILL_PROTECT_OFF_WORDS ({ "off", "stop" })
#define _LANG_SKILL_PROTECT_HELP "With 'protect <someone>' you stand by someone who is " + \
      "with you, and the blows aimed at them may fall on you. They have to accept " + \
      "your protection. With 'protect off <someone>' you stop protecting them, and " + \
      "with 'protect off' you stop protecting everyone with you.\n"
#define _LANG_SKILL_PROTECT_SYNTAX "Syntax: protect <target>, protect off [<target>]\n"
#define _LANG_SKILL_PROTECT_WHO "Protect whom?\n"
#define _LANG_SKILL_PROTECT_SELF "You cannot protect yourself.\n"
#define _LANG_SKILL_PROTECT_REFUSED ob->query_cap_name() + " does not want your protection.\n"
#define _LANG_SKILL_PROTECT_ME "You protect " + ob->query_short() + ".\n"
#define _LANG_SKILL_PROTECT_HOW_TO_STOP "Use 'protect off' to stop doing it.\n"
#define _LANG_SKILL_PROTECT_ROOM caster->query_cap_name() + " protects " + ob->query_short() + ".\n"
#define _LANG_SKILL_PROTECT_THEM caster->query_cap_name() + " protects you.\n"
#define _LANG_SKILL_PROTECT_NOT_PROTECTING "You are not protecting anyone by that name.\n"
#define _LANG_SKILL_PROTECT_NOBODY "You are not protecting anyone here.\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_THEM_HERE caster->query_cap_name() + \
      " steps back and protects you no more.\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_ROOM caster->query_cap_name() + " stops protecting " + \
      ob->query_cap_name() + ".\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_THEM caster->query_cap_name() + " stops protecting you.\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_ME "You step back and stop protecting " + ob->query_cap_name() + ".\n"

#define _LANG_SKILL_JUDGE_NAME "judge"
#define _LANG_SKILL_JUDGE_ALIASES ({ })
#define _LANG_SKILL_JUDGE_HELP "With 'judge <item>' you tell yourself how much harm " + \
      "a weapon you carry does, or how well an armour or shield protects. It " + \
      "costs no energy. Your assessment is not always exact, and it improves " + \
      "with practice.\n"
#define _LANG_SKILL_JUDGE_NOT_CARRIED "You can only judge what you carry.\n"
#define _LANG_SKILL_JUDGE_WORDS ({ "very bad", "bad", "poor", "below average", \
      "average", "above average", "good", "great", "wonderful", \
      "incredible", "invincible", "unsurpassed" })
#define _LANG_SKILL_JUDGE_WEAPON "The damage of " + item->query_short() + " is " + \
      word + " (" + type_name + " damage).\n"
#define _LANG_SKILL_JUDGE_ARMOUR "The protection of " + item->query_short() + " is " + \
      word + ".\n"
#define _LANG_SKILL_JUDGE_USELESS capitalize(target->query_short()) + " looks " + \
      "rather useless to you.\n"

// pray.c

#define _LANG_SKILL_PRAY_NAME "pray"
#define _LANG_SKILL_PRAY_ALIASES ({ })
#define _LANG_SKILL_PRAY_HELP "With 'pray <prayer>' you ask your god for a favour, " + \
      "and only where they are worshipped: in one of their temples, or before an " + \
      "altar or a statue dedicated to them. Which prayers a god hears depends on " + \
      "the god; the most common are healing (you recover hit points), power (you " + \
      "recover energy), blessing (more constitution for a while), strength (more " + \
      "strength for a while) and wisdom (more wisdom and intelligence for a " + \
      "while). The gods do not always listen, and less the more they are " + \
      "bothered: if you pray too much they will get angry and end up cursing " + \
      "you. Whoever worships no god does not know how to pray.\n"
#define _LANG_SKILL_PRAY_START "You raise a prayer."
#define _LANG_SKILL_PRAY_START_ROOM "prays in silence."
#define _LANG_SKILL_PRAY_NO_DEITY "You worship no god: there is nobody to pray to.\n"
#define _LANG_SKILL_PRAY_WRONG_PLACE "You do not dare to pray here: your $noun$ could take offence.\n"
#define _LANG_SKILL_PRAY_SYNTAX "Pray for what? The prayers $god$ hears are: " + \
      "$prayers$.\nSyntax: pray <prayer>\n"
#define _LANG_SKILL_PRAY_CURSED "You are cursed and cannot pray.\n"
#define _LANG_SKILL_PRAY_REFUSED "$god$ shuns hearing a prayer like that.\n"

// base-extraction.c, shared by skinning.c, gutting.c, dismembering.c, pulling.c

#define _LANG_SKILL_EXTRACT_SEPARATORS ({ " from ", " of " })
#define _LANG_SKILL_EXTRACT_START "You bend over the body, blade in hand."
#define _LANG_SKILL_EXTRACT_START_ROOM "bends over a body, blade in hand."
#define _LANG_SKILL_EXTRACT_SYNTAX "Syntax: " + query_effect_name() + " <corpse> | " + \
      query_effect_name() + " <part> from <corpse>\n"
#define _LANG_SKILL_EXTRACT_NO_BLADE "You need to wield a knife or a dagger to do that.\n"
#define _LANG_SKILL_EXTRACT_NO_CORPSE "You see no body like that here.\n"
#define _LANG_SKILL_EXTRACT_UNKNOWN_RACE "You cannot tell what this being was, so you " + \
      "would not know where to start.\n"
#define _LANG_SKILL_EXTRACT_NOTHING "There is nothing on this body you know how to take that way.\n"
#define _LANG_SKILL_EXTRACT_NO_SUCH_PART "This body has nothing like that.\n"
#define _LANG_SKILL_EXTRACT_WRONG_SKILL "That is not how that is taken.\n"
#define _LANG_SKILL_EXTRACT_TAKEN "Somebody has already taken that from this body.\n"
#define _LANG_SKILL_EXTRACT_NOT_SKILLED "You do not master this art well enough to try it " + \
      "on this body yet.\n"
#define _LANG_SKILL_EXTRACT_BROKEN "Something goes wrong and you get nothing.\n"
#define _LANG_SKILL_EXTRACT_DONE "You work on the " + corpse_name + " with your blade and " + \
      "take " + part->query_article() + " " + part_name + ".\n"
#define _LANG_SKILL_EXTRACT_DONE_ROOM caster->query_cap_name() + " works on the " + corpse_name + \
      " with a blade and takes " + part->query_article() + " " + part_name + ".\n"
#define _LANG_SKILL_EXTRACT_RUINED "Your hand slips and you ruin what you were taking from " + \
      "the " + corpse_name + ".\n"
#define _LANG_SKILL_EXTRACT_RUINED_ROOM caster->query_cap_name() + " works on the " + corpse_name + \
      " with a blade, but ruins it.\n"

// skinning.c

#define _LANG_SKILL_SKINNING_NAME "skin"
#define _LANG_SKILL_SKINNING_ALIASES ({ })
#define _LANG_SKILL_SKINNING_HELP "With 'skin <corpse>', wielding a knife or a dagger, you " + \
      "take the skin, plumage or scales off a body to sell or work them later. Skinning " + \
      "an animal is the easiest; the feathers of a bird need more skill, and the scales " + \
      "of a reptile more still. It improves with practice.\n"

// gutting.c

#define _LANG_SKILL_GUTTING_NAME "gut"
#define _LANG_SKILL_GUTTING_ALIASES ({ })
#define _LANG_SKILL_GUTTING_HELP "With 'gut <corpse>', wielding a knife or a dagger, you open " + \
      "a body and take out its heart. A heart rots with time and loses its value. It " + \
      "improves with practice.\n"

// dismembering.c

#define _LANG_SKILL_DISMEMBERING_NAME "dismember"
#define _LANG_SKILL_DISMEMBERING_ALIASES ({ })
#define _LANG_SKILL_DISMEMBERING_HELP "With 'dismember <corpse>' or 'dismember <part> from " + \
      "<corpse>', wielding a knife or a dagger, you cut the head, the hands or the tail off " + \
      "a body. What is cut rots with time and loses its value. It improves with practice.\n"

// pulling.c

#define _LANG_SKILL_PULLING_NAME "pull"
#define _LANG_SKILL_PULLING_ALIASES ({ })
#define _LANG_SKILL_PULLING_HELP "With 'pull <corpse>' or 'pull <part> from <corpse>', " + \
      "wielding a knife or a dagger, you take the teeth, fangs, claws or horns out of a " + \
      "body. They do not rot. It improves with practice.\n"
