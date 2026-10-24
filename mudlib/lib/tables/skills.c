// Skills catalogue: skill_id -> ({ object_path, initial_value, times_used,
// ACTIVE_SKILL | PASSIVE_SKILL [, kind [, levels]] }). The optional kind is
// SKILL_KIND_PERCENTAGE (the default when absent), SKILL_KIND_LEVELLED (with
// its number of levels) or SKILL_KIND_FIXED; the initial value means whatever
// the kind says (a percentage, a level, 1 for known).
// Consumed by lib/living/skills.c
// (query_skill_data / add_known_skill). Skill objects live under SKILLS_PATH.
// The key is a stable English id (= the skill object's filename), defined in
// <living/skills.h>; the player-visible name is translated per-skill in
// /lib/obj/skills/.lang.<lang>.h and read from the object at display time.
//
// Per-game overrides live at /games/<game>/tables/skills.c (standard
// hexagon table convention: table("skills") resolves the game one first).

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

    // active skill: searches the room for hidden players
    SKILL_SEARCH :
      ({ SKILLS_PATH + SKILL_SEARCH, 10, 0, ACTIVE_SKILL, }),

    // active skill: hides the character behind a shadow (paired with search)
    SKILL_HIDE :
      ({ SKILLS_PATH + SKILL_HIDE, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'climb <something>'. Not learned by wandering: somebody
    // has to teach it, or a quest has to pay it.
    SKILL_CLIMB :
      ({ SKILLS_PATH + SKILL_CLIMB, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'repair <item>' at a smithy whose forge is lit. Learned
    // with the smith's trade, then it climbs with use.
    SKILL_REPAIR :
      ({ SKILLS_PATH + SKILL_REPAIR, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'slice <someone>', a flurry of cuts with two slashing
    // weapons. A warrior's trade: taught, not picked up.
    SKILL_SLICE :
      ({ SKILLS_PATH + SKILL_SLICE, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'swim <direction>' in water. Nobody teaches it: whoever
    // reaches water or a coast picks it up, then it climbs with use.
    SKILL_SWIM :
      ({ SKILLS_PATH + SKILL_SWIM, 10, 0, ACTIVE_SKILL, }),

    // active skills: 'follow <somebody>' and 'evade [somebody]', rolled one
    // against the other. Every player has both from the start.
    SKILL_FOLLOW :
      ({ SKILLS_PATH + SKILL_FOLLOW, 10, 0, ACTIVE_SKILL, }),
    SKILL_EVADE :
      ({ SKILLS_PATH + SKILL_EVADE, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'steal <item> from <someone>', or coins. A thief's trade:
    // taught, not picked up, then it climbs with use.
    SKILL_STEAL :
      ({ SKILLS_PATH + SKILL_STEAL, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'focus <someone>', keeps every blow on one opponent for
    // a while and draws its attention. Taught by a guild (it grants it), then it
    // climbs with use.
    SKILL_FOCUS :
      ({ SKILLS_PATH + SKILL_FOCUS, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'protect <someone>' / 'protect off', standing by someone
    // so blows aimed at them may fall on the protector (the ability sets the
    // chance, see query_protect_valid). A default skill: every player has it
    // (DEFAULT_SKILLS). It does not climb with use.
    SKILL_PROTECT :
      ({ SKILLS_PATH + SKILL_PROTECT, 50, 0, ACTIVE_SKILL, }),

    // active skill: 'judge <item>', rates a weapon or an armour carried.
    SKILL_JUDGE :
      ({ SKILLS_PATH + SKILL_JUDGE, 10, 0, ACTIVE_SKILL, }),

    // active skill: 'pray <prayer>', asking one's deity for a favour. Known or
    // not: every worshipper is given it when they take a deity.
    SKILL_PRAY :
      ({ SKILLS_PATH + SKILL_PRAY, 1, 0, ACTIVE_SKILL, SKILL_KIND_FIXED, }),

    // active skills: taking body parts from a corpse, each kind of part by its
    // own skill (the race says which). Taught by whoever teaches them, then
    // they climb with use.
    SKILL_SKINNING :
      ({ SKILLS_PATH + SKILL_SKINNING, 10, 0, ACTIVE_SKILL, }),
    SKILL_GUTTING :
      ({ SKILLS_PATH + SKILL_GUTTING, 10, 0, ACTIVE_SKILL, }),
    SKILL_DISMEMBERING :
      ({ SKILLS_PATH + SKILL_DISMEMBERING, 10, 0, ACTIVE_SKILL, }),
    SKILL_PULLING :
      ({ SKILLS_PATH + SKILL_PULLING, 10, 0, ACTIVE_SKILL, }),
  ]);
}

/**
 * Adds a skill to the catalogue; a game's own table calls it from create to
 * list the skills it ships under its own tree.
 * @param id the skill id, the same word as the skill object's filename
 * @param data ({ object_path, initial_value, times_used, ACTIVE_SKILL | PASSIVE_SKILL [, kind [, levels]] })
 */
void add_skill(string id, mixed * data)
{
  list[id] = data;
}

mapping query_skills() { return list; }
mixed query_skill_data(string str) { return list[str]; }
