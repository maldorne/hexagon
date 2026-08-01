// Skills catalogue: skill_id -> ({ object_path, initial_ability_percent,
// times_used, ACTIVE_SKILL | PASSIVE_SKILL }). Consumed by lib/living/skills.c
// (query_skill_data / add_known_skill). Skill objects live under SKILLS_PATH.
// The key is a stable English id (= the skill object's filename), defined in
// <living/skills.h>; the player-visible name is translated per-skill in
// /lib/obj/skills/.lang.<lang>.h and read from the object at display time.
//
// Per-game overrides live at /games/<game>/tables/skills_table.c (standard
// hexagon table convention: table("skills_table") resolves the game one first).

#include <living/skills.h>
#include <living/effects.h>

private mapping list;

void create()
{
  list = ([
    // passive skill: no active command; queried by other systems (movement).
    // Fields: object path, initial ability %, times used, active/passive.
    // Orientation is learned cheaply (10%) — a newbie first picks it up
    // by wandering a maze, then it climbs with use.
    SKILL_ORIENTATION :
      ({ SKILLS_PATH + SKILL_ORIENTATION, 10, 0, PASSIVE_SKILL, }),

    // active skill: reveals hidden objects in the room, scaling with ability
    SKILL_SEARCH :
      ({ SKILLS_PATH + SKILL_SEARCH, 10, 0, ACTIVE_SKILL, }),
  ]);
}

mapping query_skills() { return list; }
mixed query_skill_data(string str) { return list[str]; }
