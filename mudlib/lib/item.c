/* Item.c
 * Made by Baldrick.
 * to be used instead of /std/object on *EVERY* object that can be carried.
 *
 * Right now it's only armour.c and weapon.c that inherits it.
 * and for now that's the only one needing it..
 * unless you have any wear / wieldable items.. the should also inherit this
 *
 * Modificado por neverbot@Cc, Abril/03, para generalizar e incluir aqui funciones
 *  de otros archivos (/std/basic/item_misc.c).
 *
 * Eliminado el /obj/handlers/item_handler.c, por cambiar la forma en que se
 *  comprueban los gremios, razas, deidades, etc del item, neverbot 4/03
 * Ahora el item tiene una lista de strings que se corresponden con los query_name
 *  de cada objeto del living (query_guild_name, query_race_name, etc), en lugar
 *  de funcionar mediante propiedades.
 *
 * Como personalizar objetos:
 *
 * Si necesitamos que sea para una raza, gremio, deidad determinada, etc,
 *  simplemente tenemos que hacerle un set_guild("nombre del gremio"), set_race, etc
 *  o genero con set_needed_gender(1 o 2).
 * Se puede hacer el objeto para un unico jugador mediante el
 *  add_property("player", nombre);
 * Se puede hacer el objeto de quest mediante add_property("property", propiedad)
 *  y solo lo podran utilizar los jugadores que tengan la property <propiedad>
 * Si se le pone la propiedad CURSED_PROP (ver properties.h), no se podra quitar o
 *  desempuñar, y dara el mensaje el mensaje devuelto por el
 *  objeto->query_property(CURSED_PROP). Se pueden poner una lista de dos mensajes en lugar
 *  de un solo string, y dara el segundo mensaje a la room donde esta el personaje.
 * Mismo funcionamiento con las propiedades "messon" y "messoff" para dar mensajes
 *  al equiparse y desequiparse.
 * Eliminadas las propiedades para subir caracteristicas, ac o thac0... lo dejaremos
 *  para la gente que sepa utilizar las funciones de bonos temporales de combate y
 *  similares en lugar de permitir que cualquier programador meta objetos poderosos.
 *
 * Añadida herencia de read_desc, antiguamente en todos los objetos del mud, ahora
 *  solo en items. neverbot 24/08/2008
 *
 * Añadidas resistencias magicas a los objetos mediante properties
 *    add_property( "tipo resistencia", porcentaje);
 *    Los tipos de resistencias están en /include/resistances.h
 *    neverbot 13/17/2009
 */

inherit obj       "/lib/core/object";
inherit alignment "/lib/core/basic/alignment";
inherit condition "/lib/core/basic/condition";
inherit read_desc "/lib/core/basic/read_desc";

// Incluido para que los items tengan el mismo numero de objetos que un
//  living (race, race_group, guild, etc).
#include <living/living.h>
#include <common/properties.h>
#include <basic/condition.h>
#include <living/social.h>
#include <basic/resistances.h>

int wearable;
int holdable;
int size;
int hands;  /* Hamlet */
int in_use;
int gender_needed;
string * list; // Lista de nombres (similar a la lista de objetos de un living).

void create()
{
  int i;
  holdable = wearable = 0;
  size = 0;                 // Fix by Wonderflug
  gender_needed = 0;
  hands = 1;

  list = allocate(NUM_SOCIAL_OBJECTS);
  for (i = 0; i<sizeof(list); i++)
    list[i] = NULL_SOCIAL_VALUE;

  alignment::create(); // neverbot
  condition::create();
  obj::create();
  read_desc::create();
}

void init()
{
  obj::init();
  read_desc::init();
}

void set_holdable(int i)
{
  holdable = i;
  if (wearable && holdable)
    wearable = 0;
}

void set_wearable(int i)
{
  wearable = i;
  if (wearable && holdable)
    holdable = 0;
}

int query_holdable() { return holdable; }
int query_wearable() { return wearable; }
int query_in_use() { return in_use; }

/* Sizes.. This will be used in wield and wear and later also by containers.
 * Will have it here and not in /std/object because the roomsize is something
 * else..
 * Baldrick aug '95.
 */
void set_size(int i)
{
    size = i;
} /* void size */

int adjust_size(int i)
{
    size += i;
    return size;
} /* adjust_size */

int query_size()
{
    return size;
}

/* Hamlet Sep 1995 */
void set_hands_needed(int i)
{
    hands = i;
}

int query_hands_needed() { return hands; }

// Radix, needed for long()
// string cond_string() { return(0); }

// Radix moved long() from /obj/weapon.c & armour.c to here.
// Also added calc_extra_look() cause it was unused (though inherited)
// Jan 18, 1996
string long(varargs string s, int dark)
{
  string cond, xtra;
  if ((this_object()->query_weapon()) ||
      (this_object()->query_armour()) ||
      (this_object()->query_shield()) )
  {
    cond = cond_string();
  }

  xtra = calc_extra_look();

  if (!cond)
    cond = "";
  if (!xtra)
    xtra = "";

  // return obj::long() + read_desc::long() + xtra + cond;

  return(
         sprintf("\n  %-*s\n", (this_user()?this_user()->query_cols()-2:77),
                 "   " + obj::long()) +
                 read_desc::long() + xtra + cond
        );
}

/* **************************
 *  Antiguo archivo /std/basic/item_misc.c añadido aqui, no se utilizaba
 *  en ningun otro lugar, neverbot 4/03
 * **************************
 */

// Move hack so that wearables/holdables will leave the wear/hold
// arrays of living creatures.

int move(mixed dest, varargs mixed messout, mixed messin)
{
  object ob;
  int i;
  int unused_it;

  unused_it = 0;  /* Hamlet's nastiness */

  /* Next line is Hamlet's and it's ugly, but needed */
  if (this_object()->query_in_use())
  {
    this_object()->set_in_use(0);
    unused_it = 1;
  }

  ob = environment(this_object());
  i = (int)::move(dest, messout, messin);

  if (!i && unused_it && objectp(ob) && living(ob))
  {
    if (this_object()->query_holdable())  /* Hamlet fixed me */
      ob->unhold_ob(this_object());
    else if (this_object()->query_wearable()) /* hamlet fixed me */
      ob->unwear_ob(this_object());
  }

  if (i && unused_it)
    this_object()->set_in_use(1); /* Hamlet */

  return i;
}

/* Added april '95 by Baldrick, does the same as the move, just for dest_me
 * Not sure if it will work tho
 */
void dest_me()
{
  object ob;

  ob = environment(this_object());

  if (in_use && objectp(ob) && living(ob))
    if (this_object()->query_holdable()) /* Hamlet fixed me */
      ob->unhold_ob(this_object());
    else if (this_object()->query_wearable()) /* Hamlet */
      ob->unwear_ob(this_object());
  ::dest_me();
} /* void dest_me */

// Major hack, Taniwha 1996, most of it is in HANDLER
// To clean up adding resistances on magic stuff, stat mods etc
// Eliminado el handler para realizar aqui las comprobaciones, neverbot 4/03
// (cambiado el funcionamiento general de gremios/razas/etc para objetos).
int set_in_use(int i)
{
  mixed str;
  int count;

  count = 0;
  in_use = i;

  // Taniwha, see if there is any cute stuff
  /*
  if ( (!(sizeof(map_prop) || sizeof(timed_prop) ||
         sizeof(query_static_properties())) &&
       (this_object()->query_align() == 0) )) return i;
  */
  // Comprobacion de alineamiento contrario en objetos, idea de Iolo@Rl
  if ( ((this_object()->query_align()<0) && (environment(this_object())->query_align()>100)) ||
        ((this_object()->query_align()>100) && (environment(this_object())->query_align()<0)) )
  {
    tell_object(environment(this_object()),"Sientes un extraño remordimiento al coger "+
               (this_object()->query_gender()?(this_object()->query_article()+" "):"tu")+
               this_object()->short()+". Te ves obligado a soltarlo.\n");
    return 0;
  }

  if (i)
  {
    // Comprobacion de raza
    if (list[RACE_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[RACE_OB] != environment(this_object())->query_race_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que seas de una raza "+
                    "concreta para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de clase
    if (list[CLASS_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[CLASS_OB] != environment(this_object())->query_class_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que seas de una clase "+
                    "concreta para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de gremio
    if (list[GUILD_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[GUILD_OB] != environment(this_object())->query_guild_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que seas de un gremio "+
                    "concreto para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de oficio
    if (list[JOB_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[JOB_OB] != environment(this_object())->query_job_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que seas de un oficio "+
                    "concreto para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de grupo racial
    if (list[RACEG_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[RACEG_OB] != environment(this_object())->query_race_group_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que seas de un grupo "+
                    "racial concreto para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de clan
    if (list[GROUP_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[GROUP_OB] != environment(this_object())->query_group_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que seas de un grupo "+
                    "concreto para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de deidad
    if (list[DEITY_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[DEITY_OB] != environment(this_object())->query_deity_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que adores a una deidad "+
                    "concreta para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion de ciudadania
    if (list[CITY_OB] != NULL_SOCIAL_VALUE)
    {
      if (list[CITY_OB] != environment(this_object())->query_city_name())
      {
        tell_object(environment(this_object()), "Este objeto requiere que tengas una ciudadanía "+
                    "concreta para que puedas usarlo.\n");
        return 0;
      }
    }

    // Comprobacion del genero de quien se lo pone
    if (gender_needed > 0)
    {
      if (gender_needed != environment(this_object())->query_gender() )
      {
        tell_object(environment(this_object()), "Este objeto esta diseñado para alguien "+
                     "de otro sexo.\n");
        return 0;
      }
    }

    // Comprobaciones extraidas del antiguo item_handler.c
    // individual player
    if (this_object()->query_property("player") &&
       (environment(this_object())->query_name() != this_object()->query_property("player")))
    {
      tell_object(environment(this_object()),"No puedes usar "+(string)this_object()->short()+
         ", ya que ha sido diseñado específicamente para otra persona.\n");
      return 0;
    }

    // quest item, tagged when got
    if (this_object()->query_property("property") &&
       (!environment(this_object())->query_property(this_object()->query_property("property"))))
    {
      tell_object(environment(this_object()), "No puedes utilizar "+(string)this_object()->short()+
           ", ya que no te has ganado el derecho a usarlo.\n");
      return 0;
    }

    str = this_object()->query_property("messon");

    if (stringp(str))
      tell_object(environment(this_object()), str +".\n"); // single string, tell wearer
    else if (pointerp(str)) // array, wearer + tell_room
    {
      tell_object(environment(this_object()),str[0]+".\n");
      if (sizeof(str) > 1 && environment(environment(this_object())))
      {
          tell_room(environment(environment(this_object())),str[1]+".\n",environment(this_object()));
      }
    }

    for (count = 0; count < sizeof(RESISTANCES); count++)
    {
      if (query_property(RESISTANCES[count]))
         environment(this_object())->adjust_tmp_resistance(RESISTANCES[count],
                                         query_property(RESISTANCES[count]) );
    }

  } // Fin si nos lo estamos poniendo
  else  // unwearing
  {
    str = this_object()->query_property(CURSED_PROP);

    if (str)
    {
      if (stringp(str))
        tell_object(environment(this_object()),str+".\n");
      else if (pointerp(str))
      {
        tell_object(environment(this_object()),str[0]+".\n");
        if (sizeof(str) >1  && environment(environment(this_object())))
          tell_room(environment(environment(this_object())), str[1]+".\n", environment(this_object()));
      }

      tell_object(environment(this_object()),"Lo siento, "+
           (string)this_object()->short()+ " está maldito.\n");
      return 1;
    }

    str = this_object()->query_property("messoff");

    if (stringp(str))
       tell_object(environment(this_object()),str+".\n");
    else if (pointerp(str))
    {
      tell_object(environment(this_object()),str[0]+".\n");
      if (sizeof(str) > 1 && environment(environment(this_object())))
      {
        tell_room(environment(environment(this_object())),str[1]+".\n",environment(this_object()));
      }
    }

    for (count = 0; count < sizeof(RESISTANCES); count++)
    {
      if (query_property(RESISTANCES[count]))
        environment(this_object())->adjust_tmp_resistance(RESISTANCES[count],
                                            -(query_property(RESISTANCES[count])) );
    }
  }

  // catch(in_use = HANDLER->local_in_use(i, environment(this_object()), this_object()));
  return in_use;
} /* set_in_use() */

// Funciones de personalizacion de objetos
int query_gender_needed(){ return gender_needed; }
void set_gender_needed(int i){ gender_needed = i; }

void set_class(string str){ list[CLASS_OB] = str; }
void set_guild(string str){ list[GUILD_OB] = str; }
void set_race(string str){ list[RACE_OB] = str; }
void set_race_group(string str){ list[RACEG_OB] = str; }
void set_group(string str){ list[GROUP_OB] = str; }
void set_deity(string str){ list[DEITY_OB] = str; }
void set_city(string str){ list[CITY_OB] = str; }
void set_job(string str){ list[JOB_OB] = str; }

mapping query_auto_load_attributes()
{
  return ([ 
      "::" : ::query_auto_load_attributes() 
    ]) + 
    ((cond != MAX_COND) ? ([ "condition" : cond ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["condition"]))
    cond = attribute_map["condition"];
  else
    cond = MAX_COND;

  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
} 

// Funcion stats() añadida
mixed stats()
{
  mixed * ret;

  ret = obj::stats() +
    alignment::stats() +
    condition::stats() +
    read_desc::stats();

  ret += ({
     ({ "Wearable", wearable, }),
     ({ "Holdable", holdable, }),
     ({ "Size", size, }),
     ({ "Hands", hands, }),
     ({ "In Use", in_use, }),
     ({ "Gender Needed", gender_needed, }),
     ({ "Data List", list, }),
         });
  return ret;
}
