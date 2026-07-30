
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
