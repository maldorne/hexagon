/* Edited by Lummen and Antiron 15-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_HEALER_NAME);
  set_short(_LANG_NPCS_HEALER_SHORT);
  add_alias(_LANG_NPCS_HEALER_ALIASES);
  
  set_main_plural(_LANG_NPCS_HEALER_PLURAL);
  add_plural(_LANG_NPCS_HEALER_PLURAL_ALIASES);
  
  set_long(_LANG_NPCS_HEALER_LONG);
  
  set_gender(1);
  
  // the race object
  set_race_ob(OBJ + "races/human.c");
  set_class_ob("/lib/obj/classes/scholar.c");
  
  set_random_stats(6, 16);
  set_level(16);
  set_gender(1);
  set_real_align(-100);
  set_max_hp(1000);
  set_hp(1000);

  load_chat(50, ({
    1, _LANG_NPCS_HEALER_CHAT[0],
    1, _LANG_NPCS_HEALER_CHAT[1],
    1, _LANG_NPCS_HEALER_CHAT[2]
  }));

  adjust_money(5 + random(5), "gold");
  add_clone(BASEOBS + "weapons/mace", 1);
  add_clone(BASEOBS + "armours/cape", 1);
  add_clone(BASEOBS + "armours/chain_mail", 1);

  init_equip();
}

void init()
{
  add_action("do_raise", _LANG_NPCS_HEALER_RAISE_COMMANDS);
  add_action("do_info", _LANG_NPCS_HEALER_INFO_COMMANDS);

  // add_attack_spell(40, "something here", ({"/std/spells/cleric/cause_lt", "cast_spell", 0}));

  ::init();
} 

int do_info(string str)
{
  do_say(_LANG_NPCS_HEALER_INFO);
  return 1;
}

int do_raise(string str)
{
  if (this_player()->query_timed_property("thran_raise_abuse")) 
  {
    do_say(_LANG_NPCS_HEALER_RAISE_TOO_SOON);
    return 1; 
  }

  if (!this_player()->query_dead())
  {
    do_say(_LANG_NPCS_HEALER_RAISE_NO);
    return 1;
  }

  tell_object(this_player(), _LANG_NPCS_HEALER_RAISE_YES_ME);
  tell_room(environment(this_player()), _LANG_NPCS_HEALER_RAISE_YES_ROOM, this_player());
  
  this_player()->remove_ghost();
  this_player()->add_timed_property("thran_raise_abuse", 1, 100);
  return 1;
}
