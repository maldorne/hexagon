
#include <common/properties.h>
#include <living/death.h>
#include <living/races.h>
#include <living/body-parts.h>
#include <translations/common.h>
#include <language.h>

inherit container "/lib/core/basic/container";
inherit equip     "/lib/living/equip";

#define STD_CORPSE_WEIGHT 1600
#define DECAY_TIME 60
#define RAISE_CHANCE 25
#define PC_RAISE_CHANCE 50

int wasplayer;
string owner, race_ob, race_name, filename;
// The name of whoever this was, kept rather than shown: the corpse reads as
// what the world saw of them.
string given_name;
// Who they were, for what is done with the body afterwards (its parts, an
// offering): race and base race (path with ".c" and id), level, real
// alignment, who killed them and when they died; and the parts already taken,
// part id -> how many.
string race_id, base_race_ob, base_race_id;
int dead_level, dead_align, died_at;
string killer_name, killer_file;
// the source an NPC was spawned from (its template), "" for anybody else
string npc_source;
mapping parts_taken;
static int decay;
static object original;

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
  given_name = "";
  race_name = "";
  race_ob = "";
  filename = "";
  race_id = "";
  base_race_ob = "";
  base_race_id = "";
  dead_level = 0;
  dead_align = 0;
  died_at = time();
  killer_name = "";
  killer_file = "";
  npc_source = "";
  parts_taken = ([ ]);
  decay = 10;
  wasplayer = 0;
  original = nil;

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
  // set_race_ob(DEFAULT_RACE_OB);
}

string query_given_name() { return given_name; }

void set_owner(string n, object ob) 
{
  owner = n;
  original = ob;

  if (ob)
    given_name = ob->query_given_name();
  
  set_name(_LANG_CORPSE_OF + n);
  set_short(capitalize(_LANG_CORPSE_OF) + capitalize(n));
  add_alias(_LANG_CORPSE_OF + capitalize(n));

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
       (ob->query_race_ob() != DEFAULT_RACE_OB))
    {
      set_race_ob(ob->query_race_ob());
      race_id = ob->query_race_id() ? ob->query_race_id() : "";
      base_race_ob = ob->query_base_race_ob() ? ob->query_base_race_ob() : "";
      base_race_id = ob->query_base_race_id() ? ob->query_base_race_id() : "";
      if (ob->query_base_race_name() != "")
        set_race_name(ob->query_base_race_name());
      else
        set_race_name(ob->query_race_name());
    }

    set_file_name(base_name(ob));
    npc_source = ob->query_npc_source() ? (string)ob->query_npc_source() : "";
    dead_level = ob->query_level();
    dead_align = ob->query_real_align();
  }

  if (ob && ob->query_weight()) 
    set_weight(ob->query_weight());
  else 
    set_weight(STD_CORPSE_WEIGHT);

  // Taniwha 1996, supress clean up if a player pegs out in a room
  if (ob && interactive(ob))
  {
    wasplayer = 1;
    environment(ob)->add_timed_property("no_clean_up", 1, 30000);
  }
}

/** Who killed them: kept by name and file, the killer may be gone soon. */
void set_killer(object killer)
{
  if (!killer)
    return;
  killer_name = killer->query_name() ? killer->query_name() : "";
  killer_file = base_name(killer);
}

/** The body starts to rot from the moment of death. */
void start_decay()
{
  died_at = time();
  set_heart_beat(DECAY_TIME);
}

string query_race_id() { return race_id; }
string query_base_race_ob() { return base_race_ob; }
string query_base_race_id() { return base_race_id; }
int query_dead_level() { return dead_level; }
string query_npc_source() { return npc_source; }
int query_dead_align() { return dead_align; }
int query_was_player() { return wasplayer; }
int query_died_at() { return died_at; }
string query_killer_name() { return killer_name; }
string query_killer_file() { return killer_file; }

/** How fresh the body is, 0-100: it falls a tenth with every stage of decay. */
int query_freshness()
{
  if (decay >= 10)
    return 100;
  if (decay <= 0)
    return 0;
  return decay * 10;
}

// --- Body parts ----------------------------------------------------------------

/** The parts a body of its race has (part id -> spec); none for an unknown race. */
mapping query_body_parts()
{
  object race;

  race = (race_ob && strlen(race_ob)) ? load_object(race_ob) : nil;
  return race ? race->query_body_parts() : ([ ]);
}

mapping query_parts_taken() { return ([ ]) + parts_taken; }
int query_part_taken(string id) { return parts_taken[id] ? parts_taken[id] : 0; }

/**
 * How many of a part are still on the body.
 *
 * @param id the part id
 * @return what its race has, less what was taken; 0 for a part it lacks
 */
int query_parts_left(string id)
{
  mapping spec;
  int count;

  spec = query_body_parts()[id];
  if (!spec)
    return 0;

  count = intp(spec[BODY_PART_COUNT]) && spec[BODY_PART_COUNT] > 0 ?
          spec[BODY_PART_COUNT] : 1;
  count -= query_part_taken(id);
  return count > 0 ? count : 0;
}

/** One more of a part is gone from the body, taken or ruined. */
void add_part_taken(string id)
{
  if (stringp(id))
    parts_taken[id] = query_part_taken(id) + 1;
}

/**
 * What the body is worth as an offering: its race's size, grown with the
 * level of the dead, cut by its decay and by every part taken from it.
 */
int query_offering_value()
{
  object race;
  int size, value, fresh, taken, share, i;
  mixed * counts;

  race = (race_ob && strlen(race_ob)) ? load_object(race_ob) : nil;
  size = race ? race->query_body_size() : 5;
  if (size <= 0)
    size = 1;

  value = BODY_LEVEL_WORTH(CORPSE_WORTH_PER_SIZE * size, dead_level);

  fresh = query_freshness();
  if (fresh < BODY_PART_ROTTEN_WORTH)
    fresh = BODY_PART_ROTTEN_WORTH;
  value = value * fresh / 100;

  taken = 0;
  counts = map_values(parts_taken);
  for (i = 0; i < sizeof(counts); i++)
    taken += (int)counts[i];

  share = 100 - taken * CORPSE_PART_TAKEN_COST;
  if (share < CORPSE_LEAST_WORTH)
    share = CORPSE_LEAST_WORTH;

  value = value * share / 100;
  return value > 0 ? value : 1;
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
    cow = clone_object(UNDEAD_OBS_PATH + "skeleton.c");
  else if (i <= 80)
    cow = clone_object(UNDEAD_OBS_PATH + "zombie.c");
  else if (i <= 90)
    cow = clone_object(UNDEAD_OBS_PATH + "wraith.c");
  else if (i <= 98)
    cow = clone_object(UNDEAD_OBS_PATH + "specter.c");
  else
    cow = clone_object(UNDEAD_OBS_PATH + "lich.c");

  if (owner)
  {
    cow->set_short(cow->query_short() + " " + _LANG_OF + " " + owner);
    
    if (original)
    {
      cow->set_gender(original->query_gender());
      cow->set_friend(original);
    }
  }
    
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
  
  // /* dump the equip */
  // if (wasplayer) 
  // {
  //   obs = all_inventory(this_object());
  //   for (i = 0; i < sizeof(obs); i++)
  //   {
  //     if (obs[i]) obs[i]->move(environment(this_object()));
  //   }
  // }
  
  // dest_me();
}

mixed * stats(){
  return container::stats() + equip::stats() + ({
    ({ "Race Ob", race_ob }),
    ({ "Race Id", race_id }),
    ({ "Base Race Ob", base_race_ob }),
    ({ "Dead Level", dead_level }),
    ({ "Npc Source", npc_source }),
    ({ "Dead Align", dead_align }),
    ({ "Was Player", wasplayer }),
    ({ "Killer", killer_name + " (" + killer_file + ")" }),
    ({ "Died At", ctime(died_at) }),
    ({ "Freshness (nosave)", query_freshness() }),
    ({ "Parts Taken", implode(map_indices(parts_taken), ", ") }),
  });
}

