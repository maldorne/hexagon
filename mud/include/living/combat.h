
#define FUMBLE "fumble"
#define HIT "hit"
#define MISS "miss"
#define DODGE "dodge"

#define ATT "%^BOLD%^%^GREEN%^# %^RESET%^"
#define DFF "%^BOLD%^%^RED%^* %^RESET%^"

#define SPE_ATT "%^BOLD%^%^GREEN%^# # %^RESET%^"
#define SPE_DFF "%^BOLD%^%^RED%^* * %^RESET%^"
#define SPE_FAIL "%^BOLD%^YELLOW%^~ ~ %^RESET%^"
#define SPE_INFO "%^BOLD%^YELLOW%^~ %^RESET%^"

// Tipos de combate
#define MORTAL_COMBAT 0
#define SAFE_COMBAT 1
#define COMBAT_MODE_STRINGS ({ "letal", "no letal" })

// Actitud de combate (combat role)
#define NEUTRAL_ROLE 0
#define DPS_ROLE 1
#define TANK_ROLE 2
#define HEALER_ROLE 3
#define COMBAT_ROLE_STRINGS ({ "cualquiera", "ofensiva", "defensiva", "de apoyo" })

#define COMBAT_ROLE_CHANGED_PROP "combat_role_changed"

// ****************************************************
// old attacks.h

#define SLASHING 1
#define BLUNT 2
#define PIERCING 3

#define SLASHING_STR "cortante"
#define BLUNT_STR    "contundente"
#define PIERCING_STR "perforante"

// ****************************************************
// armed combat

#define WEAPON_TABLE "/table/weapon_table.c"

// Porcentaje minimo de control de un estilo
// #define MIN_ARMED_ABILITY 10

// Porcentaje maximo que se puede alcanzar sin maestros
#define MAX_ARMED_AUTO_LEARN 75

// ****************************************************
// unarmed combat

#define UNARMED_BASE "/table/unarmed_combat"

// Tipo de estilo de combate por defecto
#define DEF_UNARMED "pelea"
// Porcentaje minimo de control de un estilo
#define MIN_UNARMED_ABILITY 10

// Porcentaje maximo que se puede alcanzar sin maestros
#define MAX_UNARMED_AUTO_LEARN 75

#define BDEFNUM 1
#define BDEFTYPE 6

