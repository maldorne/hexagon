/* 
 * citizenship functions extracted, neverbot 4/2003
 * new alignment, neverbot 27/4/03
 * it_them removed
 * inheritance improved in general, neverbot 6/03
 * new combat system for CcMud, neverbot 6/03
 *
 * bring to his file the needed logic for recovering life, making it common
 * to players and npcs (old monster::heart_beat and player::heart_beat,
 * now living::heart_beat()), neverbot 04/09
 *
 * do_death added for npcs and players, neverbot 03/09
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
 * New combat system for CcMud, neverbot 6/03
 * if the player has the PASSED_OUT_PROP, he has 5 less AC points
 */
int query_total_ac(varargs int type)
{
  int ret, eac, bac;
  ret = 0;

  if (undefinedp(type))
    eac = this_object()->query_equip_ac(BLUNT);
  else
    eac = this_object()->query_equip_ac(type);

  bac = this_object()->query_body_ac();

  if (this_object()->query_property(PASSED_OUT_PROP)) 
    ret += 5 + eac + bac;
  else
    ret += 10 + eac + bac;

  // bonus by constitution characteristic added
  ret += this_object()->query_stat_bonus_to_con();

  // bonus to ac by (race) size
  // size 5 == humano
  if (this_object()->query_body_size() <= 3)
    ret += 1;

  return ret;
}

/*
 * New combat system for CcMud, neverbot 6/03
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

  // if we are unarmed, the coeficient is different
  if (sizeof(obs) == 0)
  {
    ret += this_object()->query_current_unarmed_ability() / 10;
  }
  else // fighting with weapons
  {
    // add the difficulty of the weapon and its enchant
    for (i = 0; i < sizeof(obs); i++)
    {
      ret -= obs[i]->query_difficulty();
      ret += obs[i]->query_enchant();
    }

    // bonus by current mastery
    mastery = this_object()->query_current_mastery_value() / 10;
    
    if (mastery <= 0) mastery = 1;

    // with multiple weapons we divide the mastery value
    // Pending: when we have combat skills with multiple weapons,
    // improve this
    mastery = mastery/sizeof(obs);
    
    ret += mastery;
  }

  // add the bonus for the level
  // every 5 class levels, a wc point is added
  // Pending: check if we don't add too many levels, if we allow many
  // class levels, change the division / 10
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

// Hamlet -- this is so noncombat rooms can keep players from healing
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

  // weather only for players
  if (this_object()->query_player() && 
     ((hb_counter%5) == 0) && 
      environment(this_object()))
  {
    this_object()->weather_heart_beat();
  }

  // Quark, adding heart beat to curses.
  // if (!(hb_counter & 31))
  //   this_object()->curses_heart_beat();

  // hp regen system updated, neverbot@Cc, 13/7/03
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
      // racial check (if the race is in a condition to regen hp)
      //  ie: drows only in the underdark, etc...
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

// added to check if it is a quest npc
int do_death(varargs object killer)
{
  if (!killer)
    return ::do_death(killer);
  
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
