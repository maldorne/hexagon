
#define GAME_COORDINATOR "admin"
#define GAME_NAME "Hexagon"

#include <language.h>

inherit "/lib/core/game.c";

void create()
{
   ::create();
   set_game_coordinator(GAME_COORDINATOR);
   set_game_name(GAME_NAME);
   set_game_short_description(_LANG_GAME_SHORT_DESC);   
   set_open_read(0);
   set_open_write(0);
   // set_finger_info("");

   // not open to players
   set_open(false);

   save_me();
}
