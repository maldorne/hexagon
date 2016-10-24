// 
// Nuevos sistema de dotes para CcMud, Folken 11/10/08
//
// - Los antiguos comandos (std/commands) pasamos a llamarlos dotes (obj/feats)
//   (feats) para diferenciarlos de los comandos de /cmds
// - Extraida la funcionalidad principal de /global/living/groups_obs.c
// - Reprogramado por completo 18/10/08

#include <mud/secure.h>
#include <living/feats.h>
#include <common/properties.h>

string * known_feats;         // nombre de cada dote (tiene que coincidir
                              // con los nombres de /table/feats_table.c)
mapping feat_list;            // feats_list[nombre] = ({ lista })
                              //    usamos como nombres los de la lista known_feats
                              // Este mapping sera un subconjunto de la lista
                              // de /table/feats_table.c, unicamente con los que el 
                              // player conoce
                              //    lista == ({ filename feat, porcentaje, times_used, pasivo})
                              //    si es pasivo no tiene accion que ejecutar asociada

void create()
{
  feat_list = ([ ]);
  known_feats = ({ });
}

mapping query_feat_list() { return feat_list; }
string * query_known_feats() { return known_feats; }

void set_feat_list(mapping feats) 
{ 
  int i;
  string * indices;

  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;  

  feat_list = ([ ]);
  known_feats = ({ });
  indices = keys(feats);
  
  for (i = 0; i < sizeof(indices); i++)
  {
    feat_list[indices[i]] = ({ }) + feats[indices[i]];
    known_feats += ({ indices[i] });
  }
}

// ¿Conoce una dote concreta?
int query_known_feat(string name)
{
  return (member_array(name, known_feats) != -1);
}

int query_feat_ability(string str);
int adjust_feat_ability(string str, int value, varargs int silence);
int update_feat_used_times(string str);
int query_feat_used_times(string str);

// HACK
/*
  string * query_known_commands() { return ({ }); }
  int add_known_command(string str){ return 1; }
  string help_command(string str) { return ""; }
*/

void feats_commands()
{
  int i;
  mixed * feat;
  object feat_ob;
  string * pieces;
  string aux;

  add_action("list_feats", "dotes");
  add_action("list_feats", "habilidades");
  
  for (i = 0; i < sizeof(known_feats); i++) 
  {
    // Las dotes pasivas no tienen accion asociada
    if (feat_list[known_feats[i]][3] == PASSIVE_FEAT)
      continue;      
    
    feat = FEATS_TABLE->query_feat_data(known_feats[i]);
    
    if (!sizeof(feat))
      continue;
    
    catch(feat_ob = load_object(feat[FEAT_DATA_PATH]));

    // feat_list[known_feats[i]] = 
    //     (string *)feat_SERVER->query_feat(known_feats[i]);

    // Si la dote/comando no carga, no añadimos la accion al player
    if (!objectp(feat_ob))
      continue;
    
    // Importante: si la dote es de varias palabras, el add_action es
    // unicamente sobre la primera, cuando ejecutemos el comando
    // tendremos que comprobar si las siguientes tambien se corresponden
    
    pieces = explode(known_feats[i], " ");
    
    add_action("do_feat", pieces[0]);

    // Tambien damos la accion sin acentos ni otros simbolos
    if ((aux = FEATS_TABLE->feat_translate_to_action(pieces[0])) != "")
      add_action("do_feat", aux);
    
    // tell_object(find_living("folken"), "[FEAT] add_action(do_feat, "+pieces[0]+")\n");
  }
}

int list_feats() 
{
  string ret, feat_name;
  int passive_feats_found;
  string * categories;
  int i, j;

  ret = "";
  feat_name = "";
  passive_feats_found = 0;
  categories = ({ });
  i = 0;
  j = 0;

  if (this_object()->query_dead())
  {
    notify_fail("Estás en forma espiritual, no necesitas conocer eso.\n");
    return 0;
  }
      
  if (!sizeof(known_feats))
  {
    notify_fail("Aún no has conseguido ninguna dote.\n");
    return 0;
  }
    
  // Rellenamos una lista con todas las categorias de dotes
  for (i = 0; i < sizeof(known_feats); i++)
  {
    object f;
    string cat;

    f = load_object(feat_list[known_feats[i]][0]);
    
    if (!f)
      continue;
    
    cat = implode(f->query_categories(), " - ");
    
    if (member_array(cat, categories) == -1)
      categories += ({ cat });
  }


  ret = sprintf("%*'-'|s\n\n", this_object()->query_cols()+18, 
      "> %^GREEN%^Conoces las siguientes dotes: %^RESET%^<");
  
  for (j = 0; j < sizeof(categories); j++)
  {
    ret += "    %^GREEN%^" + capitalize(categories[j]) + "%^RESET%^:\n";
    
    for (i = 0; i < sizeof(known_feats); i++)
    {
      object f;
      string cat;

      f = load_object(feat_list[known_feats[i]][0]);

      if (!f)
        continue;

      cat = implode(f->query_categories(), " - ");
      
      if (categories[j] != cat)
        continue;
        
      if (feat_list[known_feats[i]][3] == ACTIVE_FEAT)
        feat_name = "%^BOLD%^"+capitalize(known_feats[i]) +"%^RESET%^";
      else
      {
        feat_name = "%^BOLD%^"+capitalize(known_feats[i]) +"%^RESET%^ (*)";
        passive_feats_found = 1;
      }
      
      ret += sprintf("\t%35-s %25|s (%s)\n", feat_name, 
                "["+percentage_bar(query_feat_ability(known_feats[i]))+"]",
                ""+query_feat_ability(known_feats[i])+"%");
    }
  }
    
  if (passive_feats_found)
    ret += "\n (*) Dotes pasivas\n";
    
  ret += sprintf("\n%*'-'s\n", this_object()->query_cols(), "");    
  tell_object(this_object(), ret);
  
  return 1;
}

int add_known_feat(string str, varargs int silence) 
{
  mixed * feat;
  object feat_ob;
  
  if (!silence)
    silence = 0;

  // Si ya conocemos la dote  
  if (member_array(str, known_feats) != -1)
    return 0;

  feat = FEATS_TABLE->query_feat_data(str);
  
  // Si la tabla no nos ha devuelto datos
  if (!arrayp(feat) || !sizeof(feat))
    return 0;
  
  catch(feat_ob = load_object(feat[FEAT_DATA_PATH]));

  // Si la dote/comando no carga
  if (!objectp(feat_ob))
    return 0;

  // Añadimos la dote
  known_feats += ({ str });
  feat_list[str] = ({ }) + feat;
  
  // Informamos al player
  if (!silence)
    tell_player(this_object(), "¡Has obtenido la dote "+capitalize(str)+"!\n");

  // Actualizamos los comandos
  feats_commands();
  
  return 1;
    
} /* add_known_feat() */

int remove_known_feat(string str) 
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_feats)) == -1)
    return 0;
    
  known_feats = delete(known_feats, i, 1);
  feat_list = m_delete(feat_list, str);
  
  return 1;
    
} /* remove_known_feat() */

// Porcentaje de conocimiento sobre una dote
int query_feat_ability(string str)
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_feats)) == -1)
    return 0;

  return feat_list[str][1];
}

int adjust_feat_ability(string str, int value, varargs int silence)
{
  int i;

  if (!silence)
    silence = 0;

  // No la conocemos
  if ((i = member_array(str, known_feats)) == -1)
    return 0;

  feat_list[str][1] += value;
    
  if (feat_list[str][1] <= 1)
      feat_list[str][1] = 1;

  // Reseteamos el numero de veces usadas
  feat_list[str][2] = 0;

  if ((value > 0) && !silence)
    tell_player(this_object(), "¡Has mejorado tus habilidades en "+str+"!\n");

  return feat_list[str][1];
}

int update_feat_used_times(string str)
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_feats)) == -1)
    return 0;

  // Si ya estamos al maximo no seguimos actualizando
  if (feat_list[str][1] < 100)
  {
    feat_list[str][2] += 1;
    
    // Cada 50 usos de un feat, vemos si aumentamos la habilidad
    if (feat_list[str][2] % 50 == 0)
    {
        if (random(100) > feat_list[str][1])
          adjust_feat_ability(str, 1);
    }
  }

  return feat_list[str][2];
}

int query_feat_used_times(string str)
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_feats)) == -1)
    return 0;

  return feat_list[str][2];
}

// Funcion que ejecuta la dote (si tiene comando asociado)
int do_feat(string str)   
{
  int i, j;
  int found;
  int result;
  string * feat_pieces;
  string * str_pieces;

  i, j = 0;
  found = -1;
  result = 0;
  feat_pieces = ({ });

  if (!strlen(str))
  {
    str = "";
    str_pieces = ({ str });
  }
  else
    str_pieces = explode(str, " "); 

  if (!known_feats) 
  {
    known_feats = ({ });
    return 0;
  }

  if (!mappingp(feat_list))
    feat_list = ([ ]);

  // tell_object(find_living("folken"), "[FEAT] verb='"+query_verb()+"' str='"+str+"'\n");
  
  // Buscamos el comando
  
  // Obviamente esta no es la mejor forma de buscar un comando que pueda incluir
  // espacios en blanco, pero entre añadir esta funcionalidad unicamente aqui 
  // (se ejecutan poco) o en el sistema de add_action generico (se ejecuta
  // constantemente), creo que sera mas eficiente
  
  for (i = 0; (i < sizeof(known_feats)) && (found == -1); i++)
  {
    feat_pieces = explode(known_feats[i], " ");
    if ((feat_pieces[0] == query_verb()) ||
    // Tambien nos vale el mismo nombre pero sin acentos ni simbolos
    (feat_pieces[0] == FEATS_TABLE->feat_translate(query_verb())) )
    {
      found = i;
      
      for (j = 1; (j < sizeof(feat_pieces)) && (found == -1); j++)
      {
        // No hay coincidencia
        if ((j - 1 < sizeof(str_pieces)) && 
            (feat_pieces[j] != str_pieces[j - 1]))
          found = -1;
      }
    }
  }

  if (found == -1)  
    return 0;
  /*
  {
      tell_object(find_living("folken"), "[FEAT] found="+found+", feat=<no encontrado>\n");
      return 0;
  }
  else
  {
      tell_object(find_living("folken"), "[FEAT] found="+found+", feat="+known_feats[found]+"\n");
  }
  */
  
  // Primero buscamos si hay una coincidencia exacta
  // if((i = member_array(query_verb(), my_feats)) != -1) 

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (this_object()->query_dead())
  {
    notify_fail("Eres un espíritu, por lo que no puedes hacer uso " +
      "de tus dotes.\n" );
    return 0;
  }    

  // llamamos a la funcion cast_effect de la dote, pasandole como parametros
  // el string (objetivos a buscar), el iniciador del efecto, y quiet como false
  result = (int)call_other( feat_list[known_feats[found]][0], 
                          "cast_effect", 
                          str, 
                          this_object(), 
                          0);

  // ELIMINADO!!!
  // Algunas dotes (las de fabricacion de objetos, de tipo "pass") pueden ejecutarse sin fabricar nada, 
  // y esto haria que se pudieran mejorar sin estar utilizandolas realmente. Ahora el aprendizaje
  // se realiza en /global/living/effects.c, al final del do_active_effects del ultimo round de la dote
  // solo, si se ha ejecutado correctamente (igual que el gasto de gps) 

  // Si el efecto se ha llegado a lanzar (hemos cumplido todos sus requisitos basicos),
  // actualizamos el numero de veces usadas. Aun asi es posible que, si el efecto es de varios
  // turnos, pueda no haber tenido exito en alguno de los posteriores (pero ha llegado a lanzarse)
  // ... Algo aprendes incluso cuando las cosas salen mal ...
  // if (result == 1)
  //     update_feat_used_times(known_feats[found]);
  
  return result;
}
  
int feat_damage(int damage, string type, object attacker)
{
  int prot;
  
  prot = this_object()->query_res(type);
      
  // Debug de informacion a los inmortales (Folken 7/01)
  if ((prot != 0) && this_object()->query_coder())
    tell_object(this_object(), "DEBUG (feat_damage): Aplicada resistencia " +
                                "contra '"+type+"' ("+prot+"%).\n");
  
  // Calculamos al modificacion al daño segun resistencias:
  damage = (damage * (100 - prot)) / 100;
                        
  if(!attacker)
  {
    return (this_object()->query_hp() - this_object()->adjust_hp(-damage) );
    // return 1;
  }
  else 
    return (this_object()->query_hp() - this_object()->adjust_hp(-damage, attacker) );
    // return 1;
}
                                                                                
string help_feat(string str) 
{
  object table;
  string aux;

  table = load_object(FEATS_TABLE);

  if (!table)
    return "";
  
  if (!pointerp(feat_list[str]))
  {
    aux = table->feat_translate(str);
    if (str != aux)
      str = aux;
    else
      return "";
  }

  return (string)feat_list[str][0]->help(str);
} 

mixed * stats() 
{
  return ({ 
    ({"Known feats", known_feats, }),
    ({"Feat list", feat_list, }),             
  });
}
