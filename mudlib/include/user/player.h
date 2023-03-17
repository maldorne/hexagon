
#define PLAYER_OB "/lib/player.c"
#define PLAYERS_SAVE_DIR "/save/players/"

// #define LOADING "loading" // Ahora en properties.h
// #define AUTOEQUIP_PROP "autoequip"
#define GUEST_PROP "guest"

// On login:

// starting position for new players
// here they will choose the game for the newly created character
#define START_POS "/games/hexagon/areas/start/begin.c"

// Taniwha 14/09/1995 how long shall we disable guild commands
// and spells after login
#define LOGINLOCK 20


// gender
#define G_CHAR (this_object()->query_gender()==2?"a":"o")

// items
// objects a player must have
// TODO this will change with multiple games
#define MUST_HAVE ({ "/lib/obj/diary", "/lib/obj/hearthstone", })

