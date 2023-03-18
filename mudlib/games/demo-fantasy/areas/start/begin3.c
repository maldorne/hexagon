/*
 * start room for the demo-fantasy game
 * here the new players have to arrange their character stats
 */

inherit "/lib/room";

#include <living/races.h>
#include <common/properties.h>
#include <language.h>
#include "path.h"

object orb;

void create()
{
  ::create();
}

void setup()
{
  set_light((LIGHT_STD_HIGH - LIGHT_STD_LOW)/2);
  set_short(game_master_object(this_object())->query_game_name() + ": " + _LANG_START3_SHORT);
  set_long(_LANG_START3_LONG + " " + implode(_LANG_START3_HINTS, " ") + "\n");  

  add_property(NOFEAT_PROP, 1);
  add_property(NOKILL_PROP, 1);
}

int show_hints(object who)
{
  tell_object(who, "%^BOLD%^" + implode(_LANG_START3_HINTS, "\n") + "%^RESET%^\n");
  return 1;
}

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who))
    call_out("show_hints", 1, who);

  ::event_enter(who, msg, from, avoid);
}


void init()
{
  if (!orb)
  {
    orb = clone_object(ITEMS + "arrange_orb.c");
    orb->move(this_object());
  }

  ::init();
}

void dest_me()
{
  if (orb)
    orb->dest_me();
    
  ::dest_me();
}