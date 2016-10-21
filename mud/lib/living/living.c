/* 
 * Funciones de ciudadania extraidas, neverbot 4/2003
 * Nuevo alineamiento, neverbot 27/4/03
 * Eliminado it_them
 * Mejorada la herencia en general, neverbot 6/03
 * Nuevo sistema de combate para CcMud, neverbot 6/03
 *
 * Traida a este archivo la logica necesaria para recuperar vida, haciendola comun
 *   a players y npcs (antiguos monster::heart_beat y player::heart_beat, 
 *   ahora living::living_heart_beat()), neverbot 04/09
 *
 * Añadido do_death para los npcs o players de quest, neverbot 03/09
 */

// #include <living.h>
// #include <room.h> 
// #include <drinks.h>
// #include <quests.h>
#include <common/properties.h>
#include <living/combat.h>

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
// inherit "/global/living/spells.c";
inherit consent   "/lib/living/consent";
// inherit "/global/living/visited.c";

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
  consent::create();

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  movement::create();

  // every living has enable_commands activated,
  // so it will respond to living()
  enable_commands();

  add_action("do_equip",  "equipar");
  add_action("do_equip",  "equiparse");
  add_action("do_hold",   "empunyar");
  add_action("do_hold",   "empuñar");
  add_action("do_hold",   "sostener");
  add_action("do_unhold", "desempunyar");
  add_action("do_unhold", "desempuñar");
  add_action("do_unhold", "soltar");
  add_action("do_wear",   "ponerse");
  add_action("do_wear",   "ponerme");
  add_action("do_wear",   "vestir");
  add_action("do_unwear", "desvestir");
  add_action("do_unwear", "quitarse");
  add_action("do_unwear", "quitarme");

  combat_commands();    
  handle_commands();
  social_commands();
  // spell_commands();
  consent_commands();
}

int cannot_get_stuff() { return 1; }

void dest_me() 
{
  object *obs;
  int i;

  obs = deep_inventory(this_object());

  for (i = 0; i < sizeof(obs); i++)
    if(obs[i])
      obs[i]->dest_me();

  movement::dest_me();
}

void start_player()
{
  // groups_obs::start_player();

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
int query_total_ac(varargs string type)
{
  int ret, eac, bac;
  ret = 0;

  if (type)
    eac = this_object()->query_equip_ac(type);
  else
    eac = this_object()->query_equip_ac(BLUNT_STR);

  bac = this_object()->query_body_ac();

  if (this_object()->query_property(PASSED_OUT_PROP)) 
    ret += 5 + eac + bac;
  else
    ret += 10 + eac + bac;

  // Bonificador por la caracteristica de constitucion añadido
  ret += this_object()->query_stat_bonus_to_con();

  // Bonificador al AC por tamaño (de la raza)
  // Tamaño 5 == humano
  if (this_object()->query_race_size() <= 3)
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
         consent::stats() +
         movement::stats();
         // spells::stats() + 
         // visited::stats();
}
