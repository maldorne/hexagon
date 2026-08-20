
#define SKILLS_PATH "/lib/obj/skills/"

#define ACTIVE_SKILL 0
#define PASSIVE_SKILL 1

// Data returned by query_skill_data from the skills table
#define SKILL_DATA_PATH 0

// Skill ids: stable English keys, the same word as the skill object's
// filename. Used as the skills table key, stored in known_skills, and
// referenced by callers (e.g. the maze component). Never translated —
// the display name lives per-skill in /lib/obj/skills/.lang.<lang>.h.
#define SKILL_ORIENTATION "orientation"
#define SKILL_SEARCH      "search"
#define SKILL_HIDE        "hide"

// Skills every player must always have. Granted silently and idempotently
// on each login (grant_default_skills, called from living::start_player):
// a player missing one gets it on their next connection, first login
// included. Add an id here and every player picks it up as they reconnect.
#define DEFAULT_SKILLS ({ SKILL_SEARCH })

// Skill category ids: stable English keys, translated only for display
// (see the category display map in lib/living/.lang.<lang>.h).
#define SKILL_TYPE_BASIC       "basic"
#define SKILL_TYPE_ARMED       "armed"
#define SKILL_TYPE_UNARMED     "unarmed"
#define SKILL_TYPE_EXPLORATION "exploration"
