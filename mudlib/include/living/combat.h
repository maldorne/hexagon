
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

// The hit message ends with the wound the hit made. It always does when the
// hit took this percent or more of the victim's hit points (or was a critical,
// or killed); a smaller hit tells its wound one time in HIT_MESSAGE_WOUND_SMALL_ODDS.
#define HIT_MESSAGE_WOUND_ALWAYS_PERCENT 25
#define HIT_MESSAGE_WOUND_SMALL_ODDS     3

// combat types
#define MORTAL_COMBAT 0
#define SAFE_COMBAT 1

// combat role
#define NEUTRAL_ROLE 0
#define DPS_ROLE 1
#define TANK_ROLE 2
#define HEALER_ROLE 3

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

// The attacks a creature makes with its own body, which is what add_attacks()
// takes. Ids, so english: the words a player reads live in the table's .lang
// files
#define NATURAL_ATTACK_FANGS  "fangs"
#define NATURAL_ATTACK_CLAWS  "claws"
#define NATURAL_ATTACK_BEAK   "beak"
#define NATURAL_ATTACK_HORNS  "horns"
#define NATURAL_ATTACK_HOOVES "hooves"
#define NATURAL_ATTACK_TEETH  "teeth"
#define NATURAL_ATTACK_PAWS   "paws"
#define NATURAL_ATTACK_TAIL   "tail"
#define NATURAL_ATTACK_LEGS   "legs"

// minimum ability with a style
#define MIN_UNARMED_ABILITY 10
// maximum ability value learning by yourself (auto)
#define MAX_UNARMED_AUTO_LEARN 75

// default damage dice 1d6
#define BDEFNUM 1
#define BDEFTYPE 6

