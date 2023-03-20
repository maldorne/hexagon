
#include <common/properties.h>
#include <translations/common.h>
#include <language.h>

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

// prototypes
void decay();

int avoid_auto_load() { return 1; }
int query_no_save_object() { return 1; }

void set_race_ob(string race_ob);
void set_race_name(string str) { race_name = str; }
void set_file_name(string f) { filename = f; }
string query_race_name() { return race_name; }
string query_race() { return race_name; }
string query_file_name() { return filename; }

void create()
{
  container::create();
  equip::create();
  owner = _LANG_CORPSE_SOMEBODY;
  race_name = ""; // "unknown";
  race_ob = ""; // "/lib/obj/races/unknown";
  filename = "";
  decay = 10;
  wasplayer = 0;

  set_gender(1); // for spanish language
}

// dest_me added to solve multiple problems:
// - If the corpse disappears by itself, it moves all its inventory to the env,
//   but if it is buried only its dest_me is called without other calculations
// - We check the rest of the interior inventory in case we have to do some special
//   operation (unique objects, for example)
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

int query_corpse() { return 1; }
string query_owner() { return owner; }
void set_race_ob(string s) { race_ob = s; }
string query_race_ob() { return race_ob; }

// Converted from call_outs to heartbeats by Baldrick. after a look at Aragorns
// code on RD sept '93
void heart_beat()
{
  tell_object(find_living("admin"), "Heartbeat in "+query_short()+"\n");
  decay();
}

string query_name() 
{
  if (!::query_name())
    return _LANG_CORPSE_SOMEBODY;
  return ::query_name();
}

void setup() 
{
  set_short(capitalize(_LANG_CORPSE_NAME));
  add_alias(_LANG_CORPSE_ALIASES);
  set_main_plural(capitalize(pluralize(_LANG_CORPSE_NAME)));
  add_plural(_LANG_CORPSE_PLURALS);
  set_long(_LANG_CORPSE_DESC);

  set_weight(STD_CORPSE_WEIGHT);
  // set_race_ob("/lib/obj/races/unknown");
}

void set_owner(string n, object ob) 
{
  owner = n;
  
  set_name(_LANG_CORPSE_OF + n);
  set_short(capitalize(_LANG_CORPSE_OF) + n);
  if (ob && ob->query_main_plural())
  {
    set_main_plural(capitalize(_LANG_CORPSES_OF) + ob->query_main_plural());
    add_plural(_LANG_CORPSES_OF + ob->query_main_plural());
  }
  else
  {
    set_main_plural(capitalize(_LANG_CORPSES_OF) + pluralize(n));
    add_plural(_LANG_CORPSES_OF + pluralize(n));
  }

  set_long(_LANG_CORPSE_DESC_OF);
  
  if (ob)
  {
    if (ob->query_race_ob() && 
       (ob->query_race_ob() != "/lib/obj/races/unknown"))
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
  if (ob && interactive(ob))
  {
    wasplayer = 1;
    environment(ob)->add_timed_property("no_clean_up", 1, 30000);
  }
}

// Next function is Hamlet's. Decides whether undead are allowed
// in the room
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

  for (i = 0; i < maxi; i++)
    if (no_undead_dir[i] == roomfile[0..(strlen(no_undead_dir[i]))])
    {
      corpse_fine = 0;
      break;
    }
  */
  /* no omiqs, neverbot 6/03
  if ("/global/omiq.c"->flag_in_progress())
    corpse_fine = 0;
  */
  if (room->query_property(NO_UNDEAD))
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
    cow->set_short(cow->query_short() + " " + _LANG_OF + " " + owner);
    
  stuff = all_inventory(ob);
    
  for (i = 0; i < sizeof(stuff); i++)
    if (stuff[i])
      stuff[i]->move(cow);

  cow->move(environment(ob));
  cow->init_equip();
  tell_room(environment(ob), _LANG_CORPSE_RAISE_MSG);

  ob->dest_me();
  return;
}

void decay() 
{
  object *obs;
  int i;
  decay -= 1;

  tell_object(find_living("admin"), "Decay in "+query_short()+" " + decay + "\n");

  
  if (!strlen(race_name))
  {
    if (race_ob && (race_ob != ""))
      race_name = (string)race_ob->query_name();
    else race_name = _LANG_CORPSE_UNKNOWN_BEIGN;
  }
  
  if (decay == 7)
  {
    set_short(capitalize(_LANG_CORPSE_REMAINS));
    add_alias(_LANG_CORPSE_REMAINS);
    set_main_plural(capitalize(_LANG_CORPSE_REMAINS));
    add_plural(_LANG_CORPSE_REMAINS);
  }
  else if (decay == 5) 
  {
    set_short(capitalize(_LANG_CORPSE_ROTTEN_REMAINS));
    add_alias(_LANG_CORPSE_ROTTEN_REMAINS);
    set_main_plural(capitalize(_LANG_CORPSE_ROTTEN_REMAINS));
    add_plural(_LANG_CORPSE_ROTTEN_REMAINS);

    set_long(_LANG_CORPSE_REMAINS_DESC);
  }
  else if (decay > 0)
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
  
  dest_me();
}

mixed * stats(){
  return container::stats() + equip::stats();
}

