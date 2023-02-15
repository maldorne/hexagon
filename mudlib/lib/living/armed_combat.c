/*
 *  Nuevo sistema de tecnicas de combate con armas (como complemento al
 *   sistema de lucha sin armas), Folken@CcMud, 6/2003
 *
 *  Mejora en el sistema de aprendizaje: ahora no contamos el numero de 
 *   golpes con exito en el arma, sino en el player. Siempre que no
 *   cambiemos el _tipo_ de arma (query_base_weapon)
 *   seguimos acumulando golpes (asi practicamos con un tipo de arma,
 *   no con una arma concreta). Folken 08/05
 * 
 *  Cambio en el sistema: cambiamos estilos por maestrias. La maestria es 
 *   es una especializacion en el tipo de arma que se esta utilizando
 *   (* weapon->query_weapon_family() *)
 *   Asi el aprendizaje es mas sencillo, con empuñar un arma se empieza a
 *   practicar. Folken 09/2012
 */

#include <living/combat.h>

// Las maestrias son un mapping con ([ "tipo de arma":valor ])
// siendo valor el porcentaje de control (0-100) sobre ese tipo de arma.
mapping known_weapon_masteries;

string current_weapon_mastery;

// Dominio sobre la maestria actual (lo hacemos variable propia)
// para agilizar los accesos (y minimizar la busqueda en el mapping)
static int current_mastery_value;

// Nuevo sistema de aprendizaje, Folken 08/05
int num_armed_hits;

int adjust_num_armed_hits(int i, string weapon_type);
int query_num_armed_hits();

void armed_combat_commands();

// Estilos
int set_weapon_mastery(string mastery);
int add_weapon_mastery(string mastery, varargs int value);
int remove_weapon_mastery(string mastery);
mapping query_known_weapon_masteries();

void armed_combat_commands()
{
  add_private_action("do_weapon_masteries", "maestrias");
  add_private_action("do_weapon_masteries", "maestrías");
}

void create()
{
  // Los jugadores comienzan sin saber usar armas
  // Al crear el personaje y escoger clase, les podemos dar un valor base segun los objetos
  // que les entreguemos
  known_weapon_masteries = ([ ]);
  current_weapon_mastery = "";
  current_mastery_value = 0;
  // num_armed_hits = 0;
}

int set_weapon_mastery(string mastery)
{
  if (member_array(mastery, keys(known_weapon_masteries)) > -1)
  {
      if (mastery != current_weapon_mastery)
      {
         num_armed_hits = 0;
         current_weapon_mastery = mastery;
      }  

      // outside the if, so when we wield the first weapon after login, 
      // the current_mastery_value gets updated 
      // (its nosave, while current_weapon_mastery IS saved)
      current_mastery_value = known_weapon_masteries[mastery];
      
      return 1;
  }
  // no tenemos la maestria
  else
  {
    notify_fail("No conoces la maestría "+mastery+".\n");
    return 0;
  }

  return 1;
}

int add_weapon_mastery(string mastery, varargs int value)
{
  object table;
  table = load_object(WEAPON_TABLE);

  if (!table)
  {
    notify_fail("La tabla de armas tiene un error. Notifícalo a un programador.\n");
    return 0;
  }

  if (member_array(mastery, table->query_weapon_masteries()) == -1 )
  {
    notify_fail("La maestría que vas a utilizar tiene un error.\n");
    return 0;
  }
  
  if (!mastery) 
   return 0;

  if (!known_weapon_masteries[mastery])
  {
    tell_player(this_object(), "¡Has ganado la maestría con el tipo de arma '"+mastery+"'!\n");
  }
  else
  {
    return known_weapon_masteries[mastery];
  }
  
  if (!value)
    known_weapon_masteries[mastery] = 1;
  else
    known_weapon_masteries[mastery] = value;
   
  return known_weapon_masteries[mastery];
}

int remove_weapon_mastery(string mastery)
{
  if (!mastery) 
  return 0;
  
  map_delete(known_weapon_masteries, mastery);

  if (mastery == current_weapon_mastery)
  {
    num_armed_hits = 0;
    current_weapon_mastery = "";
    current_mastery_value = 0;
  }
  return 1;
}

string query_current_weapon_mastery() { return current_weapon_mastery; }
int query_current_mastery_value() { return current_mastery_value; } 
mapping query_known_weapon_masteries() { return known_weapon_masteries; }

// Ajustamos el dominio sobre la maestria, devolvemos la
// habilidad resultante
int adjust_current_mastery_value(string mastery, int i)
{
   if (!known_weapon_masteries[mastery])
     return 0;

   known_weapon_masteries[mastery] += i;

   if (mastery == current_weapon_mastery)
     current_mastery_value += i;

   if (i > 0) 
   {
     // Si es el primer punto que subimos, o
     //  por debajo de 20, cada diez, o
     //  por debajo de 50, cada cinco, o
     //  por encima de 50, siempre
     if ( (current_mastery_value == 2) ||
          ((current_mastery_value > 2) && (current_mastery_value <= 20) && (current_mastery_value % 10 == 0)) ||
          ((current_mastery_value > 20) && (current_mastery_value <= 50) && (current_mastery_value % 5 == 0)) ||
          (current_mastery_value > 50) )    
    
      tell_player(this_object(), "¡Has mejorado tu maestría con "+mastery+"!\n");
   }

   return known_weapon_masteries[mastery];   
}

int adjust_num_armed_hits(int i, string weapon_type)
{
  // Removed this, with ranged weapons the increment is 10 by 10, Folken 08/2013
  // if (i > 1)
  //    i = 1;

  // Pueden ser golpes con armas secundarias, solo practicamos el arma principal
  if (weapon_type != current_weapon_mastery)
    return num_armed_hits;

  num_armed_hits += i;

  // Por debajo de 20 en maestria, aprendemos de cada golpe
  // Por debajo de 50, cada 10
  // Por encima de 50, cada 50 golpes

  if ( (current_mastery_value < 20) ||
      ((current_mastery_value < 50) && (num_armed_hits % 10 == 0)) ||
      ((current_mastery_value >= 50) && (num_armed_hits % 50 == 0)) )
  {
    // Cuanto mas dominemos la maestria, mas dificil es mejorarla
    if ((current_mastery_value < MAX_ARMED_AUTO_LEARN) && (random(100) > current_mastery_value))
    {
      adjust_current_mastery_value(current_weapon_mastery, 1);
    }

    num_armed_hits = 0;
  }

  return num_armed_hits;
}

int query_num_armed_hits() { return num_armed_hits; }

int do_weapon_masteries(string mastery)
{
  int i;
  string ret;
  string line;
  string * masteries;

  line = sprintf("\n%*'-'s\n", this_object()->query_cols(), "");
  masteries = keys(query_known_weapon_masteries());

  if (this_object()->query_dead())
  {
     notify_fail("Estás en forma espiritual, no necesitas conocer eso.\n");
     return 0;
  }

  ret = sprintf("%*'-'|s\n\n", this_object()->query_cols()+18, "> %^GREEN%^Posees las siguientes maestrías con armas: %^RESET%^<");

  for (i = 0; i < sizeof(masteries); i++)
  {
      ret += sprintf("\t%35-s %25|s (%s)\n", "%^BOLD%^"+capitalize(masteries[i]) +"%^RESET%^", 
                "["+percentage_bar(query_known_weapon_masteries()[masteries[i]])+"]",
                ""+query_known_weapon_masteries()[masteries[i]]+"%");
  }

  ret += line;

  tell_object(this_object(), ret);
  return 1;
  
}

// stats añadido
mixed stats() 
{
  mixed * ret;
  ret = ({ ({"Known Armed Masteries", known_weapon_masteries, }),
           ({"Current Weapon Mastery", current_weapon_mastery, }),
           ({"Num Armed Hits", num_armed_hits, }),
       });
  return ret;
}
