
#define GAME_COORDINATOR "neverbot"

#include <mud/genres.h>
#include <language.h>

inherit "/lib/core/game.c";

void create()
{
   ::create();
   set_game_coordinator(GAME_COORDINATOR);
   set_game_name(_LANG_GAME_NAME);
   set_game_short_description(_LANG_GAME_SHORT_DESC);

   // a demo: playable from a brand new account, and the way out of one
   set_demo(true);
   set_genre(GENRE_FANTASY);
   set_open_read(0);
   set_open_write(0);
   // set_finger_info("");

   // open to players
   set_open(true);

   save_me();
}
