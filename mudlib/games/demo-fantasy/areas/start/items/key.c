// items to choose the class at the beginning of the game

#include <language.h>

inherit "/games/demo-fantasy/areas/start/items/base.c";

void setup() 
{
  set_name(_LANG_KEY_NAME);
  set_short(capitalize(_LANG_KEY_NAME));
  set_main_plural(capitalize(pluralize(_LANG_KEY_NAME)));
  add_plural(pluralize(_LANG_KEY_NAME));

  set_long(_LANG_KEY_DESC);

  set_class_ob("/lib/obj/classes/mixed.c");
  set_message(_LANG_ITEMS_MIXED_MSG);
  set_items( ({
        "/games/demo-fantasy/baseobs/weapons/dagger.c",
        "/games/demo-fantasy/baseobs/armours/shirt.c",
        "/games/demo-fantasy/baseobs/armours/trousers.c",
      }));
  }

// if we need to do something special
int start_player(object player)
{
  // flag for not giving messages
  // player->adjust_feat_ability("seguir", 20, 1);
  return 1;
}
