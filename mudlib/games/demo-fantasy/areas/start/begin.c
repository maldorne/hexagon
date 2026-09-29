/*
 * start room for the demo-fantasy game
 * here the new players have to choose a class
 */

inherit "/lib/room";

#include <living/races.h>
#include <common/properties.h>
#include <language.h>
#include "path.h"

object sword, key, book;

void setup()
{
  set_light((LIGHT_STD_HIGH - LIGHT_STD_LOW)/2);
  set_short(game_master_object(this_object())->query_game_name() + ": " + _LANG_START_SHORT);
  set_long(_LANG_START_LONG + " " + implode(_LANG_START_HINTS, " ") + "\n");

  add_property(NOSKILL_PROP, 1);
  add_property(NOKILL_PROP, 1);
}

int show_hints(object who)
{
  tell_object(who, "%^BOLD%^" + implode(_LANG_START_HINTS, "\n") + "%^RESET%^\n");
  return 1;
}

// Somebody who already has a class does not choose again: with a class of this
// game they go straight on, and with one from another game the item of the same
// class here takes them. With no such item, they choose like anybody else.
void pass_through(object who)
{
  object * here;
  int i;

  if (!who || environment(who) != this_object())
    return;

  if (game_from_path(who->query_class_ob()) == game_name(this_object()))
  {
    who->move_living("X", CHOOSE_RACE_ROOM);
    return;
  }

  here = all_inventory(this_object());

  for (i = 0; i < sizeof(here); i++)
    if (!living(here[i]) && here[i]->query_class_id() == who->query_class_id())
    {
      here[i]->choose(who);
      return;
    }

  show_hints(who);
}

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who))
    call_out(who->query_class_ob() ? "pass_through" : "show_hints", 1, who);

  ::event_enter(who, msg, from, avoid);
}

void init()
{
  if (!sword)
  {
    sword = clone_object(ITEMS + "sword.c");
    sword->move(this_object());
  }

  if (!key)
  {
    key = clone_object(ITEMS + "key.c");
    key->move(this_object());
  }

  if (!book)
  {
    book = clone_object(ITEMS + "book.c");
    book->move(this_object());
  }

  ::init();
}

void dest_me()
{
  if (sword)
    sword->dest_me();
  if (key)
    key->dest_me();
  if (book)
    book->dest_me();
    
  ::dest_me();
}