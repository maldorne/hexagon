/*
 * start room for the demo-fantasy game
 * here the new players have to choose a class
 */

inherit "/lib/room";

#include <living/races.h>
#include <language.h>

#define ITEMS "/games/demo-fantasy/areas/start/items/"

void setup()
{
  set_light((LIGHT_STD_HIGH - LIGHT_STD_LOW)/2);
  set_short(game_master_object(this_object())->query_game_name() + ": " + _LANG_START_SHORT);
  set_long(_LANG_START_LONG + " " + implode(_LANG_START_HINTS, " ") + "\n");
}

int show_hints(object who)
{
  tell_object(who, "%^BOLD%^" + implode(_LANG_START_HINTS, "\n") + "%^RESET%^\n");
  return 1;
}

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who))
    call_out("show_hints", 1, who);

  ::event_enter(who, msg, from, avoid);
}
