
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

// combat types
#define MORTAL_COMBAT 0
#define SAFE_COMBAT 1
#define COMBAT_MODE_STRINGS ({ "letal", "no letal" })

// combat role
#define NEUTRAL_ROLE 0
#define DPS_ROLE 1
#define TANK_ROLE 2
#define HEALER_ROLE 3
#define COMBAT_ROLE_STRINGS ({ "cualquiera", "ofensiva", "defensiva", "de apoyo" })

#define COMBAT_ROLE_CHANGED_PROP "combat_role_changed"

// ****************************************************
// armed combat

// minimum ability with a style
// #define MIN_ARMED_ABILITY 10

// maximum ability value learning by yourself (without a teacher)
#define MAX_ARMED_AUTO_LEARN 75

// ****************************************************
// unarmed combat

// just an id, there is no need to translate it
#define DEF_UNARMED_STYLE_NAME "fighting"

// minimum ability with a style
#define MIN_UNARMED_ABILITY 10
// maximum ability value learning by yourself (auto)
#define MAX_UNARMED_AUTO_LEARN 75

// default damage dice 1d6
#define BDEFNUM 1
#define BDEFTYPE 6

