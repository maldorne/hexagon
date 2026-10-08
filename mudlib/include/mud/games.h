// Why a game cannot be played by an account right now, as game.c's
// query_closed_reason() answers it. An empty string means it can.

// closed to players altogether
#define GAME_CLOSED     "closed"
// not a demo, and the account has not finished one yet
#define GAME_NEEDS_DEMO "needs-demo"
// a demo another character of the account is already playing
#define GAME_DEMO_TAKEN "demo-taken"

// where every game starts a new character, and where a character moving in
// from another game arrives
#define GAME_START_ROOM "areas/start/begin.c"

// carried by a character moving in from another game while the start rooms
// send it on: it goes through them unseen, and only a room where it has to
// stop and choose something is shown
#define PASSING_THROUGH_PROP "passing through"
