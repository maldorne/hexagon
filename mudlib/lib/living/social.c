/*
 * By Radix - 1996 for FRMud
 * race_ob       - Race
 * guild_ob      - Guild
 * group_ob      - Clans (Special Interest Groups)
 * race_group_ob - Racial Groups (Dwarf Clans, Orc Tribes, Drow Houses, ...)
 * New CcMud:
 * job_ob        - Profession
 * deity_ob      - Religion
 * city_ob       - Citizenship
 * class_ob      - Class (generic guild)
 *
 * The file is no longer guild-race.c but has been renamed to groups_obs.c 
 * (I don't know who came up with putting a hyphen in a file name, as it makes things 
 * like guild-race::function() impossible. neverbot@Cc, 23/4/2003)
 *
 * Modified the old guild_joined to keep track of all guilds the player has belonged to.
 * neverbot 18/7/03
 *
 * Extracted everything related to the old commands (gr_commands) to create 
 * a new system of feats (feats.c), neverbot 11/10/08
 *
 * Added a new system of specialties for each of the social groups.
 * neverbot 20/10/2012
 *
 * Renamed as social.c, neverbot 10/2016
 */


#include <user/player.h>
#include <living/living.h>
#include <living/social.h>
#include <living/races.h>
#include <mud/secure.h>
#include <language.h>

inherit skills "/lib/living/skills";
inherit specs "/lib/living/specs";

// guild_joined[guild_ob] = ({ nivel, 1º vez alistado, ultima vez abandonado, })
mapping guild_joined;
mapping job_joined;

int class_level;
int guild_level;
// the played age, in seconds, at which this living last joined a guild
int guild_join_age;

// we have too many global variables, so I put all the xx_ob in a single array
// neverbot 3/2002
string * social_object_list;

// lifetime totals of the worship (DEITY_TOT_* keys in <deities.h>), saved with
// the living; the deities handler only keeps a devotion that decays
mapping deity_totals;
// string race_ob, guild_ob;
// string group_ob, race_group_ob;
// string job_ob, deity_ob;

// jobs
int total_job_xp, job_xp;
int job_level;

void create()
{
  social_object_list = allocate(NUM_SOCIAL_OBJECTS);
  deity_totals = ([ ]);
  guild_joined = ([ ]);
  job_joined = ([ ]);
  guild_level = 0;
  class_level = 0;
  job_level = 0;
  job_xp = 0;
  total_job_xp = 0;

  skills::create();
  specs::create();
}

string * query_social_object_list() { return social_object_list; }
void set_social_object_list(string * list)
{
  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  social_object_list = list;
}

// A refused social object path, written down for the coders. Nobody is told
// on the spot: whoever this_player() is may only have walked into the room
// that built this living.
private void _log_social_path(string str, string setter, string why)
{
  log_file("social_paths", "[" + ctime(time(), 4) + "] " + setter + "(\"" +
    str + "\") on " + file_name(this_object()) + " (" +
    this_object()->query_cap_name() + "): " + why + "\n");
}

// A social object is named by its full path, ".c" included: any other form
// is a bug in whoever passes it, so it is reported and refused.
private int _full_social_path(string str, string setter)
{
  if (strlen(str) > 2 && str[strlen(str) - 2 ..] == ".c")
    return TRUE;

  _log_social_path(str, setter, "not a path ending in \".c\"");
  return FALSE;
}

// Whether a path lies in the shared directory of a social object or in the
// same directory of the game the living belongs to.
private int _in_social_dirs(string str, string lib_dir, string game_subdir)
{
  string * dirs;
  int i;

  dirs = ({ lib_dir, game_root(this_object()) + game_subdir });

  for (i = 0; i < sizeof(dirs); i++)
    if (extract(str, 0, strlen(dirs[i]) - 1) == dirs[i])
      return TRUE;

  return FALSE;
}

void social_commands()
{
  skills_commands();
  specs_commands();
}

void start_player()
{
  if (social_object_list[GUILD_OB])
    catch(social_object_list[GUILD_OB]->start_player(this_object()));
  if (social_object_list[RACE_OB])
    catch(social_object_list[RACE_OB]->start_player(this_object()));
  if (social_object_list[GROUP_OB])
    catch(social_object_list[GROUP_OB]->start_player(this_object()));
  if (social_object_list[RACEG_OB])
    catch(social_object_list[RACEG_OB]->start_player(this_object()));
  if (social_object_list[JOB_OB])
    catch(social_object_list[JOB_OB]->start_player(this_object()));
  if (social_object_list[DEITY_OB])
    catch(social_object_list[DEITY_OB]->start_player(this_object()));
  if (social_object_list[CITY_OB])
    catch(social_object_list[CITY_OB]->start_player(this_object()));
  if (social_object_list[CLASS_OB])
    catch(social_object_list[CLASS_OB]->start_player(this_object()));

  // A body that wears a race knows its tongue and has one of them selected.
  if (social_object_list[RACE_OB])
  {
    this_object()->add_languages(
      social_object_list[RACE_OB]->query_initial_languages());

    if (!strlen(this_object()->query_current_language()) &&
        sizeof(this_object()->query_languages()))
      this_object()->set_language(this_object()->query_languages()[0]);
  }

  skills::start_skills();
}

// Re-assert the racial aliases on a living that already wears its race.
// Aliases are static, so anything rebuilt rather than created comes back
// without them.
void start_race()
{
  string * aliases;
  int i;

  if (!social_object_list[RACE_OB])
    return;

  // A body wears the placeholder race until it is given a real one; drop its
  // word unless that is genuinely the race.
  if (social_object_list[RACE_OB] != DEFAULT_RACE_OB)
  {
    aliases = DEFAULT_RACE_OB->query_race_aliases();
    for (i = 0; i < sizeof(aliases); i++)
      this_object()->remove_alias(aliases[i]);
  }

  aliases = social_object_list[RACE_OB]->query_race_aliases();
  for (i = 0; i < sizeof(aliases); i++)
    this_object()->add_alias(aliases[i]);
}

void set_race_ob(string str)
{
  string * old_aliases;
  int i;

  if (undefinedp(str) || !strlen(str))
    str = DEFAULT_RACE_OB;

  // for any object, the valid directories are the common one, and the specific for its game
  if (!_in_social_dirs(str, DEFAULT_RACE_DIR, "obj/races/"))
  {
    _log_social_path(str, "set_race_ob", "not in a races directory");
    return;
  }

  if (!_full_social_path(str, "set_race_ob"))
    return;

  if (file_size(str) < 0)
  {
    tell_object(this_object(),"El intento de set_race_ob no ha funcionado. "+
      "Díselo a alguien que pueda arreglarlo.\n");
    return;
  }

  // Taniwha 1997, stop these accumulating on race change
  this_object()->adjust_bonus_str(-this_object()->query_bonus_str());
  this_object()->adjust_bonus_con(-this_object()->query_bonus_con());
  this_object()->adjust_bonus_dex(-this_object()->query_bonus_dex());
  this_object()->adjust_bonus_cha(-this_object()->query_bonus_cha());
  this_object()->adjust_bonus_wis(-this_object()->query_bonus_wis());
  this_object()->adjust_bonus_int(-this_object()->query_bonus_int());
  this_object()->adjust_bonus_per(-this_object()->query_bonus_per());
  this_object()->adjust_bonus_wil(-this_object()->query_bonus_wil());

  // Remove the old language if they have one. Flode - 150997
  if (social_object_list[RACE_OB])
  {
    this_object()->remove_languages(
      social_object_list[RACE_OB]->query_initial_languages(), 1);
      // Asignamos uno de los lenguajes actuales, neverbot 02/2006
      if (sizeof(this_object()->query_languages()) > 0)
        this_object()->set_language(this_object()->query_languages()[0]);

      this_object()->adjust_ext_align(-social_object_list[RACE_OB]->query_ext_align());

      // the outgoing race's words, which the race itself lists
      old_aliases = social_object_list[RACE_OB]->query_race_aliases();
      for (i = 0; i < sizeof(old_aliases); i++)
        this_object()->remove_alias(old_aliases[i]);
  }

  social_object_list[RACE_OB] = str;
  this_object()->add_languages(social_object_list[RACE_OB]->query_initial_languages());
  // Asignamos uno de los lenguajes (el primero), neverbot 02/2006
  if (sizeof(social_object_list[RACE_OB]->query_initial_languages()) > 0)
    this_object()->set_language(social_object_list[RACE_OB]->query_initial_languages()[0]);

  this_object()->set_weight(social_object_list[RACE_OB]->query_race_weight());
  social_object_list[RACE_OB]->set_racial_bonuses(this_object());

  // Racial aliases go on here rather than in the race's start_player, because
  // overrides of that hook do not chain.
  // with lineages this would add an alias such as "Humano (Velan)"
  // this_object()->add_alias(lower_case(this_object()->query_race()));
  start_race();

  // if (this_object()->query_player())
  //  this_object()->add_alias("*"+lower_case(this_object()->query_race())+"*");

  // neverbot, nuevo sistema de alinamiento
  this_object()->adjust_ext_align(social_object_list[RACE_OB]->query_ext_align());

  this_object()->reset_hands();

  social_object_list[RACE_OB]->start_player(this_object());

} /* set_race_ob() */

void set_race(string str) { set_race_ob(DEFAULT_RACE_DIR + str + ".c"); }


string query_race_ob() { return social_object_list[RACE_OB]; }

// Nuevo sistema de subrazas, neverbot 6/03
// The name of the base race of a lineage, or "" for a race that is none
string query_base_race_name()
{
  object ob;
  if (social_object_list[RACE_OB])
    if (ob = load_object(social_object_list[RACE_OB]->query_base_race()))
        return (string)ob->query_name();
  return "";
}

// The id of the base race of a lineage, or "" for a race that is none
string query_base_race_id()
{
  object ob;
  if (social_object_list[RACE_OB])
    if (ob = load_object(social_object_list[RACE_OB]->query_base_race()))
        return (string)ob->query_race_id();
  return "";
}

string query_base_race_ob()
{
  string str;
  if (social_object_list[RACE_OB])
    if (str = social_object_list[RACE_OB]->query_base_race())
        return str;
  return "";
}

// Cambio: Ahora el query_race_name devuelve el name de la raza
// y query_race devuelve su short!! neverbot 6/03
string query_race_name()
{
  if (social_object_list[RACE_OB])
    return ((string)social_object_list[RACE_OB]->query_name());
  else
    return "Sin Raza";
}

// The id of the race, for code to compare: never translated
string query_race_id()
{
  if (social_object_list[RACE_OB])
    return (string)social_object_list[RACE_OB]->query_race_id();
  return "";
}

string query_race()
{ /* should return the name of our race */
  if (social_object_list[RACE_OB])
    return ((string)social_object_list[RACE_OB]->query_short());
  else
    return "Sin Raza";
} /* query_race() */

// neverbot 6/03
int query_body_size()
{
  if (social_object_list[RACE_OB])
    return (int)social_object_list[RACE_OB]->query_body_size();
  else
    return 5; // Tamaño estandar (humano)
} /* query_body_size() */

// *****************************************
//  Nuevo objeto para la Clase, neverbot 6/03
// *****************************************

void set_class_ob(string str)
{
  if (!_full_social_path(str, "set_class_ob"))
    return;

  if (!load_object(str))
  {
    social_object_list[CLASS_OB] = nil;
    return;
  }

  social_object_list[CLASS_OB] = str;
  this_object()->set_xp(0);
  this_object()->set_extreme_str(0);
  class_level = 1;
}

void set_class(string str)
{
  set_class_ob("/lib/obj/classes/" + str + ".c");
}

string query_class_ob() { return social_object_list[CLASS_OB]; }

string query_class_name()
{
  if (social_object_list[CLASS_OB])
    return ((string)social_object_list[CLASS_OB]->query_name());
  else
    return ("Sin Clase");
}

// The id of the class, for code to compare: never translated
string query_class_id()
{
  if (social_object_list[CLASS_OB])
    return (string)social_object_list[CLASS_OB]->query_class_id();
  return "";
}

string query_class()
{
  if (social_object_list[CLASS_OB])
    return ((string)social_object_list[CLASS_OB]->query_short());
  else
    return ("Sin Clase");
}

// *****************************************

// guild_joined[guild_ob] = ({ nivel, 1º vez alistado, ultima vez abandonado, })

mapping query_guild_joined(){ return guild_joined; }

void set_guild_joined(mapping map)
{
  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  guild_joined = map;
}

// The seconds this living has been played, or 0 for one that does not count
// them (only a player does: query_time_on counts back from now).
private int _played_age()
{
  mixed time_on;

  time_on = this_object()->query_time_on();
  return intp(time_on) ? -time_on : 0;
}

void set_guild_ob(string str)
{
  mixed * values;
  string old_guild;
  int old_level;

  old_guild = social_object_list[GUILD_OB];
  old_level = this_object()->query_guild_level();

  // Guardamos los datos del guild que abandonamos
  if (old_guild && load_object(old_guild))
  {
    // Actualizamos los datos del guild anterior
    values = guild_joined[old_guild];
    guild_joined[old_guild] = ({ old_level,
                                (sizeof(values)?values[1]:time()),
                                 time(),
                              });
  }

  // Abandonar un guild
  if (!strlen(str))
  {
    // Eliminamos la especializacion si la tuviera
    this_object()->reset_guild_spec();

    social_object_list[GUILD_OB] = nil;
    guild_level = 0;
    return;
  }

  if (!_full_social_path(str, "set_guild_ob"))
    return;

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;

  // Asignamos el gremio nuevo
  social_object_list[GUILD_OB] = str;
  // when it was joined, in played time, which starts the wait before the next
  guild_join_age = _played_age();
  // this_object()->set_xp(0); // NO!, la experiencia de clase y gremio es la misma

  // Si es uno en el que no hemos estado nunca
  if (undefinedp(guild_joined[str]))
  {
    guild_level = 1;
    guild_joined[str] = ({ 1, time(), -1, });
  }
  // Volvemos a alistarnos en un gremio anterior
  else
  {
    values = guild_joined[str];
    guild_level = values[0];
  }

  // Eliminamos la especializacion si la tuviera
  this_object()->reset_guild_spec();

  // Inicializacion del player
  // Esto solo ocurre la primera vez
  social_object_list[GUILD_OB]->join_player(this_object());
  // Esto cada vez que conectemos
  social_object_list[GUILD_OB]->start_player(this_object());

} /* set_guild_ob() */

int query_guild_join_age() { return guild_join_age; }

// The seconds of play left before this living may join a guild again: joining
// one starts a wait of GUILD_JOIN_WAIT. Nothing to wait for somebody who never
// joined one, nor for a living that does not count its play (an NPC).
int query_guild_join_wait()
{
  int age, wait;

  if (!map_sizeof(guild_joined))
    return 0;

  age = _played_age();
  if (age <= 0)
    return 0;

  wait = guild_join_age + GUILD_JOIN_WAIT - age;
  return wait > 0 ? wait : 0;
}

void set_guild(string str)
{
  set_guild_ob("/lib/obj/guilds/" + str + ".c");
}

string query_guild_ob() { return social_object_list[GUILD_OB]; }

string query_guild_name()
{
  if (social_object_list[GUILD_OB])
    return ((string)social_object_list[GUILD_OB]->query_name());
  else
    return ("Sin Gremio");
}

string query_guild()
{
  if (social_object_list[GUILD_OB])
    return ((string)social_object_list[GUILD_OB]->query_short());
  else
    return ("Sin Gremio");
}

// This is for groups such as Blades of Balance, Silver Fists, Hellfire Cult.
// These groups should be highly elite and interacial/interguild.  Should
// have a special focus or meaning in their organization. - Radix 1996
void set_group_ob(string str)
{
  // string tmp;
  if (!strlen(str))
  {
    social_object_list[GROUP_OB] = nil;
    return;
  }

  // if (sscanf(str,"/%s", tmp) == 1)
  //   str = extract(str,1);

  if (!_in_social_dirs(str, "/lib/obj/groups/", "obj/groups/"))
  {
    write("Illegal path for set_group_ob.\n");
    return;
  }

  if (!_full_social_path(str, "set_group_ob"))
    return;

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;

  social_object_list[GROUP_OB] = str;
  social_object_list[GROUP_OB]->start_player(this_object());
  return;
} /* set_group_ob() */

//string query_group_ob() { return group_ob; }
string query_group_ob()
{
  if (social_object_list[GROUP_OB])
    if (file_size(social_object_list[GROUP_OB]) < 1)
      social_object_list[GROUP_OB] = nil;
  return social_object_list[GROUP_OB];
}

string query_group_name()
{
  if (social_object_list[GROUP_OB])
    return ((string)social_object_list[GROUP_OB]->query_short());
  else
    return ("Sin Grupo");
}

// This is for Dwarf clans, Drow houses, Orc tribes...
// Radix 1996
void set_race_group_ob(string str)
{
  // string tmp;
  if (!strlen(str))
  {
    social_object_list[RACEG_OB] = nil;
    return;
  }

  // if (sscanf(str,"/%s", tmp) == 1)
  //   str = extract(str,1);

  if (!_in_social_dirs(str, "/lib/obj/race_groups/", "obj/race_groups/"))
  {
    write("Illegal path in set_race_group_ob.\n");
    return;
  }

  if (!_full_social_path(str, "set_race_group_ob"))
    return;

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;

  social_object_list[RACEG_OB] = str;
  social_object_list[RACEG_OB]->start_player(this_object());
  return;
}

string query_race_group_ob() { return social_object_list[RACEG_OB]; }

string query_race_group_name()
{
  if (social_object_list[RACEG_OB])
    return ((string)social_object_list[RACEG_OB]->query_short());
  else
    return ("Sin Grupo Racial");
}

// Sistema de localizaciones de Iolo para Rl, adaptado para Cc, neverbot 4/03
mixed query_locations()
{
  if (social_object_list[RACE_OB])
    return social_object_list[RACE_OB]->query_locations();
  else return 0;
}

mixed obtain_location()
{
  if (social_object_list[RACE_OB])
    return social_object_list[RACE_OB]->obtain_location();
  else return 0;
}

// Para los oficios, como herrero, mercader, minero, etc
mapping query_job_joined(){ return job_joined; }

void set_job_joined(mapping map)
{
  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  job_joined = map;
}

// neverbot 3/2002
void set_job_ob(string str)
{
  // string tmp;
  string old_job;
  mixed * values;

  old_job = social_object_list[JOB_OB];

  // Guardamos los datos del oficio que abandonamos
  if (old_job && load_object(old_job))
  {
      // Actualizamos los datos del guild anterior
      values = job_joined[old_job];
      job_joined[old_job] = ({ job_level,
                              (sizeof(values)?values[1]:time()),
                              time(),
                              });
  }

  if (!strlen(str))
  {
    // Eliminamos la especializacion si la tuviera
    this_object()->reset_job_spec();

    social_object_list[JOB_OB] = nil;
    job_level = 0;
    return;
  }

  // if (sscanf(str,"/%s", tmp) == 1)
  //   str = extract(str,1);

  // for any object, the valid directories are the common one, and the specific
  // for its game
  if (!_in_social_dirs(str, DEFAULT_JOB_DIR, "obj/jobs/"))
  {
    _log_social_path(str, "set_job_ob", "not in a jobs directory");
    return;
  }

  if (!_full_social_path(str, "set_job_ob"))
    return;

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;

  old_job = social_object_list[JOB_OB];

  social_object_list[JOB_OB] = str;
  job_xp = 0;

  // Si es uno en el que no hemos estado nunca
  if (undefinedp(job_joined[str]))
  {
    job_level = 1;
    job_joined[str] = ({ 1, time(), -1, });
  }
  // Volvemos a alistarnos en un oficio anterior
  else
  {
    values = job_joined[str];
    job_level = values[0];
  }

  // Eliminamos la especializacion si la tuviera
  this_object()->reset_job_spec();

  // Inicializacion del player
  // Esto solo ocurre la primera vez
  social_object_list[JOB_OB]->join_player(this_object());
  // Esto cada vez que conectemos
  social_object_list[JOB_OB]->start_player(this_object());
}

string query_job_ob() { return social_object_list[JOB_OB]; }

string query_job_name()
{
  if (social_object_list[JOB_OB])
    return ((string)social_object_list[JOB_OB]->query_short());
  else
    return ("Sin Oficio");
}

// Funciones relativas a las estadisticas del oficio
int query_job_xp() { return job_xp; }
int query_total_job_xp() { return total_job_xp; }

void logit_job(string what,int amount)
{
  if (interactive(this_object()))
  {
    log_file(LOGFILE, (string)this_object()->query_cap_name()+" gano "+amount+" "+what+
      " por: '"+previous_object()->query_name()+"' file: '"+file_name(previous_object())+
      "'\n");
}
}

int adjust_job_level(int i)
{
  object ranking;

  /* Those over lvl 100 is either immortals or cheat.. Noone needa it anyway.
   * Baldrick.     */
  // and never past the top level of the job itself
  if ((job_level + i) > 100 ||
      (query_job_ob() && (job_level + i) > query_job_ob()->query_max_level()))
  {
    notify_fail("Olvídalo.\n");
    return 0;
  }

  // on_level_change must be called before the level is adjusted
  if (query_job_ob())
    (query_job_ob())->on_level_change(this_object(), i);

  if (i >= 1)
    tell_player(this_object(), "¡Subes tu nivel de oficio!\n");

  job_level += i;

  // the new level may teach a skill of the job
  if (i > 0 && query_job_ob())
    catch(query_job_ob()->grant_job_skills(this_object()));

  // the new rank may carry a title with it
  this_object()->check_new_titles();

  // Log para players
  if (interactive(this_object()) && !this_object()->query_property(GUEST_PROP))
  {
    log_file(LOG_ADJUST_JOB_LEVEL, "["+ctime(time(),4)+"] "+this_object()->query_cap_name() +
           " ajusto "+i+" niveles ["+query_job_ob()+"], quedandose en "+job_level+"\n");

      // Ajuste de los rankings para players
    ranking = load_object(RANKING_OB);

    if (ranking)
      ranking->update(this_object());
  }
}

int query_job_level() { return job_level; }
int set_job_level(int i)
{
  if (!query_job_ob())
  {
    job_level = 0;
    return 0;
  }

  job_level = i;
  return job_level;
}

// Nivel maximo de todos los oficios en los que ha estado
int query_max_job_level()
{
  string* list;
  int i;
  int res;

  res = job_level;
  list = keys(job_joined);

  // job_joined[job_ob] = ({ nivel, 1º vez alistado, ultima vez abandonado, })
  for (i = 0; i < sizeof(list); i++)
    // El oficio actual estara almacenado con un nivel antiguo
    if (list[i] != query_job_ob())
      if (job_joined[list[i]][0] > res)
        res = job_joined[list[i]][0];

  return res;
}

int adjust_job_xp(int i)
{
  string player_job;
  int next_level;

  if (i > JOB_XP_LIMIT) logit_job(LOG_JOB_XP,i);
    if (i > 0)
    {
      i = (100 - (int)this_object()->query_wimpy()) * i / 100;
      total_job_xp += i;
    }

  job_xp += i;

  player_job = query_job_ob();
  if (player_job && (i > 0))
  {
    next_level = player_job->query_next_level_xp(this_object());
    if ((job_xp > next_level) &&
       (job_level < player_job->query_max_level()) )
    {
      adjust_job_level(1);
      job_xp -= next_level;
      this_object()->save();
    }
  }

    return job_xp;
} /* adjust_job_xp() */

int set_job_xp(int i)
{
  if ( (i - job_xp) > JOB_XP_LIMIT)
    logit_job(LOG_JOB_XP,(i - job_xp));
  if ( (job_xp - i) > JOB_XP_LIMIT)
    logit_job(LOG_JOB_XP, -(job_xp - i));
  if (i == -1 || !i)
    logit_job(LOG_JOB_XP, i);
  job_xp = i;
  return job_xp;
}/* int set_job_xp(int i) { job_xp = i; return job_xp; } */

// Para las Deidades. Al tener un objeto deidad podremos hacer mas
// cosas interesantes con los cultos a los dioses
// neverbot 3/2002
void set_deity_ob(string str)
{
  // string tmp;
  if (!strlen(str))
  {
    social_object_list[DEITY_OB] = nil;
    return;
  }

  //  if (sscanf(str,"/%s", tmp) == 1)
  //  str = extract(str,1);

  // A deity is either one the mudlib ships or one of the game's own, the same
  // way a handler or a table resolves. The game is taken from the path being
  // set, not from the caller: it is the deity that says which game it belongs
  // to.
  if (str[0 .. strlen("/lib/obj/deities") - 1] != "/lib/obj/deities" &&
      !(strlen(game_from_path(str)) &&
        str == "/games/" + game_from_path(str) + "/obj/deities/" +
               explode(str, "/")[sizeof(explode(str, "/")) - 1]))
  {
    write("Illegal path in set_deity_ob.\n");
    return;
  }

  if (!_full_social_path(str, "set_deity_ob"))
    return;

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;

  social_object_list[DEITY_OB] = str;
  social_object_list[DEITY_OB]->start_player(this_object());
  return;
}

/** The lifetime worship totals, a copy. */
mapping query_deity_totals()
{
  if (!mappingp(deity_totals))
    deity_totals = ([ ]);
  return ([ ]) + deity_totals;
}

/** Adds to a lifetime worship total. */
void add_deity_total(string key, int amount)
{
  if (!mappingp(deity_totals))
    deity_totals = ([ ]);
  deity_totals[key] = (deity_totals[key] ? (int)deity_totals[key] : 0) + amount;
}

/** Sets a lifetime worship total. */
void set_deity_total(string key, int value)
{
  if (!mappingp(deity_totals))
    deity_totals = ([ ]);
  deity_totals[key] = value;
}

string query_deity_ob() { return social_object_list[DEITY_OB]; }

string query_deity_name()
{
  if (social_object_list[DEITY_OB])
   return ((string)social_object_list[DEITY_OB]->query_short());
  else
   return ("Sin Deidad");
}

string query_deity(){ return query_deity_name(); }

// Para las ciudadanias.
// neverbot 4/2003
void set_city_ob(string str)
{
  // string tmp;
  if (!strlen(str))
  {
    social_object_list[CITY_OB] = nil;
    return;
  }

  // if (sscanf(str,"/%s", tmp) == 1)
  //  str = extract(str,1);

  // A citizenship object lives under an ".../obj/citizenships/" directory:
  // the shared /lib/obj/citizenships, or a game's own
  // /games/<game>/obj/citizenships. Accept any path with that segment rather
  // than a fixed prefix, so per-game citizenships validate too.
  if (strsrch(str, "/obj/citizenships/") < 0)
  {
    write("Illegal path in set_city_ob.\n");
    return;
  }

  if (!_full_social_path(str, "set_city_ob"))
    return;

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;

  social_object_list[CITY_OB] = str;
  social_object_list[CITY_OB]->start_player(this_object());
  return;
}

string query_city_ob() { return social_object_list[CITY_OB]; }

string query_city_name()
{
  if (social_object_list[CITY_OB])
    return ((string)social_object_list[CITY_OB]->query_short());
  else
    return ("Sin Ciudadanía");
}
string query_city(){ return query_city_name(); }
string query_citizenship(){ return query_city_name(); }

int query_citizen()
{
  if (social_object_list[CITY_OB])
    return 1;
  return 0;
}

// this was in player.c, now here (useful for monster.c too), neverbot 6/03
int query_level(){ return class_level; }

int adjust_level(int i)
{
  object ranking;

  // Those over lvl 100 is either immortals or cheat.. Noone needa it anyway.
  // Baldrick.
  if ((class_level + i) > 100)
  {
    notify_fail(_LANG_SOCIAL_FORGET_ABOUT_IT);
    return 0;
  }

  // on_level_change must be called before the level is adjusted
  if (query_class_ob())
    query_class_ob()->on_level_change(this_object(), i);

  // message about level up (except the first one, which is automatic)
  if (interactive(this_object()) && (class_level >= 5) && (i > 0))
  {
    this_object()->user()->add_notification("player", _LANG_SOCIAL_LEVEL_UP);

    log_file(LOG_ADJUST_LEVEL, "[" + ctime(time(), 4) + "] " + 
             this_object()->query_cap_name() + " adjusted " + i + 
             " levels [" + query_class_ob() + "], up to level " + class_level + "\n");
  }

  class_level += i;

  // the new rank may carry a title with it
  this_object()->check_new_titles();

  // adjust player rank
  if (interactive(this_object()) && !this_object()->query_property(GUEST_PROP))
  {
    ranking = load_object(RANKING_OB);

    if (ranking)
      ranking->update(this_object());
  }

  return class_level;
}

int query_class_level(){ return query_level(); }
int query_guild_level(){ return guild_level; }
// max level of all guilds he has been
int query_max_guild_level()
{
  string * list;
  int i, res;

  res = guild_level;
  list = keys(guild_joined);

  // guild_joined[guild_ob] = ({ nivel, 1º vez alistado, ultima vez abandonado, })
  for (i = 0; i < sizeof(list); i++)
    // the current guild will be stored with an old level
    if (list[i] != query_guild_ob())
      if (guild_joined[list[i]][0] > res)
        res = guild_joined[list[i]][0];

  return res;
}

int adjust_class_level(int i){ return adjust_level(i); }
int adjust_guild_level(int i)
{
  object ranking;

  /* Those over lvl 100 is either immortals or cheat.. Noone needa it anyway.
   * Baldrick.     */
  // and never past the top level of the guild itself
  if ((guild_level + i) > 100 ||
      (query_guild_ob() && (guild_level + i) > query_guild_ob()->query_max_level()))
  {
    notify_fail("Olvídalo.\n");
    return 0;
  }

  // on_level_change must be called before the level is adjusted
  if (query_guild_ob())
    query_guild_ob()->on_level_change(this_object(), i);

  // Mensaje de subida de nivel (excepto la primera subida que es automatica)
  if ((guild_level >= 1) && (i > 0))
    tell_player(this_object(), "¡Subes tu nivel de gremio!");

  guild_level += i;

  // the new level may teach a skill of the guild
  if (i > 0 && query_guild_ob())
    catch(query_guild_ob()->grant_guild_skills(this_object()));

  // the new rank may carry a title with it
  this_object()->check_new_titles();

  // Log para players
  if (interactive(this_object()) && !this_object()->query_property(GUEST_PROP))
  {
    log_file(LOG_ADJUST_GUILD_LEVEL, "["+ctime(time(),4)+"] "+this_object()->query_cap_name() +
           " ajusto "+i+" niveles ["+query_guild_ob()+"], quedandose en "+guild_level+"\n");

      // Ajuste de los rankings para players
    ranking = load_object(RANKING_OB);

    if (ranking)
      ranking->update(this_object());
  }

  return guild_level;
}

// Una ayuda para el sistema de avance de niveles
int query_class_xp_needed()
{
  if (social_object_list[CLASS_OB])
    return social_object_list[CLASS_OB]->query_next_level_xp(this_object());
  return 0;
}

int query_guild_xp_needed()
{
  if (social_object_list[GUILD_OB])
    return social_object_list[GUILD_OB]->query_next_level_xp(this_object());
  return 0;
}

// Para el sistema de tipos de xp, neverbot 07/04
mapping query_xp_types()
{
  if (social_object_list[CLASS_OB])
    return social_object_list[CLASS_OB]->query_xp_types();
  return ([ ]);
}

mixed * stats()
{
  mixed * ret;
  ret = ({
    ({"Social Object List", social_object_list, }),
    ({"Deity Totals", deity_totals, }),
    ({"Guild Joined", guild_joined, }),
    ({"Job Joined", job_joined, }),
    ({"Guild Level", guild_level, }),
    ({"Class Level", class_level, }),
    ({"Total Job Xp", total_job_xp, }),
    ({"Job Xp", job_xp, }),
    ({"Job Level", job_level, }),
  });
  return ret + skills::stats() + specs::stats();
}
