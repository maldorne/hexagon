
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

#define SLASHING_STR "slashing"
#define BLUNT_STR    "blunt"
#define PIERCING_STR "piercing"

#define AC_TYPES ({ SLASHING, BLUNT, PIERCING, })
#define AC_TYPES_STR ({ SLASHING_STR, BLUNT_STR, PIERCING_STR, })

#define ACID_STR       "acid"
#define AIR_STR        "air"
#define ELECTRICAL_STR "electrical"
#define COLD_STR       "cold"
#define MAGIC_STR      "magic"
#define PSIONIC_STR    "psionic"
#define MISSILES_STR   "missiles"
#define POISON_STR     "poison"
#define LAW_STR        "law"
#define WATER_STR      "water"
#define GOOD_STR       "good"
#define DISEASE_STR    "disease"
#define FIRE_STR       "fire"
#define EVIL_STR       "evil"
#define DEATH_STR      "death"
#define STONE_STR      "stone"
#define CHAOS_STR      "chaos"

#define RESISTANCES ({ ACID_STR, AIR_STR, ELECTRICAL_STR, \
    COLD_STR, MAGIC_STR, PSIONIC_STR, MISSILES_STR, \
    POISON_STR, LAW_STR, WATER_STR, GOOD_STR, DISEASE_STR, \
    FIRE_STR, EVIL_STR, DEATH_STR, STONE_STR, CHAOS_STR, })

#define COMBAT_MODE_STRINGS ({ "lethal", "non-lethal" })
#define COMBAT_ROLE_STRINGS ({ "any", "offensive", "deffensive", "support" })
