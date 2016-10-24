/*
 * By Radix - 1996 for FRMud
 * race_ob       - Raza
 * guild_ob      - Gremio
 * group_ob      - Clanes (Special Interest Groups)
 * race_group_ob - Grupos Raciales (Clanes enanos, Tribus Orcas, Casas Drow, ...)
 * Nuevos CcMud:
 * job_ob        - Oficio
 * deity_ob      - Religion que profesa
 * city_ob       - Ciudadania
 * class_ob      - Clase (gremio "generico")
 * 
 * El archivo deja de ser guild-race.c para pasar a ser groups_obs.c (No se a quien
 * se le ocurrio poner un '-' en medio de un nombre de archivo, ya que hace imposible
 * cosas como guild-race::funcion() y similares.
 * neverbot@Cc, 23/4/2003
 *
 * Modificado el antiguo guild_joined, para mantener registro de guilds a los que
 * el jugador ha pertenecido. neverbot 18/7/03
 * 
 * Extraido todo lo relacionado con los antiguos comandos (gr_commands)
 *  para crear nuevo sistema de dotes (feats.c), neverbot 11/10/08
 *
 * Añadido nuevo sistema de especialidades para cada uno de los grupos sociales
 *  neverbot 20/10/2012
 *
 * Renamed as social.c, neverbot 10/2016
 */

#include <user/player.h>
#include <living/social.h>
#include <living/races.h>
#include <mud/secure.h>

inherit feats "/lib/living/feats";
inherit specs "/lib/living/specs";

// guild_joined[guild_ob] = ({ nivel, 1º vez alistado, ultima vez abandonado, })
mapping guild_joined;
mapping job_joined;

int class_level;
int guild_level;

// Tenemos demasiadas variables globales, asi que todos los xx_ob los
// meto en un solo array, neverbot 3/2002
string * social_object_list;
// string race_ob, guild_ob;
// string group_ob, race_group_ob;
// string job_ob, deity_ob;

// Oficios
int total_job_xp, job_xp;
int job_level;

void create()
{
  social_object_list = allocate(NUM_SOCIAL_OBJECTS);
  guild_joined = ([ ]);
  job_joined = ([ ]);
  guild_level = 0;
  class_level = 0;
  job_level = 0;
  job_xp = 0;
  total_job_xp = 0;    
  
  feats::create();
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

void social_commands()
{
  feats_commands();
  specs_commands();
} /* groups_commands(), antes race_guild_commands() */

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
} 

string query_gtitle()
{
  string str;

  if (social_object_list[GUILD_OB] &&
     !catch((str = (string)social_object_list[GUILD_OB]->query_title(this_object()))))
    return str;

  // return "(su gremio no tiene título)";
  return "";
}

void set_race_ob(string str) 
{
  // string frog;
    
  if (!strlen(str)) 
    str = RACES_PATH + "unknown";
  
  // if ( sscanf(str, "/%s", frog)==1)
  //   str = extract(str, 1);

  if (str[0..strlen(RACES_PATH)-1] != RACES_PATH) 
  {
    write("Path ilegal para set_race_ob.\n");
    return;
  }

  if (str[0..strlen(RACES_PATH+"god")-1] == RACES_PATH+"god" &&
    !SECURE->high_programmer(geteuid())) 
  {
    write("El cielo retumba y los reinos tiemblan. No puedes hacer eso.\n");
    return;
  }

  if ( (file_size(str) < 0) && (file_size(str+".c") < 0) ) 
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
    this_object()->remove_languages(social_object_list[RACE_OB]->query_initial_languages());
      // Asignamos uno de los lenguajes actuales, neverbot 02/2006
      if (sizeof(this_object()->query_languages()) > 0)
        this_object()->set_language(this_object()->query_languages()[0]);

      this_object()->adjust_ext_align(-social_object_list[RACE_OB]->query_ext_align());
      // Quitamos los alias raciales
      if (social_object_list[RACE_OB]->query_base_race())
      {
        this_object()->remove_alias(lower_case(this_object()->query_race_name()));
        this_object()->remove_alias(lower_case(this_object()->query_base_race_name()));
      }
      else
      {
        this_object()->remove_alias(lower_case(this_object()->query_race_name()));
      }
  }

  social_object_list[RACE_OB] = str;
  this_object()->add_languages(social_object_list[RACE_OB]->query_initial_languages());
  // Asignamos uno de los lenguajes (el primero), neverbot 02/2006
  if (sizeof(social_object_list[RACE_OB]->query_initial_languages()) > 0)
    this_object()->set_language(social_object_list[RACE_OB]->query_initial_languages()[0]);

  this_object()->set_weight(social_object_list[RACE_OB]->query_race_weight());
  social_object_list[RACE_OB]->set_racial_bonuses(this_object());
  
  // Problema: con subrazas esto añade el alias "Humano (Velan)" por ejemplo
  // this_object()->add_alias(lower_case(this_object()->query_race()));
  if (social_object_list[RACE_OB]->query_base_race())
  {
    // Añadimos subraza
    this_object()->add_alias(lower_case(this_object()->query_race_name()));
    // Añadimos raza base
    this_object()->add_alias(lower_case(this_object()->query_base_race_name()));
  }
  else
  {
    // Unicamente raza base si nuestra raza no tiene subrazas
    this_object()->add_alias(lower_case(this_object()->query_race_name()));
  }

  // if (this_object()->query_player()) 
  //  this_object()->add_alias("*"+lower_case(this_object()->query_race())+"*");

  // neverbot, nuevo sistema de alinamiento
  this_object()->adjust_ext_align(social_object_list[RACE_OB]->query_ext_align());

  this_object()->reset_hands();

  social_object_list[RACE_OB]->start_player(this_object());

  return;
} /* set_race_ob() */

void set_race(string str) 
{
  set_race_ob(RACES_PATH + str);
}

string query_race_ob() { return social_object_list[RACE_OB]; }

// Nuevo sistema de subrazas, neverbot 6/03
string query_base_race_name() 
{ 
  object ob;
  if (social_object_list[RACE_OB])
    if (ob = social_object_list[RACE_OB]->query_base_race())
        return (string)ob->query_name(); 
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

string query_race()
{ /* should return the name of our race */
  if (social_object_list[RACE_OB])
    return ((string)social_object_list[RACE_OB]->query_short());
  else
    return "Sin Raza";
} /* query_race() */

// neverbot 6/03
int query_race_size()
{
  if (social_object_list[RACE_OB])
    return (int)social_object_list[RACE_OB]->query_race_size();
  else
    return 5; // Tamaño estandar (humano)
} /* query_race_size() */

// *****************************************
//  Nuevo objeto para la Clase, neverbot 6/03
// *****************************************

void set_class_ob(string str) 
{
  if (!stringp(str)) 
  {
    social_object_list[CLASS_OB] = nil;
    return;
  }
  
  if ( (file_size(str) < 0) && (file_size(str+".c") < 0) )
    return; 

  social_object_list[CLASS_OB] = str;
  this_object()->set_xp(0);
  this_object()->set_extreme_str(0);
  class_level = 1;
} /* set_class_ob() */

void set_class(string str) 
{
  set_class_ob("/lib/obj/classes/"+str);
}

string query_class_ob() { return social_object_list[CLASS_OB]; }

string query_class_name()
{
  if (social_object_list[CLASS_OB])
    return ((string)social_object_list[CLASS_OB]->query_name());
  else
    return ("Sin Clase");
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

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return; 

  // Asignamos el gremio nuevo
  social_object_list[GUILD_OB] = str;
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

void set_guild(string str) 
{
  set_guild_ob("/lib/obj/guilds/"+str);
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

  if (str[0..strlen("/lib/obj/groups")-1]!="/lib/obj/groups") 
  {
    write("Path ilegal para set_group_ob.\n");
    return;
  }

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
    if (file_size("/"+social_object_list[GROUP_OB]+".c") < 1)
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

  if (str[0..strlen("/lib/obj/race_groups")-1]!="/lib/obj/race_groups") 
  {
    write("Path ilegal en set_race_group_ob.\n");
    return;
  }

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

  if (str[0..strlen("/lib/obj/jobs")-1]!="/lib/obj/jobs") 
  {
    write("Path ilegal en set_job_ob.\n");
    return;
  }

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
  if ((job_level + i) > 100) 
  {
    notify_fail("Olvídalo.\n");
    return 0;
  }

  // Las funciones new_levels deben llamarse antes de ajustar el nivel!!
  if (query_job_ob())
    (query_job_ob())->new_levels(i, this_object());

  if (i >= 1)
    tell_player(this_object(), "¡Subes tu nivel de oficio!\n");

  job_level += i;

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
      i = (100 - (this_object()->query_wimpy()) ) * i / 100;
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

  if (str[0..strlen("/lib/obj/deities")-1]!="/lib/obj/deities") 
  {
    write("Path ilegal en set_deity_ob.\n");
    return;
  }

  if ( !load_object(str) || (!load_object(str)->query_legal_player(this_object()) ) )
    return;
  
  social_object_list[DEITY_OB] = str;
  social_object_list[DEITY_OB]->start_player(this_object());
  return;
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

  if (str[0..strlen("/lib/obj/citizenships")-1]!="/lib/obj/citizenships") 
  {
    write("Path ilegal en set_city_ob.\n");
    return;
  }

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

// Antes en player.c, ahora aqui (util para monster.c tambien :), neverbot 6/03)
int query_level(){ return class_level; }

int adjust_level(int i)
{
  object ranking;
  
  // Those over lvl 100 is either immortals or cheat.. Noone needa it anyway. 
  // Baldrick.
  if ((class_level + i) > 100) 
  {
    notify_fail("Olvídalo.\n");
    return 0;
  }

  // Las funciones new_levels deben llamarse antes de ajustar el nivel!!
  if (query_class_ob())
    query_class_ob()->new_levels(i, this_object());

  // Mensaje de subida de nivel (excepto la primera subida que es automatica)
  if ((class_level >= 5) && (i > 0)) 
  {
    tell_player(this_object(), "¡Subes de nivel!");

    // Log para players
    if (interactive(this_object()))
      log_file(LOG_ADJUST_LEVEL, "["+ctime(time(),4)+"] "+this_object()->query_cap_name() + 
           " ajusto "+i+" niveles ["+query_class_ob()+"], quedandose en "+class_level+"\n");
  }

  class_level += i;

  // Ajuste de los rankings para players
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
// Nivel maximo de todos los gremios en los que ha estado 
int query_max_guild_level()
{
  string * list;
  int i, res;

  res = guild_level;
  list = keys(guild_joined);
  
  // guild_joined[guild_ob] = ({ nivel, 1º vez alistado, ultima vez abandonado, })
  for (i = 0; i < sizeof(list); i++)
    // El gremio actual estara almacenado con un nivel antiguo
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
  if ((guild_level + i) > 100) 
  {
    notify_fail("Olvídalo.\n");
    return 0;
  }

  // Las funciones new_levels deben llamarse antes de ajustar el nivel!!
  if (query_guild_ob())
    query_guild_ob()->new_levels(i, this_object());

  // Mensaje de subida de nivel (excepto la primera subida que es automatica)
  if ((guild_level >= 1) && (i > 0)) 
    tell_player(this_object(), "¡Subes tu nivel de gremio!");

  guild_level += i;
  
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
    ({"Guild Joined", guild_joined, }),
    ({"Job Joined", job_joined, }),
    ({"Guild Level", guild_level, }),             
    ({"Class Level", class_level, }),                          
    ({"Total Job Xp", total_job_xp, }),
    ({"Job Xp", job_xp, }),
    ({"Job Level", job_level, }),
  });
  return ret + feats::stats() + specs::stats();
}
