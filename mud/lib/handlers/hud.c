
// Nuevo sistema para ver enemigos, Folken 6/03

// A diferentes jugadores les gusta que aparezcan los nombres de otros jugadores/npcs
// de diferentes formas, asi que hacemos el metodo configurable.
// Opciones:
//    Alineamiento:   Si son enemigos, en color rojo (modo por defecto)
//    Dificultad: Diferentes colores segun la dificultad relativa ('considerar' automatico)

// PENDIENTE:
//    Raza:   Un color diferente segun la raza del personaje

#include <basic/alignment.h>
#include <user/hud.h>

string query_colored_name(object me, object he, string his_name, int how_many)
{
  int al_me, al_him;
  string aux, ret;
  object race;
  
  string hud;
  string color;

  hud = me->query_hud();
  color = "";

  if (!me)
    return he->query_cap_name();

  if (me == he)
    return he->query_cap_name();

  // al_me = 0;
  // al_him = 0;

  ret = "";

  al_me = (int)me->query_real_align();
  al_him = (int)he->query_ext_align();

  // Con los npcs (o players sin raza) no sacamos el sobrenombre con la raza!!
  if (!he->query_race_ob() || he->query_npc())
  {
    // Obtenemos el nombre
    if (he->query_npc()) 
    {
      if (how_many > 1) 
      {
        aux = capitalize(query_num(how_many, 0)) + " " +
          he->pretty_plural();
      } 
      else 
      {
        aux = capitalize(his_name);
      }
    } 
    else 
    {
      aux = capitalize(his_name);
    }
    
    // Tras el nombre obtenemos el color segun el tipo de apuntador
    if (hud == HUD_ALIGNMENT && ALIGN_TABLE->query_enemies(al_me, al_him))
    {
      color = "%^BOLD%^RED%^";
    }
    else if (hud == HUD_DIFFICULTY)
    {
      int dif;

      dif = he->query_level() - me->query_level();

      if (dif > 10)
        color = "%^BOLD%^RED%^";
      else if (dif > 5)
        color = "%^BOLD%^ORANGE%^";
      else if (dif > -2)
        color = "%^GREEN%^";
    }
    
    // Unimos color y nombre
    if (color != "")
      ret += color + aux + "%^RESET%^";
    else
      ret += aux;
    
  }
  else // Es player con raza
  {
    string new_str;

    new_str = "";
    race = he->query_race_ob();

    // No damos las subrazas/culturas, porque los nombres
    //  suelen ser mas largos y menos representativos
    // Algo simple como humano, elfo, etc da informacion mas
    //  rapido.
    // if (race->query_subrace())
    //  race = race->query_base_race();

    // ret += "%^BOLD%^RED%^" + capitalize(inv[0][i]) + " " + 
    //    race->query_race_gender_string(inv[1][i][0]) +
    //    "%^RESET%^";

    // Nueva forma de hacerlo para que funcionen bien las monturas
    // Folken 07/04
    ret += capitalize(his_name);

    // Tras el nombre obtenemos el color segun el tipo de apuntador
    if (hud == HUD_ALIGNMENT && ALIGN_TABLE->query_enemies(al_me, al_him))
    {
      color = "%^BOLD%^RED%^";
    }
    else if (hud == HUD_DIFFICULTY)
    {
      int dif;
      
      dif = he->query_level() - me->query_level();

      if (dif > 10)
        color = "%^BOLD%^RED%^";
      else if (dif > 5)
        color = "%^BOLD%^ORANGE%^";
      else if (dif >= -2)
        color = "%^GREEN%^";
    }

    if (color != "")
      new_str = color + he->query_cap_name() + " " + race->query_race_gender_string(he) + "%^RESET%^";
    else
      new_str = he->query_cap_name() + " " + race->query_race_gender_string(he);

    ret = replace_string(ret, he->query_cap_name(), new_str);
  }

  return ret;

}
/*
string query_colored_name(object me, object he, string his_name, int how_many)
{
  int al_me, al_him;
  string aux, ret;
  object race;

  if (!me)
    return he->query_cap_name();

  if (me == he)
    return he->query_cap_name();

  // al_me = 0;
  // al_him = 0;

  ret = "";

  al_me = (int)me->query_real_align();
  al_him = (int)he->query_ext_align();

  if (ALIGN_TABLE->query_enemies(al_me, al_him))
  {
    // Con los npcs no sacamos el sobrenombre con la raza!!
    if (!he->query_race_ob() || he->query_npc())
    {
      if (he->query_npc()) 
      {
        if (how_many > 1) 
        {
          aux = capitalize(query_num(how_many, 0)) + " " +
            he->pretty_plural();
        } 
        else 
        {
          aux = capitalize(his_name);
        }
      } 
      else 
      {
        aux = capitalize(his_name);
      }
      ret += "%^BOLD%^RED%^" + aux + "%^RESET%^";
    }
    else
    {
      race = he->query_race_ob();

      // No damos las subrazas/culturas, porque los nombres
      //  suelen ser mas largos y menos representativos
      // Algo simple como humano, elfo, etc da informacion mas
      //  rapido.
      if (race->query_subrace())
        race = race->query_base_race();

      // ret += "%^BOLD%^RED%^" + capitalize(inv[0][i]) + " " + 
      //    race->query_race_gender_string(inv[1][i][0]) +
      //    "%^RESET%^";

      // Nueva forma de hacerlo para que funcionen bien las monturas
      // Folken 07/04
      ret += capitalize(his_name);

      ret = replace_string(ret, he->query_cap_name(), "%^BOLD%^RED%^" +
        he->query_cap_name() + " " + race->query_race_gender_string(he)+ 
        "%^RESET%^");
    }
  }
  else
  {
    if (he->query_npc()) 
    {
      if (how_many > 1) 
      {
        aux = capitalize(query_num(how_many, 0)) + " " +
          // inv[1][i][0]->pretty_plural(inv[0][i]);
          he->pretty_plural();
      } 
      else 
      {
        aux = capitalize(his_name);
      }
    } 
    else 
    {
      aux = capitalize(his_name);
    }
    ret += aux;
  }

  return ret;

// return he->query_cap_name();
}
*/
