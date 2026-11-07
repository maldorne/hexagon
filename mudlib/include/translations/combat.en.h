// Damage types: the attack types and the ids are in <living/damage-types.h>,
// here is what a player reads. Ids, never these names, are what the code
// compares and stores.
#include <living/damage-types.h>

// the name of each damage type, by id
#define _LANG_DAMAGE_TYPE_NAMES ([ \
    DAMAGE_SLASHING : "slashing", DAMAGE_BLUNT : "blunt", \
    DAMAGE_PIERCING : "piercing", DAMAGE_ACID : "acid", \
    DAMAGE_AIR : "air", DAMAGE_ELECTRICAL : "electrical", \
    DAMAGE_COLD : "cold", DAMAGE_MAGIC : "magic", \
    DAMAGE_PSIONIC : "psionic", DAMAGE_MISSILES : "missiles", \
    DAMAGE_POISON : "poison", DAMAGE_LAW : "law", DAMAGE_WATER : "water", \
    DAMAGE_GOOD : "good", DAMAGE_DISEASE : "disease", \
    DAMAGE_FIRE : "fire", DAMAGE_EVIL : "evil", DAMAGE_DEATH : "death", \
    DAMAGE_STONE : "stone", DAMAGE_CHAOS : "chaos" ])
// other words a player may use for them
#define _LANG_DAMAGE_TYPE_ALIASES ([ ])

#define COMBAT_MODE_STRINGS ({ "lethal", "non-lethal" })
#define COMBAT_ROLE_STRINGS ({ "any", "aggressive", "defensive", "supportive" })
