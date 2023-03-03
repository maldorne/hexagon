/*
 * Weapon.c, reviewed for CcMud, neverbot 6/03
 *
 * old weapon_logic.c added
 * weapon difficulty, max dex bon, weapon family for masteries 
 *   neverbot 6/03
 *
 * armed styles auto-learning
 *   neverbot 08/05
 * 
 * reviewed for Hexagon
 * part of the old weapon_table.c added here
 *   neverbot 03/23  
 */

inherit item "/lib/item.c";

#include <basic/move.h>
#include <living/combat.h>
#include <basic/condition.h>
#include <item/item.h>
#include <translations/weapon.h>
#include <translations/combat.h>
#include <item/material.h>
#include <language.h>

int enchant;

static int basic_cost, // cost of a basic weapon of this type
  ench_basic_cost,     // cost of a basic weapon of this type with enchant

  attack_type,         // attack type, 1 slashing, 2 blunt, 3 piercing
  material,            // material of the weapon
  difficulty,          // difficulty of handling
  max_dex_bon,         // max dex bonus if using this weapon
  multiplier,          // multiplier to damage if we do a critical hit
  num_of_attacks,      // number of attacks (usually one, only two in exotic
                       //   weapons like double swords, etc)
  hands_needed;

static string weapon_name,   // weapon name
              weapon_family; // weapon family (to check masteries)

static object attacker, defender;
mapping attacks;


void create() 
{
  hands_needed = 1;
  enchant = 0;
  attacks = ([]);
  item::create();
  set_holdable(1);
} 

// prototypes
string query_attack_type_name(int num);
void add_attack(string name, int type, int num_dice, int type_dice, int bonus);

// functions
int query_weapon() { return 1; } 
string query_weapon_name() { return weapon_name; }
string query_weapon_family() { return weapon_family; }
string query_base_weapon() { return weapon_name; }
void set_weapon_type(int n) { attack_type = n; }
int query_weapon_type() { return attack_type; }
int query_attack_type() { return attack_type; }
int query_material() { return material; }
int query_enchant() { return enchant; }
int query_difficulty() { return difficulty; }
int query_max_dex_bon() { return max_dex_bon; }
int query_multiplier() { return multiplier; }
int query_num_of_attacks() { return num_of_attacks; }

int query_hands_needed() { return hands_needed; }

// added for ranged weapons, neverbot 12/2006
// in any other weapon it should not be needed, it would take this data
// directly from the weapon table
void set_hands_needed(int h) { hands_needed = h; }
// void set_twohanded(int flag) { twohanded = flag; }
// int query_twohanded() { return twohanded; }

void set_value(int i);

void set_base_weapon(string lookup)
{
  mixed *data;
  
  data = table("weapons")->lookup_weapon_data(lookup);
  weapon_name = lookup;
  basic_cost = data[0];
  ench_basic_cost = data[1];
  set_weight(data[2]);
  set_size(data[3]);
  attack_type = data[4];
  material = data[8]; 
  difficulty = data[9];
  max_dex_bon = data[10];
  multiplier = data[11];
  weapon_family = data[12];
  hands_needed = data[13];
  num_of_attacks = data[14];

  add_attack(BASIC_ATTACK_NAME, attack_type, data[5], data[6], data[7]);

  set_value(basic_cost);
}

void set_value(int i)
{
  if (enchant <= 0)
    ::set_value(basic_cost + (MAX_COND - cond) * COST_MULTIPLIER_TO_FIX_WEAPON);
  else
    ::set_value(ench_basic_cost * enchant + (MAX_COND - cond) * COST_MULTIPLIER_TO_FIX_WEAPON);
}

void set_enchant(int i)
{
  if (i > 6)
    i = 6;
  if (i < -3)
    i = -3;
  // damage because of enchant is already added in weapon_attack
  // roll_add += (i - enchant);
  enchant = i;
} 

void dest_me()
{
  enchant = 0;
  ::dest_me();
}

string query_attack_type_name(int type)
{
  switch (type)
  {
    case SLASHING:
      return SLASHING_STR;
    case BLUNT:
      return BLUNT_STR;
    case PIERCING:
      return PIERCING_STR;
    default:
      return SLASHING_STR;
  }
}

mixed * stats()
{
  return item::stats() + ({ 
              ({ "Enchant", enchant, }),
              ({ "Hands Needed (nosave)", hands_needed, }),
              ({ "Basic Cost (nosave)", basic_cost, }),
              ({ "Ench Basic Cost (nosave)", ench_basic_cost, }),
              ({ "Max Dex Bon (nosave)", max_dex_bon, }),
              ({ "Difficulty (nosave)", difficulty, }),
              ({ "Material (nosave)", material, }),
              ({ "Weapon Name (nosave)", weapon_name, }),
              ({ "Weapon Family (nosave)", weapon_family, }),
              ({ "Attack Type (nosave)", attack_type, }),
              ({ "Attacks", attacks, }),
          }); 
} 

mapping query_auto_load_attributes() 
{
  if (enchant == 0)
    return item::query_auto_load_attributes() +
           ([ "attacks" : attacks, ]);
  else
    return item::query_auto_load_attributes() +
           ([ "enchant" : enchant,
              "attacks" : attacks,
            ]);
}

void init_auto_load_attributes(mapping args) 
{
    if (!undefinedp(args["enchant"]))
      enchant = args["enchant"];
    if (!undefinedp(args["attacks"]))
      attacks = args["attacks"];
 
    item::init_auto_load_attributes(args);
}

// *************************************************
//   old weapon_logic.c
// *************************************************

mapping remove_attack(string name) { return attacks = m_delete(attacks, name); }
mapping query_attacks() { return attacks; }

void add_attack(string name,   // to index
                int type,      // piercing, blunt, slashing
                int num_dice,  // num of dice
                int type_dice, // num of faces of the dice
                int bonus)     // bonus to add
                               // format: 2D6+6
{
  attacks[name] = ({ type, num_dice, type_dice, bonus });
}       

// workout_attack taken from unarmed_combat to make them similar, neverbot 6/03
mixed * workout_attack(int type, int rolls, int dice, int roll_add)
{
  string happen;
  int result, attackerwc, defenderac, damage_done;
  
  // just in case, neverbot 4/03
  if (!defender || !attacker)
     return ({ MISS, 0 });

  damage_done = 0;

  // luck factor
  if (roll(1, 200) < 10)
  {
    // fumble message added
    tell_object(this_object(), _LANG_WEAPON_FUMBLE_MSG);
    return ({ FUMBLE, 0 });
  }

  attackerwc = (int)attacker->query_total_wc();

  // check defender AC to know it we hit or not
  // important to check the type of protection against this type of weapon
  defenderac = defender->query_total_ac(type) + defender->query_skill_malus();
  
  // idea taken from the localization system by Iolo@Rl, the more attackers you
  // have, the more hits you will receive
  //   1 or 2 attackers, same AC
  //   3 or 4 attackers, half AC
  //   5 or 6 attackers, one third of the ac ...  

  // if ( sizeof(defender->query_attacker_list()) > 2)
  //   defenderac = (defenderac / (sizeof(defender->query_attacker_list()) / 2));

  // changed by neverbot, 10/2009. It does not depend of the number of attackers, but the difference
  // between your attackers and your oponent's. This way we avoid modifiying the AC in big battles 
  // like 6 vs 6
  {
    int ac_diff;
    ac_diff = sizeof(defender->query_attacker_list()) - sizeof(attacker->query_attacker_list());
  	if (ac_diff >= 2)
    	defenderac = (defenderac / (ac_diff / 2));	
  }

  // attacker ability plus some luck (1d10), minus deffender ac.
  result = ((attackerwc + roll(1, 10)) - defenderac);

  if (result >= 1)
  {
    happen = HIT;
    damage_done = (int)roll(rolls, dice) + roll_add + 
                  this_object()->query_enchant() +
                  (int)attacker->query_damage_bonus() + 
                  (int)attacker->query_stat_bonus_to_str();

    if (damage_done <= 0)
      damage_done = 1;

    // luck factor
    if (roll(1, 200) < 10) 
    {
      // critical hit message added
      tell_object(this_object(), _LANG_WEAPON_CRITICAL_MSG);
      damage_done *= multiplier;
    }

    // masteries auto-learning system, neverbot 08/05, modified 09/2012

    // ignore ranged weapons (you can hit with them in melee combat, but that should
    // not improve your "bow" mastery)
    if (!this_object()->query_ranged_weapon())
      attacker->adjust_num_armed_hits(1, this_object()->query_weapon_family());
  }
  else
  {
    // calculate if the miss was because of the defender dodging or because the attacker
    // fails (the result is irrelevant except in "narrative", we will change the messages
    // shown)
    result = ((attacker->query_level() + 
               attacker->query_dex() + 
               attacker->query_skill_malus()) + 
               roll(1,10)) -
             ((defender->query_level() + 
               defender->query_dex() + 
               defender->query_skill_malus()) + 
               roll(1,10));
  
    if (result < 0)
    {
      happen = DODGE;
      damage_done = 0;
    }
    else
    {
      happen = MISS;
      damage_done = 0;
    }
  }

  return ({ happen, damage_done });
} /* workout_attack */

// similar to the localization sytem Iolo used in Rl
mixed * recalc_damage(int i)
{
  int hps;
  mixed local;

  hps = 0;
  local = defender->obtain_location();

  if (!local)
    local = ({ 1, "" });

  // apply the modifications for the localization
  // (depending of the place, we'll do more or less damage with the same hit)
  hps = (int)((float)i * local[0]);

  return ({ hps, local[1] });
}

int weapon_attack(object def, object att)
{
  mixed *att_val;
  mixed damage;
  int absorbed_damage;
  object * obs;
  object where;
  int i, j, k, ret, percent;
  string * attack_names;

  attack_names = keys(attacks);
  attacker = att;
  defender = def;
  att_val = ({ });
  ret = 0;
  where = nil;
  
  // loop added, neverbot 6/03
  // with exotic weapons we do double number of attacks (two handed sword, etc)
  // WARNING!!: if we add a second attack to the table, it will happen twice too
  for (k = 0; k < num_of_attacks; k++)
  for (i = 0; i < sizeof(attack_names); i++)
  {  
    att_val = workout_attack(attacks[attack_names[i]][0],
                             attacks[attack_names[i]][1],
                             attacks[attack_names[i]][2],
                             attacks[attack_names[i]][3]);
  
    if (att_val[0] == HIT) 
    {
      damage = recalc_damage(att_val[1]);
      absorbed_damage = 0;

      // look for the protection item to ruin it
      if (damage[1] != "")
      {
        obs = defender->query_worn_ob();
        for (j = 0; j < sizeof(obs); j++)
        {
          if (obs[j]->query_localization() == damage[1])
          {
            // if we found the armour protecting the localization, ruin both objects 
            // if we did not found the armour, do not ruin the weapon
            where = obs[j];

            // calculate damage percentage absorbed by the armour
            if (obs[j]->query_ac_against(attacks[attack_names[i]][0]) > 0) 
            {
              if (obs[j]->query_ac_against(attacks[attack_names[i]][0]) > 10)
                percent = 100;
              else
                percent = obs[j]->query_ac_against()*10;
               
              absorbed_damage = damage[0]*percent/100;
            }
            
            // if the armour did not absorbed anything (equiment without ac)
            // we make full damage AND ruin the armour
            where->hit_item((absorbed_damage > 0)?absorbed_damage:1);
            this_object()->hit_item(1);
            break;
          }
        }
      }

      // tell_object(find_living("neverbot"), "@@ "+attacker->query_cap_name()+": "+
      //                                         absorbed_damage+"/"+damage[0]+"\n");
      // apply final damage
      defender->adjust_hp(-(damage[0] - absorbed_damage), attacker);

      if (where)
        handler("weapons")->write_message(damage[0], attacks[attack_names[i]][0], 
                       damage[1], attacker, defender, where);
      else
        handler("weapons")->write_message(damage[0], attacks[attack_names[i]][0], 
                       damage[1], attacker, defender);
      ret += damage[0];
    }
    else 
    {
      if (att_val[0] == DODGE)
      {
        tell_object(attacker, defender->query_cap_name() + _LANG_WEAPON_DODGE_MSG_ATTACKER);
        tell_object(defender, attacker->query_cap_name() + _LANG_WEAPON_DODGE_MSG_DEFENDER);
        tell_room(environment(attacker), attacker->query_cap_name() + 
                  _LANG_WEAPON_DODGE_MSG_ROOM + defender->query_cap_name() + 
                  _LANG_WEAPON_DODGE_MSG_ROOM_END, ({attacker, defender}));
        continue;
      }

      // fumble
      if (att_val[0] == FUMBLE)
      {
        tell_object(attacker, _LANG_WEAPON_FUMBLE_MSG_ATTACKER + defender->query_cap_name() + ".\n");
        tell_object(defender, attacker->query_cap_name() + _LANG_WEAPON_FUMBLE_MSG_DEFENDER);
        tell_room(environment(attacker), attacker->query_cap_name() + 
                  _LANG_WEAPON_FUMBLE_MSG_ROOM + defender->query_cap_name() + 
                  ".\n", ({attacker, defender}));
        continue;
      }

      // up to this point att_val[0] should be MISS
      // to show more "flavour": if the defender has weapons or shields, show a message
      // like they parry the attack

      obs = defender->query_held_ob();
      obs -= ({ 0 });

      // defender parries
      if (sizeof(obs) && (random(3) == 0))
      {
        // take a random item
        where = obs[random(sizeof(obs))];
        tell_object(attacker, defender->query_cap_name() + _LANG_WEAPON_PARRY_MSG_ATTACKER +
                    where->query_name() + ".\n");
        tell_object(defender, attacker->query_cap_name() + _LANG_WEAPON_PARRY_MSG_DEFENDER +
                    where->query_name() + ".\n");
        tell_room(environment(attacker), attacker->query_cap_name() + 
                    _LANG_WEAPON_PARRY_MSG_ROOM + defender->query_cap_name() + 
                    _LANG_WEAPON_PARRY_MSG_ROOM_END + where->query_name() + ".\n", 
                    ({attacker, defender}));
                    
        // ruin both items
        where->hit_item(1);
        this_object()->hit_item(1);
      }
      else // attacker fails
      {
        tell_object(attacker, _LANG_WEAPON_NO_HIT + defender->query_cap_name() + ".\n");
        tell_object(defender, attacker->query_cap_name() + _LANG_WEAPON_NOT_HIT + "\n");
        tell_room(environment(attacker), attacker->query_cap_name() + 
                  _LANG_WEAPON_NO_HIT_ROOM + defender->query_cap_name() + 
                  ".\n", ({attacker, defender}));
      }
    }
  }

  return ret;
} /* weapon_attack */
