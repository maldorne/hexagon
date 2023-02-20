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
#include <item/weapon.h>
#include <item/material.h>

#define COST_TO_FIX 9
#define SIZE 8

int enchant;

static int basic_cost, // Coste de un arma basica de este tipo.
  ench_basic_cost,     // Coste de un arma basica de este tipo con enchant.

  attack_type,         // Tipo de Ataque, 1 cortante, 2 aplastante y 3 punzante
  material,            // Material del que esta fabricado el arma
  difficulty,          // Dificultad de manejo
  max_dex_bon,         // Bonificador a la destreza maximo si se esta utilizando
                       // este arma
  multiplier,          // Multiplicador al daño si hacemos critico.
  num_of_attacks,      // Numero de ataques (generalmente uno, solo seran dos en
                       //   armas exoticas como espadas dobles, etc).
  hands_needed;

static string weapon_name,        // weapon name
              weapon_family;      // weapon family (to check masteries)

static object attacker, defender; // atacante y defensor
mapping attacks;                  // ataques del arma


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

// Añadida para armas de rango, neverbot 12/2006
// En cualquier otro arma no deberia ser necesario, saca estos datos
// directamente de la tabla de armas.
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
    ::set_value(basic_cost + (MAX_COND - cond) * COST_TO_FIX);
  else
    ::set_value(ench_basic_cost * enchant + (MAX_COND - cond) * COST_TO_FIX);
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

// change by neverbot, 6/03
// we should check if this won't make weapons ruin too fast nor too slow
int hit_weapon(int dam)
{
	int res;
  res = this_object()->adjust_cond(-1);
	if ((res <= 0) && interactive(environment(this_object())) )
		tell_player(environment(this_object()), "¡Tu "+query_short()+" se rompe en mil pedazos!\n");
  	return res;
}

string query_attack_type_name(int type)
{
  switch (type)
  {
    case 1:
      return SLASHING_STR;
    case 2:
      return BLUNT_STR;
    case 3:
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
    tell_object(this_object(), "¡Oh, qué torpeza!\n");
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
      tell_object(this_object(), "¡Oh, qué habilidad!\n");
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

// *************************************************
//   old weapon_table.c
// *************************************************

mixed _query_message(int damage, 
                     int attack_type,
                     string localization, 
                     object attacker, 
                     object defender,
                     varargs object where)
{
  string msg_me, msg_him, msg_env, aux;
  int relative;

  relative = 1;
  msg_me = msg_him = msg_env = "";
  msg_him = msg_env = attacker->query_cap_name();
 
  switch (attack_type) 
  {
    case SLASHING:
      msg_me += "Cortas";
      msg_him += " te corta";
      msg_env += " corta";
      break;
    case PIERCING: 
      msg_me +="Perforas";
      msg_him +=" te perfora";
      msg_env +=" perfora";
      break;
    case BLUNT:     
      msg_me += "Golpeas";
      msg_him += " te golpea";
      msg_env += " golpea";
      break;
    // case RES_FIRE:     
    //     msg_me += "Quemas";
    //     msg_him += " te quema";
    //     msg_env += " quema";
    //     relative = 0;
    //     break;
    // case RES_COLD:     
    //     msg_me += "Congelas";
    //     msg_him += " te congela";
    //     msg_env += " congela";
    //     relative = 0;
    //     break;
    default:
      msg_me += "Golpeas";
      msg_him += " te golpea";
      msg_env += " golpea";
      break;
  }
 
  aux = "";

  if (relative)
  {
    if (damage <= 0) 
      aux = " sin efecto";
    else 
      switch (damage) 
      {
        case 1..4:   aux = " débilmente";                 break;
        case 5..8:   aux = " con poca fuerza";            break;
        case 9..12:  aux = "";                            break;
        case 13..16: aux = " con fuerza";                 break;
        case 17..21: aux = " con mucha fuerza";           break;
        case 22..28: aux = " violentamente";              break;
        case 29..60: aux = " con una increíble fuerza";   break;
        default: aux =     " con una fuerza sobrehumana"; break;
      }
  }
 
  msg_me += aux;
  msg_him += aux;
  msg_env += aux;

  // if we have the object that hits
  if (where)
  {
    if (relative)
    {
      msg_me += " a "+defender->query_cap_name()+" en su "+where->query_name();
      msg_him+= " en tu "+where->query_name();
      msg_env+= " a "+defender->query_cap_name()+" en su "+where->query_name();
    }
    else
    {
      msg_me += " "+where->query_article()+" "+where->query_name()+" de "+defender->query_cap_name();
      msg_him+= " "+where->query_article()+" "+where->query_name();
      msg_env+= " "+where->query_article()+" "+where->query_name()+" de "+defender->query_cap_name();
    }
  }
  // do not have the object but we have the localization
  else if (localization && (localization != ""))
  {
    msg_me+=  " a "+defender->query_cap_name() + " en "+localization;
    msg_him+= " en "+localization;
    msg_env+= " a "+defender->query_cap_name() + " en "+localization;
  }
  // do not have object nor localization
  else
  {
    msg_me+=  " a "+defender->query_cap_name();
    msg_env+= " a "+defender->query_cap_name();
  }

 // relative damage
 /* 
 i = to_int( (100*(defender->query_hp()-damage))/defender->query_max_hp());
 
 if (damage<=0) aux = "";
 else switch (type) {
   case "slashing": 
      switch(i) {
        case 0..10 : aux =  " haciendo graves cortes";       break;
        case 11..25: aux =  " dejando severas heridas";      break;
        case 26..40: aux =  " haciendo profundas heridas";   break;
        case 41..60: aux =  " dejando una sangrante herida"; break;
        case 61..80: aux =  " dejando una ligera herida";    break;
        case 81..100: aux = " haciendo solo rasgunyos";      break;
        default: aux =      " dejando una herida mortal";    break;
      }
   default: 
     switch (i) {
       case 0..10 : aux =  " dejando graves heridas internas"; break;
       case 11..25: aux =  " haciendo graves heridas";         break;
       case 26..40: aux =  " provocando un terrible dolor";    break;
       case 41..60: aux =  " provocando grandes dolores";      break;
       case 61..80: aux =  " provocando serios hematomas";     break;
       case 81..100: aux = " provocando ligeros moratones";    break;
       default: aux =      " provocando mortales heridas";     break;
    }
 }
 */
 
  msg_me  += ".\n";
  msg_him += ".\n";
  msg_env += ".\n";

  return ({ msg_me, msg_him, msg_env });  
}

void _write_message(int damage,          // damage from the hit
                   int attack_type,      // attack type (slashing, etc)
                   string localization,  // localization in the body
                   object attacker,      // 
                   object defender,      // 
                   varargs object where) // armour piece that receives the hit
{
  mixed *messages;
  if (where)
    messages = _query_message(damage, attack_type, localization, attacker, defender, where);
  else
    messages = _query_message(damage, attack_type, localization, attacker, defender);  

  tell_room(environment(attacker), messages[2], ({attacker,defender}));
  tell_object(attacker, ATT + messages[0]);
  tell_object(defender, DFF + messages[1]);

  return;
} /* _write_message */

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
            if (obs[j]->query_total_ac_against(attacks[attack_names[i]][0]) > 0) 
            {
              if (obs[j]->query_total_ac_against(attacks[attack_names[i]][0]) > 10)
                percent = 100;
              else
                percent = obs[j]->query_total_ac_against()*10;
               
              absorbed_damage = damage[0]*percent/100;
            }
            
            // if the armour did not absorbed anything (equiment without ac)
            // we make full damage AND ruin the armour
            where->hit_armour((absorbed_damage > 0)?absorbed_damage:1);
            this_object()->hit_weapon(1);
            break;
          }
        }
      }

      // tell_object(find_living("neverbot"), "@@ "+attacker->query_cap_name()+": "+
      //                                         absorbed_damage+"/"+damage[0]+"\n");
      // apply final damage
      defender->adjust_hp(-(damage[0] - absorbed_damage), attacker);

      if (where)
        _write_message(damage[0], attacks[attack_names[i]][0], 
                       damage[1], attacker, defender, where);
      else
        _write_message(damage[0], attacks[attack_names[i]][0], 
                       damage[1], attacker, defender);
      ret += damage[0];
    }
    else 
    {
      if (att_val[0] == DODGE)
      {
        tell_object(attacker,defender->query_cap_name()+" logra esquivar tu ataque.\n");
        tell_object(defender,attacker->query_cap_name()+" intenta golpearte pero logras " +
           "esquivar su ataque.\n");
        tell_room(environment(attacker), attacker->query_cap_name() + 
                  " trata de golpear a " + defender->query_cap_name() + 
                  " pero "+defender->query_demonstrative()+" logra esquivar su ataque.\n", ({attacker,defender}));
        continue;
      }

      // fumble
      if (att_val[0] == FUMBLE)
      {
        tell_object(attacker,"Te haces un lío con tu arma y no consigues golpear a "+defender->query_cap_name()+".\n");
        tell_object(defender,attacker->query_cap_name()+" se hace un lío con su arma y no consigue golpearte.\n");
        tell_room(environment(attacker), attacker->query_cap_name() + 
                  " se hace un lío con su arma y no consigue golpear a " +
                  defender->query_cap_name() + ".\n", ({attacker,defender}));
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
        tell_object(attacker,defender->query_cap_name()+" bloquea tu ataque con su " +
                    where->query_name()+".\n");
        tell_object(defender,attacker->query_cap_name()+" intenta golpearte pero logras " +
                    "bloquear su ataque con tu "+where->query_name()+".\n");
        tell_room(environment(attacker), attacker->query_cap_name() + 
                    " trata de golpear a " + defender->query_cap_name() + 
                    " pero "+defender->query_demonstrative()+" bloquea el ataque con su " +
                    where->query_name()+".\n", 
                    ({attacker,defender}));
                  
        // ruin both items
        if (where->query_weapon())
          where->hit_weapon(1);
        else if (where->query_armour())
          where->hit_armour(1);
        else if (where->query_shield())
          where->hit_shield(1);
        else
          where->adjust_cond(-1);
        this_object()->hit_weapon(1);
      }
      else // attacker fails
      {
        tell_object(attacker,"No consigues golpear a "+defender->query_cap_name()+".\n");
        tell_object(defender,attacker->query_cap_name()+" no consigue golpearte.\n");
        tell_room(environment(attacker), attacker->query_cap_name() + 
                  " no consigue golpear a " + defender->query_cap_name() + 
                  ".\n", ({attacker,defender}));
      }
    }
  }

  return ret;
} /* weapon_attack */
