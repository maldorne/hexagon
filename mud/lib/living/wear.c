/* Taniwha was here 09/94
 * extended the code to allow more types
 * so clothing like shirts can be worn with armour
 *
 * Wear.c used for wearable objects.. or rather for players to be able
 * to wear armour and other stuff. 
 * Will make it possible for items to be worn even if they're not armour.
 * And also make armour heldable, not only wearable.
 * It is completely rewritten by Baldrick, april '94 
 * (With a lot help from Aragorn)
 *
 * we will have different types of worn objects.
 * Body armour, Body clothing, boots/shoes and head will be one only.
 * Rings will be multipple.  Maybe add an amount_heads ? :=)
 * It won't be any problem to add new types later.
 *
 * Añadido bonificador maximo por destreza segun la armadura.
 * Añadido penalizador a las habilidades segun la armadura.
 */

#include <common/properties.h>
#include <user/player.h>
#include <item/armour.h>

#define MAXTYPES 20

private mixed *worn_objects;
private int *armour_types;
private mapping worn_ac;
private int max_dex_bon; 
private int skill_malus;

int query_skill_malus(){ return skill_malus; }

int unwear_ob(object ob);
int do_unwear_ob(object on);
int wear_ob(object ob);
int do_wear_ob(object ob);
void reset_types();
object *query_worn_ob(varargs int type);
int query_max_dex_bon(){ return max_dex_bon; }
int set_max_dex_bon(int i){ return max_dex_bon = i; }
mapping query_total_worn_ac(){ return worn_ac; }

int query_worn_ac(string tipo)
{
  if (member_array(tipo, keys(worn_ac)) != -1)
    return worn_ac[tipo];
  return 0;
}

void create()
{
  int i;
  worn_ac = ([ ]);
  worn_objects = ({ });
  // Un numero negativo representa que no hay maximo
  max_dex_bon = -1;
  reset_types();

  for(i = 0; i < sizeof(AC_TYPES); i++)
  {
     worn_ac[AC_TYPES[i]] = 0;
  }
}

void wear_commands() 
{
  add_action("do_wear",   "ponerse");
  add_action("do_wear",   "ponerme");
  add_action("do_wear",   "vestir");
  add_action("do_unwear", "desvestir");
  add_action("do_unwear", "quitarse");
  add_action("do_unwear", "quitarme");
}

object * query_worn_ob(varargs int type)
{ 
  int i;
  object * ret; 

  ret = ({ }); 
  
  // Si esta fuera de rango no devuelvo nada.
  if (type < -1 || type > 14) 
    return ({ });

  // Si no especifico tipo devuelvo todos.
  if (!type) 
    return worn_objects;

  if (type == -1) 
    type = 0; // para poder ver los objetos sin tipo

  for (i = 0; i < sizeof(worn_objects); i++)
     if (worn_objects[i] && worn_objects[i]->query_armour_type()==type)
        ret += ({ worn_objects[i] });

  return ret;
}

void reset_types()
{
  int e;
  armour_types = allocate (MAXTYPES);

  for (e = 0; e < MAXTYPES; e++)
    armour_types[e] = 0;
}

int unwear_ob(object ob)
{
  if (!ob) 
    return 0;

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }      

  if (member_array(ob, worn_objects) == -1) 
  {
    notify_fail("No puedes quitarte algo que no lleves puesto.\n");
    return 0;
  }

  tell_object(this_object(), "Empiezas a quitarte tu "+ ob->short() + ".\n");

  return do_unwear_ob(ob);
} 

int do_unwear_ob(object ob)
{
  if (!ob) 
    return 0;

  if (ob->set_in_use(0) == 0)
  {
    worn_objects -= ({ ob });
    armour_types[ob->query_armour_type()]--;
    tell_object(this_object(),"Te quitas tu "+ob->query_short()+".\n");

    // Nuevo sistema de AC, Folken 06/03
    worn_ac[AC_TYPES[0]] -= (ob->query_ac() + ob->query_slashing_bon());
    worn_ac[AC_TYPES[1]] -= (ob->query_ac() + ob->query_blunt_bon());
    worn_ac[AC_TYPES[2]] -= (ob->query_ac() + ob->query_piercing_bon());

    // Actualizamos el bonificador maximo de destreza
    if (ob->query_max_dex_bon() >= 0)
      this_object()->recalc_max_dex_bon();

    // Actualizamos el penalizador a las habilidades
    skill_malus -= ob->query_skill_malus();
    // Siempre negativo o inexistente
    if (skill_malus > 0)
      skill_malus = 0;

    if (ob->query_pieces())
    {
      string main_piece;
      object main_object;

      main_piece = ob->query_main_piece();
      main_object = nil;
      
      // Buscamos el objeto principal del conjunto
      if (base_name(ob) == main_piece)
        main_object = ob; // La pieza que nos estamos quitando es la principal
      else
      {
        // La pieza principal es otra distinta a la que nos estamos quitando, 
        // la buscamos entre el equipo que llevamos puesto
        int i;
        for (i = 0; i < sizeof(worn_objects); i++)
        {
          // tell_object(find_living("folken"), "-- buscando main: " + file_name(worn_objects[i]) + "\n");
          
          if (base_name(worn_objects[i]) == main_piece)
          {
            main_object = worn_objects[i];
            break;
          }
        }
      }
      
      // Si la pieza principal del conjunto existe y la llevamos puesta
      // (y solo si tenemos puesto el conjunto completo)
      if (main_object && sizeof(main_object->query_piece_object_list()))
      {
        // tell_object(find_living("folken"), "-- main es: " + file_name(main_object) + "\n");
      
        main_object->set_piece_object_list( 0 );

        tell_object(this_object(), "%^BOLD%^Dejas de llevar el conjunto completo de: " + 
              main_object->query_piece_set_name() + ".%^RESET%^\n");

        main_object->complete_set_off(this_object());
      }
    }
  }
  else 
    tell_object(this_object(),"No puedes quitarte tu "+ob->query_short()+".\n");

  return 1;
}

int wear_ob(object ob)
{
  int a_type;
  // string *exp;
  string thisone;
  int aux, i;
  mixed local;
  
  /* Maybe this check is/can be another place ? */
  /* Made a query_holdable in living.c.. */
  if (this_object()->query_property(LOADING_PROP))
  {
    notify_fail("Tu equipamiento todavía está cargándose, te es imposible "+
                "ponértelo.\n");
    return 0;
  }
  
  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP) && (query_verb()!= "equipar"))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  // Sistema de localizaciones de Iolo, adaptado para Cc, Folken 4/03
  local = this_object()->obtain_location();
  if (local && local[2] && 
      (local[2] != ob->query_body_type())) {
    tell_object(this_object(),"Este equipo no se ajusta bien a tu cuerpo, "+
        "ha sido diseñado para otro tipo de persona.\n");
    return 1;
  }

  if (this_object()->query_guild_ob() && 
     !this_object()->query_guild_ob()->
      query_legal_armour(ob->query_armour_name()))
  {
    tell_object(this_object(),"Este objeto no se corresponde con "+
      "los permitidos en tu gremio.\n");
    return 1;
  }

  notify_fail("Ya tienes puesto un objeto de las mismas características.\n");
  
  thisone = base_name(ob);
  
  // Only one copy of each object (i.e. can't wear the same ring in each hand)
  for (i = 0; i < sizeof(worn_objects); i++)
  {
    if (thisone == base_name(worn_objects[i]))
        return 0;
  }
  
  // Max 4 amulet/necklace (type 5)
  // Max 2 rings, so in case they are magic, only one type of magic per hand (type 7)
  // Max two pendants (type 12)
  // Max 4 tied objects (bagpack, quiver, etc) (type 13)
  a_type = (int)ob->query_armour_type();
  aux = 1;
  switch(a_type)
  {
    case 5:  if (armour_types[a_type] >= 4) aux = 0; break;
    case 7:  if (armour_types[a_type] >= 2) aux = 0; break;
    case 12: if (armour_types[a_type] >= 2) aux = 0; break;
    case 13: if (armour_types[a_type] >= 4) aux = 0; break;
    default: if (armour_types[a_type])      aux = 0; break;
  }

  if (!aux)
      return 0;

  if (!(aux = ob->query_size()))
  {
    notify_fail("Este objeto tiene un fallo (no tiene tamaño), pide ayuda a un programador.\n");
    return 0;
  }
  
  if (ob->set_in_use(1))
  {
    worn_objects += ({ ob });
    armour_types[a_type]++;
    // aux = aux / 5;
    tell_object(this_object(), "Comienzas a ponerte tu "+ob->short()+".\n");
    
    // Nuevo sistema de AC, Folken 06/03
    worn_ac[AC_TYPES[0]] += ob->query_ac() + ob->query_slashing_bon();
    worn_ac[AC_TYPES[1]] += ob->query_ac() + ob->query_blunt_bon();
    worn_ac[AC_TYPES[2]] += ob->query_ac() + ob->query_piercing_bon();

    // Actualizamos el bonificador maximo de destreza
    if (max_dex_bon < 0)
       max_dex_bon = ob->query_max_dex_bon();
    else
       if ((ob->query_max_dex_bon() != -1) && (ob->query_max_dex_bon() < max_dex_bon))
         max_dex_bon = ob->query_max_dex_bon();
    
    // Actualizamos el penalizador a las habilidades
    skill_malus += ob->query_skill_malus();
    // Siempre negativo o inexistente
    if (skill_malus > 0)
      skill_malus = 0;

  // Sistema de equipo por piezas
  // Este trozo de codigo me parece de todo menos optimo, incluye recorrer dos listas (cortas, eso si)
  // y se puede mejorar. Lo dejo como trabajo futuro ya que es algo que se va a ejecutar en pocas ocasiones
  // (cada vez que un personaje se pone una prenda que forme parte de un conjunto)
  if (ob->query_pieces())
  {
    object main;
    object * set;
    string * pieces;
    string * wearing;
    int found;
    int position, central_piece;
    
    main = nil;
    set = ({ });
    pieces = ({ });
    wearing = ({ });
    found = 0;

    if (ob->query_main_piece() && (main = load_object(ob->query_main_piece())) )
    {
      // Guardamos los nombres de archivo de los objetos que llevamos equipados
      for (i = 0; i < sizeof(worn_objects); i++)
      {
        wearing += ({ base_name(worn_objects[i]) });
        // tell_object(find_living("folken"), "-- llevando: " + base_name(worn_objects[i]) + "\n");
      }
      
      pieces = main->query_piece_list();

      for (i = 0; i < sizeof(pieces); i++)
      {
        // tell_object(find_living("folken"), "-- comparando: " + pieces[i] + "\n");
        
        if ((position = member_array(pieces[i], wearing)) != -1)
        {
          // Sumamos cuantas piezas de este conjunto llevamos
          found += 1;
          // tell_object(find_living("folken"), "-- añadimos al set: " + file_name(worn_objects[position]) + "\n");
          
          // Guardamos los objetos reales (no nombres de archivo) que forman el conjunto
          set += ({ worn_objects[position] });
          
          // Apuntamos en que posicion de la lista 'set' esta la pieza central del equipo
          if (worn_objects[position]->is_main_piece())
            central_piece = sizeof(set) - 1;
        }
        else
          // Nos falta una, dejamos de buscar el resto de piezas de equipo
          break;
      }
      
      if ( (found > 0) && (found == sizeof(pieces)) )
      {
        set[central_piece]->set_piece_object_list(set);
        
        tell_object(this_object(), "%^BOLD%^Te pones el conjunto completo de: " + 
              main->query_piece_set_name() + ".%^RESET%^\n");
        
        set[central_piece]->complete_set_on(this_object());
      }
    }   
  }

    // Solo players
    if (this_object()->query_player())
        // En el primer minuto de conexion no cuenta
        if (time() - this_object()->query_last_log_on() > 60)
            if (this_object()->query_time_remaining(PASSED_OUT_PROP) < aux/6)
                this_object()->add_timed_property(PASSED_OUT_PROP, "Todavía estás "+
                  "intentando ponerte tu "+ob->short()+".\n", aux/6);
    return 1; 
  }
  
  notify_fail("No puedes ponerte tu "+ob->short()+".\n");
  return 0; 

} /* wear_ob */

int do_wear(string woo)
{
  object *boo;
  int i;
  int size;
  
  if (!strlen(woo))
  {
    notify_fail("¿"+capitalize(query_verb()) + " el qué?\n");
    return 0;
  }

  // AAArrrggghhhh find_match() doesn't work... -Aragorn
  // boo = find_match(woo, this_object());
  boo = all_inventory(this_object());
  
  for (i = 0; i < sizeof(boo); i++)
    if (!boo[i]->id(woo)) 
      boo[i] = nil;
    
  boo -= ({ 0 });

  if (!sizeof(boo))
  {
    notify_fail("No llevas eso en tu inventario.\n");
    return 0;
  }

  // Somewhat nasty but does the trick... -Aragorn
  /* what does this do ? Baldy.. 
   * It gives me an error message here, but not in hold.c */
  boo -= (mixed *)this_object()->query_worn_ob();

  if (sizeof(boo))
  {
    if (!boo[0]->query_wearable())
    {
      notify_fail("¡Este objeto no se puede vestir!\n");
      return 0;
    }

    if (boo[0]->query_in_use())
    {
      notify_fail("Ya lo llevas puesto.\n");
      return 0;
    }
  
    if (size = boo[0]->query_size() == 0)
    {
      notify_fail("Ese objeto no tiene tamaño definido, díselo a alguien que pueda arreglarlo.\n");
      return 0;
    }

    size = size / 2;
    if (size < 1)
      size = 1;

    return wear_ob(boo[0]);
  }
  return 1;
}

int do_unwear(string woo)
{
  object *boo;
  int i;

  if (!strlen(woo))
  {
    notify_fail("¿"+capitalize(query_verb()) + " el qué?\n");
    return 0;
  }

  // AAArrrggghhhh find_match() doesn't work... -Aragorn
  // boo = find_match(woo, this_object());
  boo = all_inventory(this_object());
  
  for (i = 0; i < sizeof(boo); i++)
    if (!boo[i]->id(woo)) 
      boo[i] = nil;
    
  boo -= ({ 0 });

  // As nasty as it gets but I'm just trying to get it to work 
  // right now. Speedups later.
  // -Aragorn
  boo -= (all_inventory(this_object())-(mixed*)this_object()->query_worn_ob());

  if (!sizeof(boo))
  {
    notify_fail("No llevas eso en tu inventario.\n"); 
    return 0;
  }

   // At the moment you can only unwear one thing at the time. -Aragorn
   return unwear_ob(boo[0]);
}

mixed stats() 
{
  return ({ 
      ({"Worn Objects (nosave)", worn_objects, }),
      ({"Armour Types (nosave)", armour_types, }),
      ({"Worn Ac (nosave)", worn_ac, }),
      ({"Max Dex Bon (nosave)", max_dex_bon, }),
      ({"Skill Malus (nosave)", skill_malus, }),
        });
}
