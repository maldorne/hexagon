
#define SKILLS_PATH "/lib/obj/skills/"

#define ACTIVE_SKILL 0
#define PASSIVE_SKILL 1

// Data returned by query_skill_data from the skills table:
//   ({ path, initial value, times used, ACTIVE_SKILL | PASSIVE_SKILL,
//      [kind, [number of levels]] })
// The last two entries are optional; a row without them is a percentage skill.
#define SKILL_DATA_PATH   0
#define SKILL_DATA_VALUE  1
#define SKILL_DATA_USES   2
#define SKILL_DATA_TYPE   3
#define SKILL_DATA_KIND   4
#define SKILL_DATA_LEVELS 5

// What the stored value of a skill means. There is no conversion between
// kinds: a skill's own code asks for the kind it was written for.
//   percentage: ability 1-100, improves with use (the default)
//   levelled:   discrete level 1..N, N given by SKILL_DATA_LEVELS
//   fixed:      known or not (stored as 1), never improves
#define SKILL_KIND_PERCENTAGE 0
#define SKILL_KIND_LEVELLED   1
#define SKILL_KIND_FIXED      2

// Skill ids: stable English keys, the same word as the skill object's
// filename. Used as the skills table key, stored in known_skills, and
// referenced by callers (e.g. the maze component). Never translated —
// the display name lives per-skill in /lib/obj/skills/.lang.<lang>.h.
#define SKILL_ORIENTATION "orientation"
#define SKILL_SEARCH      "search"
#define SKILL_HIDE        "hide"
#define SKILL_CLIMB       "climb"
#define SKILL_REPAIR      "repair"
#define SKILL_SLICE       "slice"
#define SKILL_SWIM        "swim"
#define SKILL_FOLLOW      "follow"
#define SKILL_EVADE       "evade"
#define SKILL_STEAL       "steal"
#define SKILL_CHARGE      "charge"
#define SKILL_FOCUS       "focus"
#define SKILL_JUDGE       "judge"

// Skills every player must always have. Granted silently and idempotently
// on each login (grant_default_skills, called from living::start_player):
// a player missing one gets it on their next connection, first login
// included. Add an id here and every player picks it up as they reconnect.
#define DEFAULT_SKILLS ({ SKILL_SEARCH, SKILL_FOLLOW, SKILL_EVADE })

// Skill category ids: stable English keys, translated only for display
// (see the category display map in lib/living/.lang.<lang>.h).
#define SKILL_TYPE_BASIC       "basic"
#define SKILL_TYPE_ARMED       "armed"
#define SKILL_TYPE_UNARMED     "unarmed"
#define SKILL_TYPE_EXPLORATION "exploration"
