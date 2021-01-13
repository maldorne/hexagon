
#define GAME_COORDINATOR "neverbot"
#define GAME_NAME "demo-fantasy"

inherit "/lib/core/game.c";

create()
{
   ::create();
   set_game_coordinator(GAME_COORDINATOR);
   set_game_name(GAME_NAME);
   set_open_read(0);
   set_open_write(0);
   set_finger_info("");

   save_me();

   // Look at /secure/dom/README for more info
}
