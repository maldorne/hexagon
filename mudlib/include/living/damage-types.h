// Damage types. Each one is an int, for the combat rolls, and an English id
// string, for everything that is stored or looked up by name: the resistances
// of a living, the resistance properties of an item, the type of the damage a
// spell or a skill deals. The ids are never translated; the name a player
// reads is in <translations/combat.h> and comes from table("damage-types").

#ifndef DAMAGE_TYPES_H
#define DAMAGE_TYPES_H

// attack types: the physical ones first, then the elemental ones, in the order
// of DAMAGE_ELEMENTAL_TYPES (an elemental type minus sizeof(AC_TYPES) is its
// index there)
#define SLASHING 0
#define BLUNT 1
#define PIERCING 2

#define ACID 3
#define AIR 4
#define ELECTRICAL 5
#define COLD 6
#define MAGIC 7
#define PSIONIC 8
#define MISSILES 9
#define POISON 10
#define LAW 11
#define WATER 12
#define GOOD 13
#define DISEASE 14
#define FIRE 15
#define EVIL 16
#define DEATH 17
#define STONE 18
#define CHAOS 19

#define AC_TYPES ({ SLASHING, BLUNT, PIERCING, })

// ids of the physical damage types
#define DAMAGE_SLASHING "slashing"
#define DAMAGE_BLUNT    "blunt"
#define DAMAGE_PIERCING "piercing"

// ids of the elemental damage types
#define DAMAGE_ACID       "acid"
#define DAMAGE_AIR        "air"
#define DAMAGE_ELECTRICAL "electrical"
#define DAMAGE_COLD       "cold"
#define DAMAGE_MAGIC      "magic"
#define DAMAGE_PSIONIC    "psionic"
#define DAMAGE_MISSILES   "missiles"
#define DAMAGE_POISON     "poison"
#define DAMAGE_LAW        "law"
#define DAMAGE_WATER      "water"
#define DAMAGE_GOOD       "good"
#define DAMAGE_DISEASE    "disease"
#define DAMAGE_FIRE       "fire"
#define DAMAGE_EVIL       "evil"
#define DAMAGE_DEATH      "death"
#define DAMAGE_STONE      "stone"
#define DAMAGE_CHAOS      "chaos"

// the ids by attack type: AC_TYPES, and the types past them
#define DAMAGE_PHYSICAL_TYPES ({ DAMAGE_SLASHING, DAMAGE_BLUNT, \
    DAMAGE_PIERCING, })

#define DAMAGE_ELEMENTAL_TYPES ({ DAMAGE_ACID, DAMAGE_AIR, \
    DAMAGE_ELECTRICAL, DAMAGE_COLD, DAMAGE_MAGIC, DAMAGE_PSIONIC, \
    DAMAGE_MISSILES, DAMAGE_POISON, DAMAGE_LAW, DAMAGE_WATER, DAMAGE_GOOD, \
    DAMAGE_DISEASE, DAMAGE_FIRE, DAMAGE_EVIL, DAMAGE_DEATH, DAMAGE_STONE, \
    DAMAGE_CHAOS, })

#endif
