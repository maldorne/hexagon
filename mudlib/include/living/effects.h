
// Effects constants

#define BASE_EFFECT "/lib/effects/base_effect.c"

#define RANGE_HANDLER  "/obj/handlers/range.c"

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

#define TARGET_TYPE_NONE "none"
#define TARGET_TYPE_ONE "one"
#define TARGET_TYPE_MANY "many"
#define TARGET_TYPE_ALL "all"
#define TARGET_TYPE_ITEM "item"
#define TARGET_TYPE_PASS "pass"
#define TARGET_TYPE_SELF "self"
#define TARGET_TYPE_TOUCH "touch"

#define PROPERTY_IS_CASTING "casting"
#define PROP_EFECTO_INAUDIBLE "inaudible"
#define PROP_EFFECT_TESTER "TEST"
#define PROPERTY_BLINDED "BLIND"

#define SP_OBJECT 1
#define SP_FUNC 2
#define SP_PARAM 3
#define S_OBJECT 0
#define S_FUNC 1
#define SP_NO_RNDS 0
#define SP_DIV 5
#define SP_GP_COST 3
