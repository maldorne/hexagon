
// effects / base_effect — player-facing strings (English)

// combat role display labels, indexed by the role constants in combat.h
#define _LANG_COMBAT_ROLE_STRINGS ({ "neutral", "offensive", "defensive", "healing" })

// --- combat_fixes: protector messages ---
#define _LANG_EFFECT_PROTECT_YOU_PRE "You protect "
#define _LANG_EFFECT_PROTECT_YOU_POST " from the attack.\n"
#define _LANG_EFFECT_PROTECT_VICTIM " bravely protects you.\n"
#define _LANG_EFFECT_PROTECT_DOER " leaps in front of your victim, intercepting your spell and taking it in their body!\n"
#define _LANG_EFFECT_PROTECT_ROOM_1 " bravely protects "
#define _LANG_EFFECT_PROTECT_ROOM_2 " from "
#define _LANG_EFFECT_PROTECT_ROOM_3 "'s spell.\n"

// --- environment_fixes ---
#define _LANG_EFFECT_ENV_NO_EFFECT "Your spell has no effect!\n"
#define _LANG_EFFECT_ENV_WEAKER "Your spell seems not to work as well as it usually does.\n"
#define _LANG_EFFECT_ENV_STRONGER "Your spell seems to work better than it usually does.\n"

// --- guild_fixes ---
#define _LANG_EFFECT_PUNISH_ALIGN "You feel your God look into your heart and not find what it should.\n" + \
    "You feel yourself lose something to this humiliation.\n"

// --- base_effect: gating messages ---
#define _LANG_EFFECT_NOT_APPROVED "This effect is not for mortals, it is not approved yet.\n"
#define _LANG_EFFECT_OWN_AURA_BLOCKS "Your own protective aura prevents you from doing that.\n"
#define _LANG_EFFECT_CANT_WHILE_HIDDEN "You cannot do that while hidden.\n"
#define _LANG_EFFECT_PROTECTED_TARGET_PRE "Some kind of protection prevents you from doing that to "
#define _LANG_EFFECT_PROTECTED_TARGET_POST ".\n"
#define _LANG_EFFECT_CANT_ON_PRE "You cannot do that to "
#define _LANG_EFFECT_CANT_ON_POST ".\n"
#define _LANG_EFFECT_ROLE_BLOCKS "Your current combat role does not let you do that.\n"

// --- base_effect: start messages ---
#define _LANG_EFFECT_START_SPELL_PRE "You begin to cast the spell '"
#define _LANG_EFFECT_START_SPELL_POST "'.\n"
#define _LANG_EFFECT_START_SPELL_ROOM " begins to cast a spell.\n"
#define _LANG_EFFECT_START_SKILL_PRE " You begin to use the skill '"
#define _LANG_EFFECT_START_SKILL_POST "'.\n"

// --- base_effect: help() ---
#define _LANG_EFFECT_CATEGORY_NONE "none"
#define _LANG_EFFECT_HELP_NAME "Name: "
#define _LANG_EFFECT_HELP_LEVEL "\nLevel: "
#define _LANG_EFFECT_HELP_CATEGORY "\tCategory: "
#define _LANG_EFFECT_HELP_RANGE_NONE "\nRange: no"
#define _LANG_EFFECT_HELP_RANGE "\nRange: "
#define _LANG_EFFECT_HELP_COST "\tCost: "
#define _LANG_EFFECT_HELP_ROLE_NEEDED "\nRequired combat role: "
#define _LANG_EFFECT_HELP_DESC "\nDescription:\n"

// --- base_effect: messages shared by all effects. The ones that embed a
// name expand where caster/target are in scope at the call site. ---
#define MSG_NO_TARGET   "There does not seem to be anyone here by that name.\n"
#define MSG_DISSAPPEAR  "Your target has disappeared.\n"
#define MSG_NO_GPS      "You are too tired to do that.\n"
#define MSG_NO_COMBAT   "You cannot do that in the middle of combat.\n"
#define MSG_ONLY_COMBAT "That only makes sense in the middle of combat.\n"
#define MSG_AFECTADO    target->query_cap_name()+" is already affected by that effect.\n"
#define MSG_NO_SELF     "You cannot do that on yourself.\n"
#define MSG_WRONG_ENV   "You cannot do that in a place like this.\n"
#define MSG_NO_BLIND    "You cannot do that while blinded.\n"
#define MSG_SUIC_C      "You would not want to kill yourself!\n"
#define MSG_SUIC_R      caster->query_cap_name()+" stops what they were doing.\n"
#define MSG_NO_PUEDO    "You cannot do that right now.\n"
#define MSG_NO_PUEDO_R  caster->query_cap_name()+" stops what they were doing.\n"
#define MSG_NO_ITEMS    "You cannot do that on inanimate objects.\n"
#define MSG_ON_ITEMS    "That can only be done on inanimate objects.\n"
#define MSG_NO_CONTINUE "You cannot continue with what you were doing.\n"
#define MSG_NO_MOVE     "Moving breaks your concentration.\n"
#define MSG_NO_RANGE    "Your target is too far away.\n"
#define MSG_NO_ON_MOUNT "You cannot do that on someone who is mounted.\n"
#define MSG_NO_RIDING   "You cannot do that while riding.\n"
#define MSG_OCUPADO     "You are busy doing something else.\n"
#define MSG_ERROR       "This effect has a bug, report it to a programmer at once.\n"
// Skill category ids (from <living/skills.h>) mapped to their display names.
#define _LANG_SKILL_CATEGORIES ([ \
    "basic"   : "basic skills", \
    "armed"   : "basic armed combat", \
    "unarmed" : "basic unarmed combat", \
  ])
