
// Nuevas razas para Cc, Folken 4/2003
#include <living/races.h>

inherit STD_RACE;

void setup()
{
  // 2 brazos
  set_limbs(2);
  // Tamaño medio
  set_race_size(STD_S);
  set_long("Un ser informe, la viva imagen de la incertidumbre.\n");
  set_name("desconocido");
  set_short("Desconocido");
  set_light_limits(LHUMANL, LHUMANH);
}

int query_skill_bonus(int lvl, string skill)
{
  return 0;
}

string query_desc(object ob)
{
  return "Es algo realmente extraño, no sabrías ni decir si hembra o "+
      "macho.\n";
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
    if (player->query_gender()==2)
      return "la desconocida";
    return "el desconocido";
  }
  else
  {
    if (player->query_gender()==2)
      return "desconocida";
    return "desconocido";
  }
}

