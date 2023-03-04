
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

#define SLASHING_STR "cortante"
#define BLUNT_STR    "contundente"
#define PIERCING_STR "perforante"

#define AC_TYPES ({ SLASHING, BLUNT, PIERCING, })
#define AC_TYPES_STR ({ SLASHING_STR, BLUNT_STR, PIERCING_STR, })

#define ACID_STR       "ácido"
#define AIR_STR        "aire"
#define ELECTRICAL_STR "electricidad"
#define COLD_STR       "frío"
#define MAGIC_STR      "magia"
#define PSIONIC_STR    "psiónica"
#define MISSILES_STR   "proyectiles"
#define POISON_STR     "veneno"
#define LAW_STR        "ley"
#define WATER_STR      "agua"
#define GOOD_STR       "bien"
#define DISEASE_STR    "enfermedad"
#define FIRE_STR       "fuego"
#define EVIL_STR       "mal"
#define DEATH_STR      "muerte"
#define STONE_STR      "tierra"
#define CHAOS_STR      "caos"

#define RESISTANCES ({ ACID_STR, AIR_STR, ELECTRICAL_STR, \
    COLD_STR, MAGIC_STR, PSIONIC_STR, MISSILES_STR, \
    POISON_STR, LAW_STR, WATER_STR, GOOD_STR, DISEASE_STR, \
    FIRE_STR, EVIL_STR, DEATH_STR, STONE_STR, CHAOS_STR, })
