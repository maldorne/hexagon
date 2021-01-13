// Edited by Lummen 11-12-97
// Moved to CcMud, neverbot 6/03
// Used again in hexagon, neverbot 01/2021

#include <language.h>

inherit "/lib/item.c";

void setup() {
  set_name(_LANG_ITEMS_FOUNTAIN_NAME);
  set_short(_LANG_ITEMS_FOUNTAIN_SHORT);
  add_alias(_LANG_ITEMS_FOUNTAIN_ALIASES);
  set_long(_LANG_ITEMS_FOUNTAIN_LONG);

  set_gender(1);
  reset_get();
}

void init() {
  ::init();
  add_action("do_drink", _LANG_ITEMS_FOUNTAIN_DRINK_VERBS);
}

int do_drink(string str) 
{
  if (strlen(str) && member_array(str, _LANG_ITEMS_FOUNTAIN_TARGETS) != -1)
  {
    tell_object(this_player(), _LANG_ITEMS_FOUNTAIN_DRINK_MSG_ME);
    tell_room(environment(this_object()), _LANG_ITEMS_FOUNTAIN_DRINK_MSG_OTHERS, this_player());
    return 1;
  }
  
  notify_fail(_LANG_ITEMS_FOUNTAIN_FAIL);
  return 0;
}
