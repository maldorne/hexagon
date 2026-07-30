// Skills catalogue: skill_name -> ({ object_path, initial_ability_percent,
// times_used, ACTIVE_SKILL | PASSIVE_SKILL }). Consumed by lib/living/skills.c
// (query_skill_data / add_known_skill). Skill objects live under SKILLS_PATH.
// Player-visible skill names come from <translations/skills.h>, so the key,
// the command verb and the object's effect_name are all the same string.
//
// Per-game overrides live at /games/<game>/tables/skills_table.c (standard
// hexagon table convention: table("skills_table") resolves the game one first).

#include <living/skills.h>
#include <living/effects.h>
#include <translations/skills.h>

private mapping list;

void create()
{
  list = ([
    // passive skill: no active command; queried by other systems (movement)
    _LANG_SKILL_ORIENTATION_NAME :
      ({ SKILLS_PATH + "orientation", 30, 0, PASSIVE_SKILL, }),
  ]);
}

mapping query_skills() { return list; }
mixed query_skill_data(string str) { return list[str]; }

// Resolve a skill verb typed without accents / symbols to the canonical name.
// For now the name is returned unchanged; per-language aliases can be added.
string skill_translate(string name) { return name; }

// Inverse of skill_translate: the accent-less action verb to also register for
// a skill, or "" when there is none.
string skill_translate_to_action(string name) { return ""; }
