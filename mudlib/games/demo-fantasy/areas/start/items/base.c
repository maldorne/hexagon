// items to choose the class at the beginning of the game

#include <user/player.h>
#include <language.h>

#define NEXT_ROOM "/games/demo-fantasy/areas/start/begin2.c"

inherit "/lib/item.c";

object item;
string class_ob;
string message;
string * items;

void set_class_ob(string n) { class_ob = n; }
void set_message(string n) { message = n; }
void set_items(string * list) { items += list; }

void create()
{
  class_ob = "/lib/obj/classes/fighter.c";
  message = _LANG_ITEMS_FIGHTER_MSG;

  items = MUST_HAVE;
  
  ::create();

  reset_get();
}

void init()
{
  ::init();
  add_action("do_choose", _LANG_ITEMS_CHOOSE_VERBS);
}

int start_player(object player)
{
  return 1;
}

int do_choose(string str)
{
  int i;

  if (!strlen(str))
  {
    notify_fail(_LANG_ITEMS_CHOOSE_FAIL);
    return 0;
  }
  
  // if we are getting this object
  if (id(str))
  {
    if (!this_player()->query_class_ob())
    {
      // set the chosen class
      this_player()->set_class_ob(class_ob);
      // adjust the level to 5
      this_player()->adjust_level(4);
      // minimal base 50 hps
      this_player()->set_max_hp(this_player()->query_max_hp() + 50);

      // this_player()->update_sheet_version(GLOBAL_SHEET_VERSION);

      // Feats in common for every player
      // this_player()->add_known_feat("seguir", 1);
      // this_player()->add_known_feat("examinar", 1);

      // if we need to do something special
      start_player(this_player());

      // initial items
      for (i = 0; i < sizeof(items); i++)
      {
        item = clone_object(items[i]);
        if (item)
          item->move(this_player());
      }
      
      write(message);
    }
    
    this_player()->move_living("X", NEXT_ROOM);
    return 1;
  }
  
  notify_fail(_LANG_ITEMS_CHOOSE_FAIL);
  return 0;
}
