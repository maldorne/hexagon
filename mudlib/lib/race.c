/*
 * Touched up by neverbot 3/2002 to remove drowning, swimming, werewolves, etc
 *
 * Adapted by neverbot for Cc, 4/2003
 * Removed a big part of the file, and another big part moved to /std/living/death.c
 * Added subraces (or cultures) for each 'generic' race, neverbot 6/2003
 * Subraces are now called lineages
 * Now the health and gp regeneration system relies on the races, neverbot 7/2003
 * 10/2026 - A race says the kind of body it has and the parts a corpse of it
 *           yields (see /lib/obj/death/body-parts.md)
 *
 */

#include <living/races.h>
#include <living/body-parts.h>
#include <living/skills.h>
#include <translations/races.h>

inherit "/lib/core/object";

static int min_light_limit;
static int max_light_limit;

// Added a standard for any race, neverbot 2/2002
int body_size; // 1 smaller - 5 human - 9 bigger
// New alignment system, neverbot 6/03
int ext_align;
// Number of arms
int limbs;
// The lineages this race divides into, by name
string * lineages;
// System to take a character to its starting zone
string init_room;
// What code compares this race by: English, never translated, one of the
// RACE_* ids. The name and the short are what players read.
string race_id;
// The kind of body, "" to follow from the race id; and what the race changes
// of that kind's parts: part id -> spec, or 0 for a part it lacks.
string body_kind;
mapping body_part_changes;

// New, to add races for NPCs (they are not playable)
// Any race in /obj/races becomes playable automatically unless it has this set to 0
int is_playable;

int query_is_race_ob() { return 1; }

void set_race_id(string id) { race_id = id; }
string query_race_id() { return race_id; }

void create() 
{
  reset_get();

  body_size = 5; // This is used as a standard 'human' size
  limbs = 2;
  is_playable = 1;
  lineages = ({ });
  init_room = "";
  race_id = "";
  body_kind = "";
  body_part_changes = ([ ]);
  // Neutral alignment by default
  ext_align = random(100);

  ::create();
  // set_name("criatura desconocida");
  // set_long("La raza estandar de "+mud_name()+", debes hacerte miembro "
  //  "de una nueva raza pronto, esta es muy aburrida.\n");
  // set_light_limits(LSTANDARDL, LSTANDARDH);
}

// Dummy for now, allows death curses etc.
// taniwha 1996
void on_death(object player, varargs object killer) { }
void on_kill(object player, object victim) { }

int query_playable() { return is_playable; }
void set_playable(int p)
{ 
  if (p > 0) 
    is_playable = 1; 
  else
    is_playable = 0; 
}

int set_light_limits(int lower, int upper)
{
  min_light_limit = lower;
  max_light_limit = upper;
  if (lower < L_B_L)
    min_light_limit = L_B_L;
  if (upper>L_B_H)
    max_light_limit = L_B_H;
  return 1;
}

int query_light_limith() { return max_light_limit; }
int query_light_limitl() { return min_light_limit; }

void set_body_size(int i) { body_size = i; }
int query_body_size() { return body_size; }

// How the race reads in a character's sheet. A race may override it.
string query_sheet_name() { return this_object()->query_short(); }

string * query_lineages(){ return lineages; }
void set_lineages(string * list){ lineages = list; }

string query_init_room(){ return init_room; }
void set_init_room(string where){ init_room = where; }

/* armor querying one... I forget what it is called */
int query_number_type(string type)
{
  if (type == "ring")
    return 2;
  return 1;
}

/* number of weapons allowed to be wielded */
/*
int query_number_wielded() {
  return limbs;
  // return 1;
}
*/

/* the number of hands the race has */
void set_limbs(int i) { limbs = i; }
int query_limbs() { return limbs; }

/* race description used on the player object...
 * There will be a race varible on the player object which allows
 * the race to keep needed information on it
 */
string query_desc(object ob) 
{ 
  return _LANG_RACES_UNKNOWN_DESC; 
}

/* when the player logs on this function is called.
 * it allows you to start up the player with whatever he needs.
 * EXCEPT it's not used, see start_player() below *curses*  
 */
/*
int player_start(object player) {
  return 1;
}
*/

/* and guess what this does?
 * Beware... If some is straight dested this will not be called,
 * So remember that you still must check your array for null members.
 */
int player_quit(object player)
{
  return 1;
}

int query_dark(int i)
{
  if (i < (min_light_limit - 5) ) return 1;  /* Can't see a thing */
  if (i <  min_light_limit) return 2;        /* can sorta see but not much */
  if (i < (min_light_limit + 5) ) return 3;  /* starting to lose it */
  if (i > (max_light_limit + 40) ) return 6; /* blinded by the light */
  if (i > (max_light_limit + 20) ) return 5; /* dazzled */
  if (i >  max_light_limit) return 4;        /* squinting */
  return 0;
}

int query_max_level(string guild) { return 100; }

// new alignment system, neverbot 6/03
int query_ext_align() { return ext_align; }
void set_ext_align(int i) { ext_align = i; }

void set_racial_bonuses(object ob) { }

// The words a living of this race answers to. A lineage answers both to its
// own name and to its base race's.
string * query_race_aliases()
{
  string * ret;
  string basepath;
  object base;

  ret = ({ lower_case((string)this_object()->query_name()) });

  // query_base_race lives on /lib/lineage.c, so a plain race answers nil
  basepath = this_object()->query_base_race();
  if (stringp(basepath) && strlen(basepath) &&
      (base = load_object(basepath)))
    ret |= ({ lower_case((string)base->query_name()) });

  return ret;
}

// Hook for what a race does to somebody the moment it is put on. Racial
// aliases are not handed out here: overrides of this do not chain.
void start_player(object ob)
{
}

void race_heartbeat(object player)
{
  // if (!player) return;
  // if (!environment(player)) return;
  // if (player->query_dead()) return;
}

// functions added to customize, neverbot 4/2003
string * query_initial_languages()
{
  return ({ });
}

int query_race_weight()
{
  return STD_WEIGHT;
}

// body location system based in an original idea by Iolo@Rl, changed and updated by
// neverbot for CcMud, 4/2003
// By default in any race the locations are of a humanoid body
string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_CHEST, 
    _LANG_RACES_LOCATIONS_ARM, _LANG_RACES_LOCATIONS_LEG,
    _LANG_RACES_LOCATIONS_HAND, _LANG_RACES_LOCATIONS_FOOT });

}

// returns enough info to hit in a body location
mixed obtain_location()
{
  float mult; // damage multiplier
  string name; // location name

  switch (random(30)) 
  {
    case 0:      mult = 2.0;  name = _LANG_RACES_LOCATIONS_HEAD;  break;
    case 1..4:   mult = 1.5;  name = _LANG_RACES_LOCATIONS_CHEST; break;
    case 5..14:  mult = 1.0;  name = _LANG_RACES_LOCATIONS_ARM;   break;
    case 15..24: mult = 1.0;  name = _LANG_RACES_LOCATIONS_LEG;   break;
    case 25..28: mult = 0.5;  name = _LANG_RACES_LOCATIONS_HAND;  break;
    case 29:     mult = 0.5;  name = _LANG_RACES_LOCATIONS_FOOT;  break;
  }

  // damage multiplier, location name, body type
  return ({ mult, name, _LANG_RACES_HUMANOID_BODY });
}

// Added by neverbot 4/03, to get rid of gender_table.c
string query_race_gender_string(object player, varargs int flag)
{
  if (!flag)
  {
    if (player->query_gender()==2)
       return "la esencia";
     return "el ente";
    }
    else{
    if (player->query_gender()==2)
       return "esencia";
     return "ente";
    }
}

/* New health/gp regeneration system
 *
 * These functions receive the player object and
 *  check whether or not they must regenerate hps!!
 * For races such as the drow it is possible to check if the
 *  player is underground, if it is day/night, etc
 * (by default gps and hps are always raised)
 */

int query_regen_hp(object player) { return 1; }
int query_regen_gp(object player) { return 1; }

// --- Body parts --------------------------------------------------------------

/** Sets the kind of body (BODY_* in <living/body-parts.h>). */
void set_body_kind(string kind) { body_kind = kind ? kind : ""; }

/**
 * The kind of body: the one set, or the one the race id stands for; any other
 * race is humanoid.
 */
string query_body_kind()
{
  if (body_kind && strlen(body_kind))
    return body_kind;
  if (!race_id)
    return BODY_HUMANOID;

  switch (race_id)
  {
    case RACE_ANIMAL:  return BODY_BEAST;
    case RACE_BIRD:    return BODY_BIRD;
    case RACE_CRITTER: return BODY_CRITTER;
    case RACE_INSECT:  return BODY_INSECT;
    case RACE_REPTILE: return BODY_REPTILE;
    case RACE_UNDEAD:  return BODY_UNDEAD;
    case RACE_UNKNOWN: return BODY_NONE;
  }
  return BODY_HUMANOID;
}

// One spec, as the defaults write it; a race adding a part of its own may
// build it the same way.
static mapping part_spec(string skill, int min, int weight, int value,
                          int count, int perishable)
{
  return ([ BODY_PART_SKILL : skill, BODY_PART_MIN : min,
            BODY_PART_WEIGHT : weight, BODY_PART_VALUE : value,
            BODY_PART_COUNT : count, BODY_PART_PERISHABLE : perishable ]);
}

/**
 * The parts every body of a kind has, before the race changes them.
 *
 * @param kind a BODY_* kind
 * @return part id -> spec
 */
mapping query_kind_body_parts(string kind)
{
  switch (kind)
  {
    case BODY_HUMANOID:
      return ([
        PART_HEAD  : part_spec(SKILL_DISMEMBERING, 20, 40, 30, 1, 1),
        PART_HAND  : part_spec(SKILL_DISMEMBERING, 10, 15, 10, 2, 1),
        PART_HEART : part_spec(SKILL_GUTTING,      20, 10, 25, 1, 1),
        PART_TEETH : part_spec(SKILL_PULLING,      10,  2,  5, 1, 0),
      ]);
    case BODY_BEAST:
      return ([
        PART_SKIN  : part_spec(SKILL_SKINNING,     10, 15, 20, 1, 0),
        PART_HEAD  : part_spec(SKILL_DISMEMBERING, 20, 30, 10, 1, 1),
        PART_HEART : part_spec(SKILL_GUTTING,      20,  8, 15, 1, 1),
        PART_FANGS : part_spec(SKILL_PULLING,      10,  2, 10, 1, 0),
        PART_CLAWS : part_spec(SKILL_PULLING,      10,  2,  5, 1, 0),
      ]);
    case BODY_BIRD:
      return ([
        PART_PLUMAGE : part_spec(SKILL_SKINNING,     20, 10, 15, 1, 0),
        PART_HEAD    : part_spec(SKILL_DISMEMBERING, 10,  5,  3, 1, 1),
        PART_HEART   : part_spec(SKILL_GUTTING,      20,  2,  5, 1, 1),
        PART_CLAWS   : part_spec(SKILL_PULLING,      10,  1,  5, 1, 0),
      ]);
    case BODY_REPTILE:
      return ([
        PART_SCALES : part_spec(SKILL_SKINNING,     30, 20, 25, 1, 0),
        PART_HEAD   : part_spec(SKILL_DISMEMBERING, 20, 20, 10, 1, 1),
        PART_HEART  : part_spec(SKILL_GUTTING,      20,  5, 10, 1, 1),
        PART_FANGS  : part_spec(SKILL_PULLING,      10,  2, 10, 1, 0),
      ]);
    case BODY_INSECT:
      return ([
        PART_MANDIBLES : part_spec(SKILL_PULLING, 20, 3, 10, 1, 0),
      ]);
    case BODY_CRITTER:
      return ([
        PART_SKIN : part_spec(SKILL_SKINNING,     5, 3, 2, 1, 0),
        PART_TAIL : part_spec(SKILL_DISMEMBERING, 5, 1, 1, 1, 1),
      ]);
    case BODY_UNDEAD:
      return ([
        PART_HEAD  : part_spec(SKILL_DISMEMBERING, 20, 30, 10, 1, 1),
        PART_TEETH : part_spec(SKILL_PULLING,      10,  2,  3, 1, 0),
      ]);
  }
  return ([ ]);
}

/** The parts the race starts from: those of its kind of body. */
mapping query_default_body_parts()
{
  return query_kind_body_parts(query_body_kind());
}

/**
 * Adds a part, or replaces the spec of one the race has.
 *
 * @param id a PART_* id, or a new one that names itself in the spec
 * @param spec the BODY_PART_* keys; a part that has one keeps the keys left out
 */
void add_body_part(string id, mapping spec)
{
  if (stringp(id) && mappingp(spec))
    body_part_changes[id] = spec;
}

/** The race lacks a part its kind of body has. */
void remove_body_part(string id)
{
  if (stringp(id))
    body_part_changes[id] = 0;
}

/**
 * The parts a body of this race has: the defaults with the race's changes.
 *
 * @return part id -> spec, a copy
 */
mapping query_body_parts()
{
  mapping parts;
  string * ids;
  int i;

  parts = ([ ]) + query_default_body_parts();
  ids = map_indices(body_part_changes);

  for (i = 0; i < sizeof(ids); i++)
  {
    if (!mappingp(body_part_changes[ids[i]]))
      parts[ids[i]] = nil;
    else if (mappingp(parts[ids[i]]))
      parts[ids[i]] = parts[ids[i]] + body_part_changes[ids[i]];
    else
      parts[ids[i]] = ([ ]) + body_part_changes[ids[i]];
  }

  return parts;
}

/** The spec of one part, nil when the race has none. */
mapping query_body_part(string id)
{
  return query_body_parts()[id];
}
