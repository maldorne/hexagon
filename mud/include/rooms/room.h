#define ROOM_DEST 0
#define ROOM_MESS 1
#define ROOM_OBV  2
#define ROOM_SIZE 3
#define ROOM_FUNC 4
#define ROOM_ENTER 5

#define NO_CLEAN_UP_PROP "no_clean_up"

#define ITEM_OBJECT "/std/room/item"
#define DOOR_OBJECT "/std/room/door"

/* Only move one room a round.  Hmm.... Make it two */
#define EXIT_TIME 10

#define ROOM_HAND "/obj/handlers/room_handler"
#define EXIT_HAND "/obj/handlers/exit_handler"
#define DIG_HAND "/obj/handlers/dig_handler"

#define SHORTEN ([\
"norte":"n", "sur":"s", "oeste":"o", "este":"e", "noreste":"ne",\
"noroeste":"no", "sudeste":"se", "sudoeste":"so", "arriba":"ar",\
"abajo":"ab", "dentro":"de", "fuera":"fu", "escaleras":"es",\
])

#define NEAREST ([\
"norte":({"noreste","noroeste",}), "sur":({"sudoeste","sudeste",}),\
"este":({"noreste","sudeste",}), "oeste":({"noroeste","sudoeste",}),\
"noreste":({"norte","este",}), "noroeste":({"oeste","norte",}),\
"sudeste":({"sur","este",}), "sudoeste":({"oeste","sur",}),\
])

#define BROAD_NEAREST ([\
"norte":({"noreste","noroeste","oeste","este",}),\
"sur":({"sudoeste","sudeste","este","oeste",}),\
"este":({"noreste","sudeste","sur","norte",}),\
"oeste":({"noroeste","sudoeste","sur","norte",}),\
"noreste":({"norte","este","sudeste","noroeste",}),\
"noroeste":({"oeste","norte","noreste","sudoeste",}),\
"sudeste":({"sur","este","noreste","sudoeste",}),\
"sudoeste":({"oeste","sur","sudeste","noroeste",}),\
])
