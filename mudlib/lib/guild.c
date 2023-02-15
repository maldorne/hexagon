/* 
 * Revisado y retocado para CcMud
 * Eliminados skills y especializaciones de armas de este sistema
 * Folken 13/7/03
 * 
 * Añadido sistema de subida de niveles de gremio por quests, 
 * Folken 29/07/2010
 */

inherit "/std/object.c";
inherit "/global/basic/quests.c";

#include <quests.h>

#define DEFAULT_COST 500
#define DEFAULT_XP_COST 4500

// string *guild_commands;
string *legal_races, *legal_classes;
string requirements_func;
int needed_ext_align;
string *specs_available;

nomask int query_is_guild() { return 1; }

void create() 
{
  // guild_commands = ({ });
  legal_races = ({ });
  legal_classes = ({ });
  specs_available = ({ });
  needed_ext_align = 0;
  requirements_func = "";
  quests::create(); // antes que object, o el setup dara errores por mappings no inicializados!!
  object::create();
}

string query_requirements_func() { return requirements_func; }
void set_requirements_func(string func) { requirements_func = func; }

// Para comprobar alineamientos
// 0 no significa neutral, significa que no importa el alineamiento!!!!
// Valores:
// 0: no importa
// 1: neutral
// 100: bueno
// -100: malo
int query_needed_ext_align() { return needed_ext_align; }
void set_needed_ext_align(int bing) { needed_ext_align = bing; }

int query_valid_align(int align)
{
  switch(needed_ext_align){
    case 1:
       if (0 <= align <= 100) return 1;
       else return 0;
       break;
    case 100:
       return (align > 100);
       break;
    case -100:
       return (align < 0);
       break;
    case 0:
    default:
       return 1;
       break;
  }
}

// Para comprobar razas
string * query_legal_races() { return legal_races; }
void set_legal_races(string * list) { legal_races = list; }

int query_legal_race(string race)
{
  if (member_array(race, legal_races) == -1)
    return 0;
  return 1;
}

// Para comprobar clases
string * query_legal_classes() { return legal_classes; }
void set_legal_classes(string * list) { legal_classes = list; }

int query_legal_class(string name)
{
  if (member_array(name, legal_classes) == -1)
    return 0;
  return 1;
}

int query_legal_weapon(string weapon_name) { return 1; }
int query_legal_armour(string weapon_name) { return 1; }
/*
int query_valid_melee_weapon(object ob) { return 1;} 
int query_number_worn(string str) { return 1; }
string query_spell_directory() { return 0; }
mixed query_legal_spheres() { return ({ }); }
*/

int query_legal_player(object player)
{  
  if (!objectp(player) || !player)
    return 0;
  // Comprobamos raza, clase y alineamiento externo
  if (!query_legal_race(player->query_race_name()))
    return 0;
  if (!query_legal_class(player->query_class_name()))
    return 0;
  if (!query_valid_align(player->query_ext_align()))
    return 0;
    
  if (requirements_func && function_exists(requirements_func, this_object()))
    return call_other(this_object(), ({ requirements_func, player }));
}

// string help() { return read_file("/doc/helpdir/guilds"); }
int query_channel() { return 1; }

int query_next_level_xp(object player)
{  
  int res = player->query_guild_level() * DEFAULT_XP_COST;
  if (res == 0)
     return DEFAULT_XP_COST;
  return res;
}

/* Simply how much money the guild will charge to train you  */
int query_advance_cost() { return DEFAULT_COST; }
/* Seems we won't use this one, but check *bits of skills instead */
// int query_xp_cost() { return DEFAULT_XP_COST; }
int query_max_level() { return 30; }
int query_extra_gp_bonus() { return 0; }

/*
// Comandos del gremio:
string *query_guild_commands() { return guild_commands; }

int add_guild_command(string name, int add_if_player) {
  if (member_array(name, guild_commands) != -1)
    return 0;
  guild_commands += ({ name, ({ add_if_player }) });
  return 1;
}

int remove_guild_command(string name) {
  int i;
  if ((i = member_array(name, guild_commands)) == -1)
    return 0;
  guild_commands = delete(guild_commands, i, 1);
  return 1;
}
*/

string query_title(object player) 
{
  return (capitalize(query_short()) + " de nivel " + 
           player->query_level() /* +".\n" */ );
}

void start_player(object pl) 
{
  /*
  int i;

  // level = (int)pl->query_level();
  if (!pointerp(guild_commands))
    guild_commands = ({ });
  for (i=0;i<sizeof(guild_commands);i+=2)
    if (guild_commands[i+1][0] || !interactive(pl))
      pl->add_known_command(guild_commands[i]);
  */
}

void on_death(object player, object killer)
{
}  

void on_kill(object player, object victim)
{
}

int player_quit(object player) 
{
    return 1;
}

void new_levels(int lvl, object ob) 
{
  int e, gp_bonus = 0;
  int gps = 0;
  object class_ob = ob->query_class_ob();
    
  ob->reset_all();

  gp_bonus = ob->query_stat_bonus(ob->query_gp_main_stat()) + (query_extra_gp_bonus());

  // Subimos todos los niveles
  for (e = 0; e < lvl; e++)
  {
      // Solo subimos gps si es un nuevo nivel maximo
      if (ob->query_guild_level() >= ob->query_max_guild_level())
      {
          if(class_ob)
          {
              // En el primer nivel se obtiene el maximo
              if (ob->query_guild_level() == 0)
                  gps = class_ob->query_gp_dice();
              else 
                  gps = roll(1, class_ob->query_gp_dice());
          }
          else // Newbies y/o NPCs sin guild.
          {
              if(interactive(ob))
                  gps = roll(1, 6);
              else
                  gps = roll(1, 8);
          }

          gps += gp_bonus;
          
          if (gps <= 0) 
              gps = 1;

          ob->set_max_gp(ob->query_max_gp() + gps);
    
          // Logeamos en /secure, para que nadie pueda ver estas cosas
          if (interactive(ob))
              secure_log_file("guilds", "[" + ctime(time(),4) + "] " + 
                  ob->query_cap_name() + 
                  " sube "+gps+" pgs ("+base_name(this_object())+") "+
                   "(a nivel "+(ob->query_guild_level()+lvl)+").\n");
        }  
    }
}

/* Not something players should be able to do until we add dual class humans.
 * Up until then we leave the function open so that lords can call it
 * instead of set_guild_ob() which they often seem to bungle.
 * We will stop removing spells when dual class is implemented
 */
 /*
void leave_guild(){
  if (this_object()->query_spell_directory() != 0) // Folken
    "/global/spells"->reset_spells();
}
*/
/* What happens when you advance in level */
/*
int do_command(string name, mixed param) {
  mixed *tmp;

  if (member_array(name, guild_commands))
    return 0;
  tmp = (mixed *)COMMAND_SERVER->query_command(name);
  return (int)call_other(tmp[0], tmp[1], param);
}
*/


// Se llamara cuando un jugador intente abandonar el gremio
// Si devolvemos 1, le permitimos hacerlo
// Si devolvemos 0, el jugador no puede dejar de tener este gremio
int can_abandon(object player)
{
  return 1;
}

// Se llama cuando se produce realmente el abandono del gremio
int guild_abandon(object player)
{
  if (player->query_player())
  {
    string * quests = player->query_quests();
    int i;
    object q;
    
    // Buscamos todas las quests hasta encontrar la de este gremio 
    // (como maximo se puede tener una quest de gremio, pero buscamos
    //  en todas por si ha habido algun error)
    for (i = 0; i < sizeof(quests); i+=2)
    {
      q = load_object(quests[i]);

      if (!q)
        continue;

      if ((q->query_quest_category() == QUEST_GUILD) && 
          (q->query_provider_path() == base_name(this_object())))
      {
        player->remove_quest(quests[i]);
      }
    }
  }
  
  return 1;
}

int asign_guild_quest(object pl, int level)
{
  string * quests = this_object()->query_quests();
  int i, result = 0;
  object q;

  // Recorremos todas las quests buscando la correspondiente al 
  // siguiente nivel 
  for (i = 0; i < sizeof(quests); i++)
  {
    q = load_object(quests[i]);

    if (!q)
      continue;

    if ((q->query_quest_category() == QUEST_GUILD) && 
        (q->query_provider_path() == base_name(this_object())) &&
        (q->query_quest_advance_level() == level))
    {
      result = pl->add_quest(quests[i]);
      if (result)
        tell_player(pl, "Tienes una nueva misión de gremio.\n");
      break;
    }
  }

  return result;
}

// Se llama cuando el living se alista, para añadir dotes, etc
int join_player(object pl)
{
  // Asignamos la primera quest (la correspondiente para subir a nivel 2)
  // Cambio por level + 1, pueden haber abandonado previamente el gremio y haber vuelto
  if (pl->query_player())
    asign_guild_quest(pl, pl->query_guild_level() + 1);
  
  return 1;
}

// Sistema de especializaciones de gremios
string * query_guild_specs() { return specs_available; }
void set_guild_specs(string * arr) { specs_available = ({ }) + arr; }
void add_guild_specs(string name) { specs_available += ({ name }); }

int query_legal_spec(string str) { return member_array(str, specs_available) != -1; }

