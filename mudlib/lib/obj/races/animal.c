#include <living/races.h>

inherit STD_RACE;

void setup()
{
  set_limbs(0);
  set_race_size(2);

  set_long("Un animal salvaje.\n");
  set_name("animal");
  set_short("Animal");
  set_light_limits(LHUMANL, LHUMANH);

  set_playable(0);
}

void set_racial_bonuses(object ob)
{
  return;
}

string query_desc(object ob)
{
  return "Un pequeño animal salvaje.\n";
}

string * query_initial_languages(){
  return ({  });
}

int query_race_weight(){
    return HUMAN_W/2;
}

string query_race_gender_string(object player, varargs int flag)
{
  return "";
}

string * query_locations()
{
  return ({"la cabeza","el cuerpo","la cola","una pata",});
}

// Funcion que devuelve datos suficientes para obtener un lugar en el que golpear
// a un ser de la raza en cuestion.
mixed obtain_location()
{
    float mult; // multiplicador al daño
    string name; // nombre de la localizacion

    switch (random(20)) 
    {
      case 0:      mult = 2.0;  name = "la cabeza";   break;
      case 1..10:  mult = 1.0;  name = "el cuerpo";   break;
      case 11..12: mult = 0.5;  name = "la cola";     break;
      case 13..20: mult = 1.0;  name = "una pata";    break;
    }

    /* Devolvemos: 
     * el multiplicador al daño por golpear en este lugar
     * el nombre del lugar en el que golpeamos
     * el tipo de 'cuerpo'
     */
    return ({ mult, name, "animal" });
}
