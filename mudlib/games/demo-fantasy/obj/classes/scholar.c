// files for classes, neverbot 6/03

#include <user/xp.h>
#include <language.h>

inherit "/lib/class.c";

void setup()
{
   set_name(_LANG_SCHOLAR_CLASS_NAME);
   set_short(capitalize(_LANG_SCHOLAR_CLASS_NAME));

   set_combat_bonus(2); // combat ability

   set_hit_dice(4);
   set_gp_dice(6);

   // 0 by default 
   // set_hp_bonus(0);
   // set_gp_bonus(0);

   // important in ALL classes!!!!
   set_xp_types(([
                  ARMED_COMBAT_XP : 50, 
                  UNARMED_COMBAT_XP : 50, 
                  KILL_XP : 10, 
                  MAGIC_XP : 100
                ]));
}

int query_legal_race(string str)
{ 
  // every race can be a scholar
  return 1;
}  

int query_gp_main_stat(object player)
{
  if (player)
    return player->query_wis();
}
