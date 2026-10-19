
// Effects constants

#include <living/range.h>

#define BASE_EFFECT "/lib/effects/base_effect.c"

#define EFFECT_IS_SKILL 0
#define EFFECT_IS_SPELL 1

#define EFFECT_TYPE_SPELL "spell"

// Category knowledge levels a caster can have in an effect's category,
// returned by query_effect_category_level (0: none, 1: minor, 2: neutral,
// 3: major). Used to surcharge/discount a spell's gp cost.
#define EFFECT_CAT_NONE    "none"
#define EFFECT_CAT_MINOR   "minor"
#define EFFECT_CAT_NEUTRAL "neutral"
#define EFFECT_CAT_MAJOR   "major"

// Kinds of spell categories, see /lib/tables/spells.c
#define SPELL_CATEGORY_SPHERE "sphere"
#define SPELL_CATEGORY_SCHOOL "school"

#define TARGET_TYPE_NONE "none"
#define TARGET_TYPE_ONE "one"
#define TARGET_TYPE_MANY "many"
#define TARGET_TYPE_ALL "all"
#define TARGET_TYPE_ITEM "item"
#define TARGET_TYPE_PASS "pass"
#define TARGET_TYPE_SELF "self"
#define TARGET_TYPE_TOUCH "touch"

// How far an effect reaches, see query_effect_reach in base_effect.c. Ids are
// never shown to players, the help and the spell list translate them.
#define EFFECT_REACH_SELF   "self"     // the caster only
#define EFFECT_REACH_TOUCH  "touch"    // one target, by touch
#define EFFECT_REACH_SINGLE "single"   // one target in the same place
#define EFFECT_REACH_RANGED "ranged"   // one target that may be in another place at the start
#define EFFECT_REACH_AREA   "area"     // everyone in the place
#define EFFECT_REACH_WIDE   "wide"     // everyone in several places

// Whom an effect is meant for, see query_effect_intent in base_effect.c: it
// helps its target (a cure, a blessing, a ward), harms it (damage, a curse, a
// hold), or has no target to judge (hide, climb, a light). An NPC picks its
// targets by it, allies for a help and enemies for a harm.
#define EFFECT_INTENT_NONE "none"
#define EFFECT_INTENT_HELP "help"
#define EFFECT_INTENT_HARM "harm"

#define PROPERTY_IS_CASTING "casting"
#define PROP_EFECTO_INAUDIBLE "inaudible"
#define PROP_EFFECT_TESTER "TEST"
#define PROPERTY_BLINDED "BLIND"
// the level an effect cast at a given level (a scroll) runs at, held on the caster
#define PROPERTY_CAST_LEVEL "cast level"
// the power (1 minor, 2 neutral, 3 major) it runs with, held the same way
#define PROPERTY_CAST_POWER "cast power"

#define SP_OBJECT 1
#define SP_FUNC 2
#define SP_PARAM 3
#define S_OBJECT 0
#define S_FUNC 1
#define SP_NO_RNDS 0
#define SP_DIV 5
#define SP_GP_COST 3

// Lasting effects (lib/living/effects.c): what each entry holds, the hook
// called on the source when one ends, and why it ended
#define LASTING_SOURCE 0
#define LASTING_FUNC 1
#define LASTING_TICKS 2
#define LASTING_DATA 3

#define LASTING_TICK_FUNC "lasting_tick"
#define LASTING_END_FUNC "lasting_end"

#define LASTING_END_EXPIRED "expired"
#define LASTING_END_DONE "done"
#define LASTING_END_ERROR "error"
#define LASTING_END_REMOVED "removed"
#define LASTING_END_REPLACED "replaced"
#define LASTING_END_CLEARED "cleared"
#define LASTING_END_DEATH "death"
