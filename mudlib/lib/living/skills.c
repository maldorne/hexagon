// 
// Nuevos sistema de dotes para CcMud, Folken 11/10/08
//
// - Los antiguos comandos (std/commands) pasamos a llamarlos dotes (obj/skills)
//   (skills) para diferenciarlos de los comandos de /cmds
// - Extraida la funcionalidad principal de /global/living/groups_obs.c
// - Reprogramado por completo 18/10/08

#include <mud/secure.h>
#include <living/skills.h>
#include <common/properties.h>
#include <language.h>

string * known_skills;         // nombre de cada dote (tiene que coincidir
                              // con los nombres de /table/skills_table.c)
mapping skill_list;            // skills_list[nombre] = ({ lista })
                              //    usamos como nombres los de la lista known_skills
                              // Este mapping sera un subconjunto de la lista
                              // de /table/skills_table.c, unicamente con los que el 
                              // player conoce
                              //    lista == ({ filename skill, porcentaje, times_used, pasivo})
                              //    si es pasivo no tiene accion que ejecutar asociada

void create()
{
  skill_list = ([ ]);
  known_skills = ({ });
}

mapping query_skill_list() { return skill_list; }
string * query_known_skills() { return known_skills; }

void set_skill_list(mapping skills) 
{ 
  int i;
  string * indices;

  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;  

  skill_list = ([ ]);
  known_skills = ({ });
  indices = keys(skills);
  
  for (i = 0; i < sizeof(indices); i++)
  {
    skill_list[indices[i]] = ({ }) + skills[indices[i]];
    known_skills += ({ indices[i] });
  }
}

// ¿Conoce una dote concreta?
int query_known_skill(string name)
{
  return (member_array(name, known_skills) != -1);
}

int query_skill_ability(string str);
int adjust_skill_ability(string str, int value, varargs int silence);
int update_skill_used_times(string str);
int query_skill_used_times(string str);

// HACK
/*
  string * query_known_commands() { return ({ }); }
  int add_known_command(string str){ return 1; }
  string help_command(string str) { return ""; }
*/

void skills_commands()
{
  int i;
  mixed * skill;
  object skill_ob;
  string * pieces;
  string aux;

  {
    string * verbs;
    int v;
    verbs = _LANG_SKILL_LIST_VERBS;
    for (v = 0; v < sizeof(verbs); v++)
      add_private_action("list_skills", verbs[v]);
  }

  for (i = 0; i < sizeof(known_skills); i++) 
  {
    // Las dotes pasivas no tienen accion asociada
    if (skill_list[known_skills[i]][3] == PASSIVE_SKILL)
      continue;      
    
    skill = SKILLS_TABLE->query_skill_data(known_skills[i]);
    
    if (!sizeof(skill))
      continue;
    
    catch(skill_ob = load_object(skill[SKILL_DATA_PATH]));

    // skill_list[known_skills[i]] = 
    //     (string *)skill_SERVER->query_skill(known_skills[i]);

    // Si la dote/comando no carga, no añadimos la accion al player
    if (!objectp(skill_ob))
      continue;
    
    // Importante: si la dote es de varias palabras, el add_private_action es
    // unicamente sobre la primera, cuando ejecutemos el comando
    // tendremos que comprobar si las siguientes tambien se corresponden
    
    pieces = explode(known_skills[i], " ");
    
    add_private_action("do_skill", pieces[0]);

    // Tambien damos la accion sin acentos ni otros simbolos
    if ((aux = SKILLS_TABLE->skill_translate_to_action(pieces[0])) != "")
      add_private_action("do_skill", aux);
    
    // tell_object(find_living("folken"), "[SKILL] add_private_action(do_skill, "+pieces[0]+")\n");
  }
}

int list_skills(varargs string str)
{
  string ret, skill_name;
  int passive_skills_found;
  string * categories;
  int i, j;

  ret = "";
  skill_name = "";
  passive_skills_found = 0;
  categories = ({ });
  i = 0;
  j = 0;

  if (this_object()->query_dead())
  {
    notify_fail(_LANG_SKILL_DEAD_LIST);
    return 0;
  }
      
  if (!sizeof(known_skills))
  {
    notify_fail(_LANG_SKILL_NONE_YET);
    return 0;
  }
    
  // Rellenamos una lista con todas las categorias de dotes
  for (i = 0; i < sizeof(known_skills); i++)
  {
    object f;
    string cat;

    f = load_object(skill_list[known_skills[i]][0]);
    
    if (!f)
      continue;
    
    cat = implode(f->query_categories(), " - ");
    
    if (member_array(cat, categories) == -1)
      categories += ({ cat });
  }


  ret = sprintf("%*'-'|s\n\n", this_user()->query_cols()+18,
      _LANG_SKILL_LIST_HEADER);
  
  for (j = 0; j < sizeof(categories); j++)
  {
    ret += "    %^GREEN%^" + capitalize(categories[j]) + "%^RESET%^:\n";
    
    for (i = 0; i < sizeof(known_skills); i++)
    {
      object f;
      string cat;

      f = load_object(skill_list[known_skills[i]][0]);

      if (!f)
        continue;

      cat = implode(f->query_categories(), " - ");
      
      if (categories[j] != cat)
        continue;
        
      if (skill_list[known_skills[i]][3] == ACTIVE_SKILL)
        skill_name = "%^BOLD%^"+capitalize(known_skills[i]) +"%^RESET%^";
      else
      {
        skill_name = "%^BOLD%^"+capitalize(known_skills[i]) +"%^RESET%^ (*)";
        passive_skills_found = 1;
      }
      
      ret += sprintf("\t%35-s %25|s (%s)\n", skill_name, 
                "["+percentage_bar(query_skill_ability(known_skills[i]))+"]",
                ""+query_skill_ability(known_skills[i])+"%");
    }
  }
    
  if (passive_skills_found)
    ret += _LANG_SKILL_PASSIVE_LEGEND;
    
  ret += sprintf("\n%*'-'s\n", this_user()->query_cols(), "");
  tell_object(this_object(), ret);
  
  return 1;
}

// initial_ability > 0 overrides the table's starting percentage (used
// e.g. when the maze teaches orientation at the 10% floor). 0 keeps the
// table default.
int add_known_skill(string str, varargs int silence, int initial_ability)
{
  mixed * skill;
  object skill_ob;

  if (!silence)
    silence = 0;

  // Si ya conocemos la dote  
  if (member_array(str, known_skills) != -1)
    return 0;

  skill = SKILLS_TABLE->query_skill_data(str);
  
  // Si la tabla no nos ha devuelto datos
  if (!arrayp(skill) || !sizeof(skill))
    return 0;
  
  catch(skill_ob = load_object(skill[SKILL_DATA_PATH]));

  // Si la dote/comando no carga
  if (!objectp(skill_ob))
    return 0;

  // Añadimos la dote
  known_skills += ({ str });
  skill_list[str] = ({ }) + skill;

  // Ability inicial forzada (ej: aprendizaje por suerte en el laberinto)
  if (initial_ability > 0)
    skill_list[str][1] = initial_ability;

  // Informamos al player
  if (!silence)
    tell_player(this_object(),
      _LANG_SKILL_GAINED_PRE + capitalize(str) + _LANG_SKILL_GAINED_POST);

  // Actualizamos los comandos
  skills_commands();
  
  return 1;
    
} /* add_known_skill() */

int remove_known_skill(string str) 
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_skills)) == -1)
    return 0;
    
  known_skills = delete(known_skills, i, 1);
  skill_list = m_delete(skill_list, str);
  
  return 1;
    
} /* remove_known_skill() */

// Porcentaje de conocimiento sobre una dote
int query_skill_ability(string str)
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  return skill_list[str][1];
}

int adjust_skill_ability(string str, int value, varargs int silence)
{
  int i;

  if (!silence)
    silence = 0;

  // No la conocemos
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  skill_list[str][1] += value;
    
  if (skill_list[str][1] <= 1)
      skill_list[str][1] = 1;

  // Reseteamos el numero de veces usadas
  skill_list[str][2] = 0;

  if ((value > 0) && !silence)
    tell_player(this_object(),
      _LANG_SKILL_IMPROVED_PRE + str + _LANG_SKILL_IMPROVED_POST);

  return skill_list[str][1];
}

int update_skill_used_times(string str)
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  // Si ya estamos al maximo no seguimos actualizando
  if (skill_list[str][1] < 100)
  {
    skill_list[str][2] += 1;
    
    // Cada 50 usos de un skill, vemos si aumentamos la habilidad
    if (skill_list[str][2] % 50 == 0)
    {
        if (random(100) > skill_list[str][1])
          adjust_skill_ability(str, 1);
    }
  }

  return skill_list[str][2];
}

int query_skill_used_times(string str)
{
  int i;

  // No la conocemos
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  return skill_list[str][2];
}

// Funcion que ejecuta la dote (si tiene comando asociado)
int do_skill(string str)   
{
  int i, j;
  int found;
  int result;
  string * skill_pieces;
  string * str_pieces;

  i, j = 0;
  found = -1;
  result = 0;
  skill_pieces = ({ });

  if (!strlen(str))
  {
    str = "";
    str_pieces = ({ str });
  }
  else
    str_pieces = explode(str, " "); 

  if (!known_skills) 
  {
    known_skills = ({ });
    return 0;
  }

  if (!mappingp(skill_list))
    skill_list = ([ ]);

  // tell_object(find_living("folken"), "[SKILL] verb='"+query_verb()+"' str='"+str+"'\n");
  
  // Buscamos el comando
  
  // Obviamente esta no es la mejor forma de buscar un comando que pueda incluir
  // espacios en blanco, pero entre añadir esta funcionalidad unicamente aqui 
  // (se ejecutan poco) o en el sistema de add_action generico (se ejecuta
  // constantemente), creo que sera mas eficiente
  
  for (i = 0; (i < sizeof(known_skills)) && (found == -1); i++)
  {
    skill_pieces = explode(known_skills[i], " ");
    if ((skill_pieces[0] == query_verb()) ||
    // Tambien nos vale el mismo nombre pero sin acentos ni simbolos
    (skill_pieces[0] == SKILLS_TABLE->skill_translate(query_verb())) )
    {
      found = i;
      
      for (j = 1; (j < sizeof(skill_pieces)) && (found == -1); j++)
      {
        // No hay coincidencia
        if ((j - 1 < sizeof(str_pieces)) && 
            (skill_pieces[j] != str_pieces[j - 1]))
          found = -1;
      }
    }
  }

  if (found == -1)  
    return 0;
  /*
  {
      tell_object(find_living("folken"), "[SKILL] found="+found+", skill=<no encontrado>\n");
      return 0;
  }
  else
  {
      tell_object(find_living("folken"), "[SKILL] found="+found+", skill="+known_skills[found]+"\n");
  }
  */
  
  // Primero buscamos si hay una coincidencia exacta
  // if((i = member_array(query_verb(), my_skills)) != -1) 

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (this_object()->query_dead())
  {
    notify_fail(_LANG_SKILL_DEAD_USE);
    return 0;
  }    

  // llamamos a la funcion cast_effect de la dote, pasandole como parametros
  // el string (objetivos a buscar), el iniciador del efecto, y quiet como false
  result = (int)call_other( skill_list[known_skills[found]][0], 
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
  //     update_skill_used_times(known_skills[found]);
  
  return result;
}
  
int skill_damage(int damage, string type, object attacker)
{
  int prot;
  
  prot = this_object()->query_resistance(type);
      
  // Debug de informacion a los inmortales (Folken 7/01)
  if ((prot != 0) && this_object()->query_coder())
    tell_object(this_object(), "DEBUG (skill_damage): Aplicada resistencia " +
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
                                                                                
string help_skill(string str) 
{
  object table;
  string aux;

  table = load_object(SKILLS_TABLE);

  if (!table)
    return "";
  
  if (!pointerp(skill_list[str]))
  {
    aux = table->skill_translate(str);
    if (str != aux)
      str = aux;
    else
      return "";
  }

  return (string)skill_list[str][0]->help(str);
} 

mixed * stats() 
{
  return ({ 
    ({"Known skills", known_skills, }),
    ({"Skill list", skill_list, }),             
  });
}
