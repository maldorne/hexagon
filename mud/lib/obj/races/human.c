
// Test race for hexagon mudlib, neverbot 06/2017
// based in ccmudlib human, by neverbot 4/2003

#include <living/races.h>

inherit STD_RACE;

void setup()
{
  // 2 brazos
  set_limbs(2);
  // Tamaño medio
  set_race_size(HUMAN_S);
  set_long("La raza humana es la más común en "+mud_name()+".\n");
  set_name("humano");
  set_short("Humano");
  set_light_limits(LHUMANL, LHUMANH);
  
  // set_subraces( ({"velan", "tresio", }) );
  // If you have subraces, the init_room will be set in each of them
  set_init_room("/game/areas/ciudad-capital/someplace.c");
}

void start_player(object ob) 
{ 
  ob->setmin("@$N llega desde $F.");
  ob->setmout("@$N se va hacia $T.");
}

void set_racial_bonuses(object ob)
{
  ob->adjust_bonus_cha(2);
}

string query_desc(object ob)
{
  if(ob)
  {
    if ((int)ob->query_gender() == 1)
      return "Un joven y apuesto humano.\n";
    return "Una joven y atractiva humana de aspecto calmado.\n";
  }
  return "";
}

string * query_initial_languages()
{
  return ({ "comun" });
}

int query_race_weight()
{
  return HUMAN_W;
}

string query_race_gender_string(object player, varargs int flag)
{
  if (!flag)
  {
    if (player->query_gender() == 2)
      return "la humana";
    return "el humano";
  }
  else
  {
    if (player->query_gender() == 2)
      return "humana";
    return "humano";
  }
}
 