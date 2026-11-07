// Damage types: the attack types and the ids are in <living/damage-types.h>,
// here is what a player reads. Ids, never these names, are what the code
// compares and stores.
#include <living/damage-types.h>

// the name of each damage type, by id
#define _LANG_DAMAGE_TYPE_NAMES ([ \
    DAMAGE_SLASHING : "cortante", DAMAGE_BLUNT : "contundente", \
    DAMAGE_PIERCING : "perforante", DAMAGE_ACID : "ácido", \
    DAMAGE_AIR : "aire", DAMAGE_ELECTRICAL : "electricidad", \
    DAMAGE_COLD : "frío", DAMAGE_MAGIC : "magia", \
    DAMAGE_PSIONIC : "psiónica", DAMAGE_MISSILES : "proyectiles", \
    DAMAGE_POISON : "veneno", DAMAGE_LAW : "ley", DAMAGE_WATER : "agua", \
    DAMAGE_GOOD : "bien", DAMAGE_DISEASE : "enfermedad", \
    DAMAGE_FIRE : "fuego", DAMAGE_EVIL : "mal", DAMAGE_DEATH : "muerte", \
    DAMAGE_STONE : "tierra", DAMAGE_CHAOS : "caos" ])
// other words a player may use for them, plain-ASCII variants included
#define _LANG_DAMAGE_TYPE_ALIASES ([ \
    DAMAGE_ACID : ({ "acido" }), DAMAGE_COLD : ({ "frio" }), \
    DAMAGE_PSIONIC : ({ "psionica" }) ])

#define COMBAT_MODE_STRINGS ({ "letal", "no letal" })
#define COMBAT_ROLE_STRINGS ({ "libre", "agresiva", "defensiva", "apoyo" })
