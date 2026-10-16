// Body parts: what can be taken from a corpse, by which skill, and what it is
// worth. A race lists them by the kind of body it has; see
// /lib/obj/death/body-parts.md.

#ifndef BODY_PARTS_H
#define BODY_PARTS_H

#define BODY_PART_OB "/lib/obj/death/body-part.c"

// Kinds of body: the race says its own (set_body_kind), or it follows from its
// race id. Each kind has its default parts in /lib/race.c.
#define BODY_HUMANOID "humanoid"
#define BODY_BEAST    "beast"
#define BODY_BIRD     "bird"
#define BODY_REPTILE  "reptile"
#define BODY_INSECT   "insect"
#define BODY_CRITTER  "critter"
#define BODY_UNDEAD   "undead"
// nothing to take: a golem, a tree
#define BODY_NONE     "none"

// Part ids: stable English keys, never translated; the names are in
// <translations/body-parts.h>, or in the part's own spec.
#define PART_SKIN      "skin"
#define PART_PLUMAGE   "plumage"
#define PART_SCALES    "scales"
#define PART_HEAD      "head"
#define PART_HAND      "hand"
#define PART_HEART     "heart"
#define PART_TEETH     "teeth"
#define PART_FANGS     "fangs"
#define PART_CLAWS     "claws"
#define PART_HORN      "horn"
#define PART_TAIL      "tail"
#define PART_MANDIBLES "mandibles"

// The spec of a part (add_body_part): a mapping with these keys.
#define BODY_PART_SKILL      "skill"      // the skill id that takes it
#define BODY_PART_MIN        "min"        // the least ability it needs
#define BODY_PART_WEIGHT     "weight"
#define BODY_PART_VALUE      "value"      // base value, before level and freshness
#define BODY_PART_COUNT      "count"      // how many a body has (1 by default)
#define BODY_PART_PERISHABLE "perishable" // 1 when it rots
#define BODY_PART_FILE       "file"       // the object it is made of, with ".c"
                                          // (BODY_PART_OB or one inheriting it)
// optional, for a part the translations do not name
#define BODY_PART_NAME       "name"
#define BODY_PART_PLURAL     "plural"
#define BODY_PART_ALIASES    "aliases"
#define BODY_PART_FEMININE   "feminine"   // 1 for a feminine noun
#define BODY_PART_PLURAL_NOUN "plural_noun" // 1 when the name is a plural ("teeth")

// Seconds of play a perishable part takes to rot, from fresh.
#define BODY_PART_FRESH_TIME 7200
// Freshness (0-100) above which a part reads fresh, and the least share of its
// value a rotten one keeps.
#define BODY_PART_FRESH_MARK 66
#define BODY_PART_ROTTEN_WORTH 10

// What a level adds to a value: a level 20 body is worth five times its base.
#define BODY_LEVEL_WORTH(value, level) ((value) * (5 + ((level) > 0 ? (level) : 0)) / 5)

// What a whole corpse is worth as an offering, per point of its race's body
// size (a human is 5); every part taken from it lowers it by a tenth, down to
// the least share.
#define CORPSE_WORTH_PER_SIZE 20
#define CORPSE_PART_TAKEN_COST 10
#define CORPSE_LEAST_WORTH 30

#endif
