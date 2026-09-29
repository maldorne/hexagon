// items to choose the class at the beginning of the game

#include <user/player.h>
#include <language.h>
#include "../path.h"

inherit "/lib/item.c";

object item;
string class_ob;
string message;
string * items;

void set_class_ob(string n) { class_ob = n; }
string query_class_id() { return load_object(class_ob)->query_class_id(); }
void set_message(string n) { message = n; }
void set_items(string * list) { items += list; }

void create()
{
  class_ob = CLASSES_PATH + "fighter.c";
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

// Makes somebody of this item's class and sends them on to choose a race. A
// new character is given it; one arriving from another game with a class of
// there takes this one instead, and leaves behind everything it carries. A
// class of this game is kept as it is.
void choose(object player)
{
  object * carried;
  int i;

  if (!player->query_class_ob() ||
      game_from_path(player->query_class_ob()) != game_name(this_object()))
  {
    if (player->query_class_ob())
    {
      // nothing crosses from one game to another, but a coder's tools
      carried = all_inventory(player);
      for (i = 0; i < sizeof(carried); i++)
        if (file_name(carried[i])[0..14] != "/lib/obj/coder/")
          carried[i]->dest_me();

      // at least the 50 hit points a new character starts with
      if (player->query_max_hp() < 50)
        player->set_max_hp(50);
    }
    else
      // minimal base 50 hps
      player->set_max_hp(player->query_max_hp() + 50);

    // set the chosen class
    player->set_class_ob(class_ob);
    // adjust the level to 5
    if (player->query_level() < 5)
      player->adjust_level(5 - player->query_level());

    // Default skills are granted from living::start_player
    // (grant_default_skills), not here, so they reach every player on
    // login including existing ones, not only newly created characters.

    // if we need to do something special
    start_player(player);

    // initial items
    for (i = 0; i < sizeof(items); i++)
    {
      item = clone_object(items[i]);
      if (item)
        item->move(player);
    }

    tell_object(player, message);
  }

  player->move_living("X", CHOOSE_RACE_ROOM);
}

int do_choose(string str)
{
  if (!strlen(str))
  {
    notify_fail(_LANG_ITEMS_CHOOSE_FAIL);
    return 0;
  }
  
  // if we are getting this object
  if (id(str))
  {
    choose(this_player());
    return 1;
  }
  
  notify_fail(_LANG_ITEMS_CHOOSE_FAIL);
  return 0;
}
