// Archivos para las Clases, neverbot 6/03

#include <user/xp.h>

inherit "/lib/class.c";

void setup()
{
   set_name("guerrero");
   set_short("Guerrero");

   set_combat_bonus(4); // Habilidad para el combate

   set_hit_dice(8);
   set_gp_dice(6);

   // Son 0 por defecto
   // set_hp_bonus(0);
   // set_gp_bonus(0);

   // Importante en TODAS las clases!!!!
   set_xp_types(([ARMED_COMBAT_XP:100, UNARMED_COMBAT_XP:80, KILL_XP:50]));
}

int query_legal_race(string str){ 
  // Todas las razas pueden ser guerrero
  return 1;
}  

int query_gp_main_stat(object player){
  if (player)
    return player->query_str();
}

