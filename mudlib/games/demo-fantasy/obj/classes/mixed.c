// files for classes, neverbot 6/03

#include <user/xp.h>
#include <language.h>

inherit "/lib/class.c";

void setup()
{
   set_name(_LANG_MIXED_CLASS_NAME);
   set_short(capitalize(_LANG_MIXED_CLASS_NAME));

   set_combat_bonus(3); // combat ability

   set_hit_dice(6);
   set_gp_dice(5);

   // 0 by default 
   // set_hp_bonus(0);
   // set_gp_bonus(0);

   // important in ALL classes!!!!
   set_xp_types(([
                  ARMED_COMBAT_XP : 70, 
                  UNARMED_COMBAT_XP : 70, 
                  KILL_XP : 50, 
                  SKILL_XP : 100
                ]));
}

int query_legal_race(string str)
{
  // every race can be of mixed class
  return 1;
}  

int query_gp_main_stat(object player)
{
  if (player)
    return player->query_dex();
}
