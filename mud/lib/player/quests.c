/* ************************************************************
 * 
 *   Sistema de Quests para CcMud, neverbot 03/09
 *
 *   quests: Cada player almacena un mapping del tipo
 *   ([ id = path del objeto a conseguir/matar :
 *      ({
 *       int = tipo de quest (1 get, 2 kill, ...)
 *       int = cuantos hay que conseguir,
 *       string = nombre de la quest,
 *       string = path del questman original,
 *       int = cuantos llevo,
 *       extra_info = lista con informacion extra que pueda ser util
 *      })
 *   ])
 *
 *   quests_done: tambien se almacena un historico de las quests 
 *    finalizadas, para no dejar repetir aquellas que ya esten
 *    hechas.
 *   ([ id = path del questman : ({ lista }) ])
 *    donde lista contiene los paths del objeto a conseguir
 *    en cada quest de ese questman
 *
 * *********************************************************** */

#include <common/quests.h>

mapping quests;
mapping quests_done;

// Para debug
mapping query_quests() { return quests; }
mapping query_quests_done() { return quests_done; }
int add_quest(string object_path, int quest_type, int how_many, 
  string quest_name, string questman_path, varargs mixed extra_info);
  
void create()
{
  quests = ([ ]);
  quests_done = ([ ]);
}

int is_doing_quest_from_name(string object_name, varargs int quest_type)
{
  if (quest_type)
  {
    if ((member_array("@"+object_name, map_indices(quests)) != -1) &&
        (quests["@"+object_name][QUEST_TYPE] == quest_type) )
      return 1;
  }
  else
  {
    if (member_array("@"+object_name, map_indices(quests)) != -1)
      return 1;   
  }
  return 0;  
}

int is_doing_quest(string object_path, varargs int quest_type)
{
  if (quest_type)
  {
    if ((member_array(object_path, map_indices(quests)) != -1) &&
       (quests[object_path][QUEST_TYPE] == quest_type) )
      return 1;
  }
  else
  {
    if (member_array(object_path, map_indices(quests)) != -1)
      return 1;   
  }
  return 0;
}

// Cuantos objetos de una quest llevamos en el inventario
//  (incluye containers)
int check_num_quest_objects(object ob)
{
  object * obs;
  int i;
  int howmany;
  string str;
  
  howmany = 0;
  obs = ({ });

  if (!ob)
    return 0;
  
  // No esta el objeto 
  if (undefinedp(quests[base_name(ob)]) && undefinedp(quests["@"+ob->query_short()]))
    return 0;
  
  obs = deep_inventory(this_object());
  
  str = base_name(ob);
  
  // Si esta almacenado por el path
  if (quests[str])
  {
    for ( i = 0; i < sizeof(obs); i++)
      if (base_name(obs[i]) == str)
        howmany++;
  }
  // Esta almacenado por el short
  else
  {
    str = ob->query_short();
    for ( i = 0; i < sizeof(obs); i++)
      if (obs[i]->query_short() == str)
        howmany++;
  }

  return howmany;
}

int check_total_quest_objects()
{
  string * names;
  object * obs;
  int i;
  int howmany;
  
  howmany = 0;
  names = map_indices(quests);
  obs = deep_inventory(this_object());

  for (i = 0; i < sizeof(obs); i++)
    if ( (member_array(base_name(obs[i]), names) != -1) ||
       (member_array("@"+obs[i]->query_short(), names) != -1) )
       howmany++;

  // tell_object(find_living("folken"), "check_total_quest_objects: "+howmany+"\n");

  return howmany;
}

int fix_num_quest_objects()
{
  string * names;
  object * obs;
  int * count;
  int i, which, previous;
  int total;
  string ret;

  names = map_indices(quests);
  obs = deep_inventory(this_object());
  count = allocate(sizeof(names)); // inicializa a ceros
  total = 0;

  for (i = 0; i < sizeof(obs); i++)
  {
    if ((which = member_array(base_name(obs[i]), names)) != -1)
    {
      count[which]++;
      total++;
    }
    else if ((which = member_array("@"+obs[i]->query_short(), names)) != -1)
    {
      count[which]++;
      total++;
    }
  }
  
  // Tras comprobar cuantos objetos tenemos de cada quest, 
  // ajustamos el estado de las quests
  for (i = 0; i < sizeof(names); i++)
  {
    // Solo para quests de conseguir objetos
    if (quests[names[i]][QUEST_TYPE] != TYPE_GET)
      continue;
    
    previous = quests[names[i]][HOWMANY_TOTAL];
    
    // Asignamos el nuevo valor
    if (count[i] > quests[names[i]][HOWMANY])
      // Si es el maximo
      quests[names[i]][HOWMANY_TOTAL] = quests[names[i]][HOWMANY];
    else if (count[i] <= 0)
      // Si no tenemos ninguno (estamos quitando objetos)
      quests[names[i]][HOWMANY_TOTAL] = 0;    
    else 
      quests[names[i]][HOWMANY_TOTAL] = count[i];
      
    // Si ha cambiado el total, informamos al player
    if ((previous != quests[names[i]][HOWMANY_TOTAL]) && !this_object()->query_dead())
    {
      ret = "" + quests[names[i]][QUEST_NAME] + " (";
      if (quests[names[i]][HOWMANY_TOTAL] == quests[names[i]][HOWMANY])
        ret += "%^RED%^terminada%^WHITE%^";
      else
        ret += "" + quests[names[i]][HOWMANY_TOTAL] + " de " + quests[names[i]][HOWMANY];
      ret += ").\n";
      tell_player(this_object(), ret);
    }      
  }

  // tell_object(find_living("folken"), "fix_num_quest_objects: "+total+"\n");

  return total;
}

int doing_quest(object ob)
{
  return (is_doing_quest(base_name(ob)) || is_doing_quest_from_name(ob->query_short()));
}

// Incrementa el numero de objetivos conseguidos en una quest
int update_quest_from_name(string object_name, varargs int howmany)
{
  string ret;

  object_name = "@"+object_name;
  
  // Si no tenemos la quest
  if (!quests[object_name])
    return 0;
    
  if (howmany)
  {
    if ((quests[object_name][HOWMANY_TOTAL] <= 0) && (howmany <= 0))
      return 0;

    quests[object_name][HOWMANY_TOTAL] += howmany;
  }
  else
    ++quests[object_name][HOWMANY_TOTAL];
  
  // Si ya estan todos
  if (quests[object_name][HOWMANY_TOTAL] > quests[object_name][HOWMANY])
  {
    quests[object_name][HOWMANY_TOTAL] = quests[object_name][HOWMANY];
    return quests[object_name][HOWMANY_TOTAL];
  }

  // Si no tenemos ninguno (estamos quitando objetos)
  if (quests[object_name][HOWMANY_TOTAL] <= 0)
    quests[object_name][HOWMANY_TOTAL] = 0;
  
  // Informamos al player
  if (!this_object()->query_dead())
  {
    ret = "" + quests[object_name][QUEST_NAME] + " (";
    if (quests[object_name][HOWMANY_TOTAL] == quests[object_name][HOWMANY])
      ret += "%^RED%^terminada%^WHITE%^";
    else
      ret += "" + quests[object_name][HOWMANY_TOTAL] + " de " + quests[object_name][HOWMANY];
    ret += ").\n";
    tell_player(this_object(), ret);
  }

  return quests[object_name][HOWMANY_TOTAL];
}

// Incrementa el numero de objetivos conseguidos en una quest
int update_quest(string object_path, varargs int howmany)
{
  string ret;
  
  // Si no tenemos la quest
  if (!quests[object_path])
    return 0;
    
  if (howmany)
  {
    if ((quests[object_path][HOWMANY_TOTAL] <= 0) && (howmany <= 0))
      return 0;

    quests[object_path][HOWMANY_TOTAL] += howmany;
  }
  else
    ++quests[object_path][HOWMANY_TOTAL];
  
  // Si ya estan todos
  if (quests[object_path][HOWMANY_TOTAL] > quests[object_path][HOWMANY])
  {
    quests[object_path][HOWMANY_TOTAL] = quests[object_path][HOWMANY];
    return quests[object_path][HOWMANY_TOTAL];
  }

  // Si no tenemos ninguno (estamos quitando objetos)
  if (quests[object_path][HOWMANY_TOTAL] <= 0)
    quests[object_path][HOWMANY_TOTAL] = 0;
  
  // Informamos al player
  if (!this_object()->query_dead())
  {
    ret = "" + quests[object_path][QUEST_NAME] + " (";
    if (quests[object_path][HOWMANY_TOTAL] == quests[object_path][HOWMANY])
      ret += "%^RED%^terminada%^WHITE%^";
    else
      ret += "" + quests[object_path][HOWMANY_TOTAL] + " de " + quests[object_path][HOWMANY];
    ret += ").\n";
    tell_player(this_object(), ret);
  }

  return quests[object_path][HOWMANY_TOTAL];
}

int up_quest(object ob, varargs int howmany)
{
  if (!howmany)
    howmany = 1;

  if (is_doing_quest(base_name(ob)))
    update_quest(base_name(ob), howmany);
  else if (is_doing_quest_from_name(ob->query_short())) 
    update_quest_from_name(ob->query_short(), howmany);  
}

int set_quest_objects(object ob, int howmany)
{
  string str;
  string ret;
  int previous;

  previous = 0;

  if (!ob)
    return 0;
    
  // Si no tenemos la quest
  if (undefinedp(quests[base_name(ob)]) && 
    undefinedp(quests["@" + ob->query_short()]))
    return 0;
    
  if (quests[base_name(ob)])
    str = base_name(ob);
  else
    str = "@" + ob->query_short();

  previous = quests[str][HOWMANY_TOTAL];

  quests[str][HOWMANY_TOTAL] = howmany;

  // Si es el maximo
  if (howmany > quests[str][HOWMANY])
    quests[str][HOWMANY_TOTAL] = quests[str][HOWMANY];

  // Si no tenemos ninguno (estamos quitando objetos)
  if (howmany <= 0)
    quests[str][HOWMANY_TOTAL] = 0;
  
  // Damos mensaje
  if (previous != quests[str][HOWMANY_TOTAL])
  {
    if (!this_object()->query_dead())
    {
      ret = "" + quests[str][QUEST_NAME] + " (";
      if (quests[str][HOWMANY_TOTAL] == quests[str][HOWMANY])
        ret += "%^RED%^terminada%^WHITE%^";
      else
        ret += "" + quests[str][HOWMANY_TOTAL] + " de " + quests[str][HOWMANY];
      ret += ").\n";
      tell_player(this_object(), ret);
    }
  }

  return quests[str][HOWMANY_TOTAL];  
}
  
int add_quest(string object_path, int quest_type, int how_many, 
  string quest_name, string questman_path, varargs mixed extra_info)
{
  if (is_doing_quest(object_path))
    return 0;
  
  if (map_sizeof(quests) >= MAX_QUESTS)
  {
    tell_object(this_object(), "Tu registro de misiones está lleno, no puedes aceptar ninguna más.\n");
      return 0;
  }

  if (!extra_info)
    quests[object_path] = ({ quest_type, how_many, quest_name, questman_path, 0, ({ }), });
  else
    quests[object_path] = ({ quest_type, how_many, quest_name, questman_path, 0, extra_info, });
  
  // Para comprobar si desde el primer momento ya cumplimos las condiciones de una quest
  // de conseguir objetos (ya los tenemos en el inventario)
  call_out("fix_num_quest_objects", 1);
  
  return 1;
}

int remove_quest(string object_path)
{
  // Si no tenemos la quest
  if (!quests[object_path])
    return 0;

  quests = map_delete(quests, object_path);
  return 1;
}

void reset_quests() 
{ 
  // Esto parece no funcionar, hay que hacerlo por las bravas
  // quests = ([ ]); 
  int i;
  string * ids;

  ids = map_indices(quests);
  
  for(i = 0; i < sizeof(ids); i++)
    quests = map_delete(quests, ids[i]);
}

// Call this, you will die!!
void reset_quests_done() 
{ 
  int i;
  string * ids; 

  ids = map_indices(quests_done);
  
  for(i = 0; i < sizeof(ids); i++)
    quests_done = map_delete(quests_done, ids[i]);
}

// Si ha terminado _alguna_ quest
int has_finished_quests()
{
  int i;
  string * ids; 

  ids = map_indices(quests);
  
  for(i = 0; i < sizeof(ids); i++)
  {
    // Si ya estan todos los objetos de alguna quest
    if (quests[ids[i]][HOWMANY_TOTAL] >= quests[ids[i]][HOWMANY])
      return 1;    
  }
  
  return 0;
}

// Si ha terminado una quest en concreto
int has_finished_quest(string object_path, string questman_path)
{
  // Si no tenemos la quest
  if (!quests[object_path])
    return 0;
  
  // Comprobacion: por si distintos questman piden conseguir mismos objetos
  if (quests[object_path][QUESTMAN_PATH] != questman_path)
    return 0;
  
  if (quests[object_path][HOWMANY_TOTAL] >= quests[object_path][HOWMANY])
    return 1;
    
  return 0;
}

// Ha hecho con anterioridad esta quest
int has_done_quest(string object_path, string questman_path)
{
  // Si no hemos hecho quests con ese questman
  if (!quests_done[questman_path])
    return 0;
  
  // La quest esta en la lista
  if (member_array(object_path, quests_done[questman_path]) != -1)
    return 1;

  return 0;
}

// Pasamos de terminada a hecha (finished -> done)
int finish_quest(string object_path, string questman_path)
{
  if (!has_finished_quest(object_path, questman_path))
    return 0;
  
  // Añadimos la quest a la lista de terminadas
  if (!quests_done[questman_path])
    quests_done[questman_path] = ({ object_path, });
  else
    quests_done[questman_path] += ({ object_path, });
    
  // Borramos la quest
  quests = map_delete(quests, object_path);
}

mixed * stats()
{
  return ({
    ({ "Quests", quests, }), 
    ({ "Quests Done", quests_done, }), 
      });
}
