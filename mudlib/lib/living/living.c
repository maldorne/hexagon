/* 
 * Funciones de ciudadania extraidas, neverbot 4/2003
 * Nuevo alineamiento, neverbot 27/4/03
 * Eliminado it_them
 * Mejorada la herencia en general, neverbot 6/03
 * Nuevo sistema de combate para CcMud, neverbot 6/03
 *
 * Traida a este archivo la logica necesaria para recuperar vida, haciendola comun
 *   a players y npcs (antiguos monster::heart_beat y player::heart_beat, 
 *   ahora living::heart_beat()), neverbot 04/09
 *
 * Añadido do_death para los npcs o players de quest, neverbot 03/09
 */

#include <living/food.h>
#include <common/quests.h>
#include <common/properties.h>
#include <translations/combat.h>

inherit alignment "/lib/core/basic/alignment";
inherit death     "/lib/living/death";
inherit force     "/lib/living/force";
inherit money     "/lib/living/money";
inherit effects   "/lib/living/effects";
inherit equip     "/lib/living/equip";
inherit gender    "/lib/core/basic/gender";
inherit stats     "/lib/living/stats";
inherit combat    "/lib/living/combat";
inherit health    "/lib/living/health";
inherit handle    "/lib/living/handle";

inherit queue     "/lib/living/queue";
inherit movement  "/lib/living/movement";

inherit social    "/lib/living/social";
inherit mount     "/lib/living/mount";
inherit drunk     "/lib/living/drunk";
inherit spells    "/lib/living/spells";
inherit consent   "/lib/living/consent";
inherit visited   "/lib/living/visited";
inherit comm      "/lib/living/communicate";

int hb_counter;
static int hp_counter, gp_counter;
static int no_heal; 

void create()
{
  alignment::create();
  death::create();
  force::create();
  money::create();
  effects::create();
  equip::create();
  gender::create();
  stats::create();
  combat::create();
  health::create();
  handle::create();
  queue::create();
  social::create();
  mount::create();
  drunk::create();
  spells::create();
  consent::create();
  visited::create();
  comm::create();

  hp_counter = gp_counter = 0;
  hb_counter = 0;
  no_heal = 0;

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  movement::create();

  // every living has enable_commands activated, so it will respond to living()
  // no, this will be done when set_living_name() is called on a living object, 
  // enable_commands does not exist anymore. Now this happens:
  //  - on a player: from the login object
  //  - on a npc: when set_name is called
  // enable_commands();

  equip_commands();
  hold_commands();
  wear_commands();
  combat_commands();    
  handle_commands();
  social_commands();
  spell_commands();
  consent_commands();
  communicate_commands();
}

int cannot_get_stuff() { return 1; }

void dest_me() 
{
  object *obs;
  int i;

  obs = deep_inventory(this_object());

  for (i = 0; i < sizeof(obs); i++)
    if (obs[i])
      obs[i]->dest_me();

  movement::dest_me();
}

void start_player()
{
  social::start_player();

  // at the end
  money::start_money();
}

// resolve multiple instances between 
// value.c (for every object) and money.c (only for livings)

int query_value() { return money::query_value(); }
mixed * query_money_array() { return money::query_money_array(); }
int query_money(string type) { return money::query_money(type); }
int adjust_money(mixed i, varargs string type) { return money::adjust_money(i, type); }

// end resolving multiple instances

/* 
 * Nuevo sistema de combate para CcMud, neverbot 6/03
 * Si el jugador tiene la propiedad PASSED_OUT_PROP, tiene 5 puntos menos de AC
 */
int query_total_ac(varargs int type)
{
  int ret, eac, bac;
  ret = 0;

  if (type)
    eac = this_object()->query_equip_ac(type);
  else
    eac = this_object()->query_equip_ac(BLUNT);

  bac = this_object()->query_body_ac();

  if (this_object()->query_property(PASSED_OUT_PROP)) 
    ret += 5 + eac + bac;
  else
    ret += 10 + eac + bac;

  // Bonificador por la caracteristica de constitucion añadido
  ret += this_object()->query_stat_bonus_to_con();

  // Bonificador al AC por tamaño (de la raza)
  // Tamaño 5 == humano
  if (this_object()->query_body_size() <= 3)
    ret += 1;

  return ret;
}

/*
 * Nuevo sistema de combate para CcMud, neverbot 6/03
 */
int query_total_wc()
{
  int ret;
  int mastery, i;
  string guild_class;
  object * obs;

  obs = this_object()->query_weapons_wielded();
  ret = 0;

  ret += this_object()->query_stat_bonus_to_dex();
  ret += this_object()->query_tohit_bonus();

  if (guild_class = this_object()->query_class_ob())
    ret += guild_class->query_combat_bonus();

  // Si estamos desarmados el coeficiente es diferente
  if (sizeof(obs) == 0)
  {
    ret += this_object()->query_current_unarmed_ability() / 10;
  }
  else // Luchamos con armas
  {
    // Añadimos la dificultad de manejo de cada arma y su enchant
    for (i = 0; i < sizeof(obs); i++)
    {
      ret -= obs[i]->query_difficulty();
      ret += obs[i]->query_enchant();
    }

    // Bonificador por la maestria actual 
    mastery = this_object()->query_current_mastery_value() / 10;
    
    if (mastery <= 0) mastery = 1;

    // Con multi-arma dividimos el coeficiente!!!
    // PENDIENTE: Cuando haya alguna habilidad de combate con
    //  varias armas habria que mejorar esto
    mastery = mastery/sizeof(obs);
    
    ret += mastery;
  }

  // Sumamos un bonificador por el nivel
  // Cada 5 niveles (de la clase) un punto mas de wc
  // PENDIENTE: Comprobar que no se suben demasiados niveles, si finalmente se
  // permiten muchos niveles de clase, cambiar la division / 10
  ret += this_object()->query_level() / 5;
  return ret;
}

int query_hb_counter()
{
  return hb_counter;
}

int query_hb_diff(int oldc)
{
  if (hb_counter > oldc) 
    return hb_counter - oldc;
  else return oldc - hb_counter;
}

void heal_hp(int i)
{
  if (i && (query_hp() < query_max_hp()) && (query_hp() >= 0) && !no_heal)
    hp++;
  return;
}

void heal_gp(int i, int intox)
{
  if (i && intox < 200 && query_gp() < query_max_gp() && !no_heal)
    gp++;
  return;
}

/* Hamlet -- this is so noncombat rooms can keep players from healing */
void set_no_heal() { no_heal = 1; }
void set_heal() { no_heal = 0; }

void heart_beat()
{
  int regen_gp, regen_hp;
  int race_regen_gp, race_regen_hp;

  regen_gp = regen_hp = 0;
  race_regen_gp = race_regen_hp = 0;

  combat::heart_beat();

  queue::heart_beat(); // queue.c, will do act()

  hb_counter++;

  if (query_race_ob() && !(hb_counter & 31) )
    query_race_ob()->race_heartbeat(this_object());

  // Clima solo a players
  if (this_object()->query_player() && 
     ((hb_counter%5) == 0) && 
      environment(this_object()))
  {
    this_object()->weather_heart_beat();
  }

  // Quark, adding heart beat to curses.
  // if (!(hb_counter & 31))
  //   this_object()->curses_heart_beat();

  /* Sistema de regeneracion de vida actualizado, neverbot@Cc, 13/7/03 */
  if ((hp_counter + query_con()) >= 22 + random(10))
    regen_hp = 1;
  if ((gp_counter + query_gp_main_stat()) >= 22 + random(10))
    regen_gp = 1;

  if (regen_hp || regen_gp) 
  {
    if (regen_hp)
      hp_counter = -1;
    if (regen_gp)
      gp_counter = -1;

    if (query_race_ob())
    {
      // Comprobacion racial (si la raza esta en condiciones de subir vida)
      //  ej: drows solo en suboscuridad, etc...
      if (regen_hp && query_race_ob()->query_regen_hp(this_object()))
        heal_hp(regen_hp);
      if (regen_gp && query_race_ob()->query_regen_gp(this_object()))
        heal_gp(regen_gp, query_volume(D_ALCOHOL));
    }
    else
    {
      if (regen_hp)
        heal_hp(regen_hp);
      if (regen_gp)
        heal_gp(regen_gp, query_volume(D_ALCOHOL));
    }
  }

  hp_counter++;
  gp_counter++;
 
  if (this_object()->query_player() && this_object()->query_is_quiet())
  {
    hp_counter++;
    gp_counter++;
  }

  health::heart_beat();
  comm::heart_beat();
}

// Añadido para comprobar si es un npc de quest
int do_death(object killer)
{
  if (!killer)
    return ::do_death(killer);
  
  // Debemos actualizar _antes_ del do_death, o este objeto ya 
  // se habra destruido
  if (killer->is_doing_quest(base_name(this_object()), TYPE_KILL) )
    killer->update_quest(base_name(this_object()));
  else if (killer->is_doing_quest_from_name(this_object()->query_short(), TYPE_KILL) )
    killer->update_quest_from_name(this_object()->query_short());

  return ::do_death(killer);
}

mixed * stats() 
{
  return alignment::stats() +
         death::stats() +
         force::stats() +
         money::stats() +
         effects::stats() +
         equip::stats() +
         gender::stats() + 
         stats::stats() + 
         combat::stats() + 
         health::stats() + 
         handle::stats() + 
         queue::stats() + 
         social::stats() +
         mount::stats() +
         drunk::stats() +
         spells::stats() +
         consent::stats() +
         visited::stats() +
         comm::stats() +
         movement::stats();
}
