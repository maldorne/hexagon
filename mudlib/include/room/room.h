#define ROOM_DEST 0
#define ROOM_MESS 1
#define ROOM_OBV  2
#define ROOM_SIZE 3
#define ROOM_FUNC 4
#define ROOM_ENTER 5

#define NO_CLEAN_UP_PROP "no_clean_up"

#define ROOM_OBJECT "/lib/room.c"
#define ITEM_OBJECT "/lib/room/items/item.c"
#define DOOR_OBJECT "/lib/room/items/door.c"

/* Only move one room a round.  Hmm.... Make it two */
#define EXIT_TIME 10

#define ROOM_HAND "/lib/room/handlers/room_handler.c"
#define EXIT_HAND "/lib/room/handlers/exit_handler.c"
#define DIG_HAND "/lib/room/handlers/dig_handler.c"

// debugging logs
//#define CLEAN_UP_LOG "clean_up_room"
//#define DOOR_LOG "room_doors.cloned"
#define FAST_CLEAN_UP 10
#define SLOW_CLEAN_UP 480
