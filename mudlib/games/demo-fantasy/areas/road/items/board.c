// Newbie info :) Gestur'97

inherit "/lib/item.c";

#include <translations/language.h>

void setup()
{
  set_short("%^BOLD%^Board with information%^RESET%^");
  set_name("board");
  set_long("This is a board where the coders of " + mud_name() + " can write " +
    "some info to newbie users and coders.\n");
  add_alias("ayuda");
  add_alias("novato");
  reset_get();
  reset_drop();
  set_read_mess("Here we will write somo information about " + mud_name() + 
    ". It's not written yet.\n", STD_LANG);
}
