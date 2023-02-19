
#include <common/properties.h>

inherit container "/lib/core/basic/container";
inherit equip     "/lib/living/equip";

#define STD_CORPSE_WEIGHT 1600
#define DECAY_TIME 60
#define RAISE_CHANCE 25
#define PC_RAISE_CHANCE 50
#define PATH "/lib/obj/"

int wasplayer;
string owner, race_ob, race_name, filename;
int decay;

int avoid_auto_load() { return 1; }
int query_no_save_object() { return 1; }

void set_race_ob(string race_ob);
void set_race_name(string str) { race_name = str; }
void set_file_name(string f) { filename = f; }
string query_race_name() { return race_name; }
string query_race() { return race_name; }
string query_file_name() { return filename; }

string query_article() { return "el"; }
string query_numeral() { return "un"; }
string query_vowel() { return "o"; }

void create()
{
  container::create();
  equip::create();
  owner = "alguien";
  race_name = ""; // "desconocida";
  race_ob = ""; // "/obj/races/desconocida";
  filename = "";
  decay = 10;
  wasplayer = 0;
}

mixed * stats(){
  return container::stats() + equip::stats();
}

int query_corpse() { return 1; }
string query_owner() { return owner; }

/* Next function is Hamlet's.  Decides whether undead are allowed
   in the room
*/
int undead_okay(object room) 
{
  int corpse_fine;
  corpse_fine = 1;

  if (!room->query_room())
    return 0;

  /* Next section excellent but considered unnecessary, Taniwha 1995 */
  /*
    string roomfile = virtual_file_name(room);
    maxi = sizeof(no_undead_dir);

    for (i=0;i<maxi;i++)
      if ( no_undead_dir[i] == roomfile[0..(strlen(no_undead_dir[i]))] ) {
        corpse_fine = 0;
        break;
      }
    */
   /* Sin omiqs, Folken 6/03
   if ("/global/omiq.c"->flag_in_progress())
      corpse_fine = 0;
   */
  if ( room->query_property(NO_UNDEAD) )
    corpse_fine = 0;

  return corpse_fine;
}

void raise_me(object ob)
{
  object *stuff, cow;
  int i;

  i = random(100);
  if (i <= 50)
    cow = clone_object(PATH + "death/skeleton.c");
  else
  if (i <= 80)
    cow = clone_object(PATH + "death/zombie.c");
  else
  if (i <= 90)
    cow = clone_object(PATH + "death/wraith.c");
  else
  if (i <= 98)
    cow = clone_object(PATH + "death/specter.c");
  else
    cow = clone_object(PATH + "death/lich.c");

  if (owner)
    cow->set_short(cow->query_short()+" de "+owner);
    
  stuff = all_inventory(ob);
    
  for (i = 0; i < sizeof(stuff); i++)
    if (stuff[i])
      stuff[i]->move(cow);

  cow->move(environment(ob));
  tell_room(environment(ob),"Escuchas el aullido de un espíritu torturado.\n"+
    "De repente, un "+
    cow->query_name()+" se levanta de entre los muertos.\n");
  cow->init_equip();
  ob->dest_me();
  return;
}

string query_name() 
{
  if (!::query_name())
    return "alguien";
  return ::query_name();
}

void setup() 
{
  // bits_gone = ({ });
  add_plural("cuerpos");
  add_alias("cuerpo");
  add_alias("cadaver");
  add_alias("cadáver");
  set_short("Cuerpo");
  set_long("Es un cadáver.\n");
  set_weight(1300);
  // set_race_ob("/obj/races/desconocida");
  set_main_plural("Cuerpos");
  add_plural("cuerpos");
  add_plural("cadaveres");
  add_plural("cadáveres");
}

void set_owner(string n, object ob) 
{
  owner = n;
  set_name("cuerpo de " + n);
  add_alias("cuerpo");
  add_alias("cuerpo de " + n);
  set_short("Cuerpo de " + capitalize(n));
  if ( ob && ob->query_main_plural() )
    set_main_plural("Cuerpos de "+ob->query_main_plural());
  else
    set_main_plural("Cuerpos de "+pluralize(n));
  add_plural("cuerpos");
  set_long("Este es el cuerpo muerto de "+capitalize(n)+".\n");
  
  if (ob)
  {
    if (ob->query_race_ob() && (ob->query_race_ob() != "/obj/races/desconocida"))
    {
      set_race_ob(ob->query_race_ob());
      if (ob->query_base_race_name() != "")
        set_race_name(ob->query_base_race_name());
      else
        set_race_name(ob->query_race_name());
    }
    set_file_name(base_name(ob));
  }

  if (ob && ob->query_weight()) 
    set_weight(ob->query_weight());
  else 
    set_weight(STD_CORPSE_WEIGHT);
  
  set_heart_beat(DECAY_TIME);

  // Taniwha 1996, supress clean up if a player pegs out in a room
  if (ob)
  {
    if (interactive(ob))
    {
      wasplayer = 1;
      environment(ob)->add_timed_property("no_clean_up", 1, 30000);
    }
  }
}

void decay() 
{
  object *obs;
  int i;
  decay -= 1;
  if (!race_name || (race_name == ""))
  {
    if (race_ob && (race_ob != ""))
      race_name = (string)race_ob->query_name();
    else race_name = "ser desconocido";
  }
  if (decay == 7)
  {
    set_short("Restos de "+query_name());
    set_main_plural("Restos de "+query_name());
    add_alias("restos");
    add_plural("restos");
  }
  if (decay == 5) 
  {
    set_short("Restos putrefactos de un "+race_name);
    set_main_plural("Restos putrefactos de un "+race_name);
    set_long("Este es el cuerpo muerto de un "+race_name+".\n");
          add_alias("restos");
          add_plural("restos");
  }
  if (decay > 0)
    return ;
    
  /* Hamlet visited the next line */
  if ( ((!wasplayer && (random(100) <= RAISE_CHANCE)) ||
        (wasplayer && (random(100) <= PC_RAISE_CHANCE)))
      && undead_okay(environment(this_object())) )
  {
    raise_me(this_object());
    return;
  }
  /* dump the equip */
  if (wasplayer) 
  {
    obs = all_inventory(this_object());
    for (i = 0; i < sizeof(obs); i++)
    {
      if (obs[i]) obs[i]->move(environment(this_object()));
    }
  }
  // destruct(this_object());
  this_object()->dest_me();
}

// Dest_me añadido para solucionar varios problemas:
// - Si el cuerpo desaparece por su propia cuenta, mueve todo su inventario al env,
//   pero si es enterrado solo se llama a su dest_me sin otros calculos
//   (no tenemos que preocuparnos por su destruccion, eso se hace mas arriba en el arbol
//   de herencias) (global/basic/misc.c creo)
// - Comprobamos el resto de inventario interior por si hay que hacer alguna operacion
//   especial (objetos unicos, por ejemplo)
void dest_me()
{
  int i;
  object * obs;
  obs = all_inventory(this_object());

  for (i = 0; i < sizeof(obs); i++)
  {
    if (obs[i]->query_unique_object()) 
    {
      obs[i]->dest_me();
    }
  }

  ::dest_me();
}

void set_race_ob(string s)
{
  race_ob = s;
}

string query_race_ob()
{
  return race_ob;
}

string query_possessive() { return "su"; }

/* Converted from call_outs to heartbeats by Baldrick. after a look at Aragorns
   code on RD sept '93 */
void heart_beat()
{
    decay();
} /* void heartbeat */
