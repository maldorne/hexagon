
#include <living/combat.h>
#include <translations/combat.h>
#include <language.h>

/*
    Unarmed Combat for FR by Sojan
    Currently unlobotomised/castrated by Baldrick :)
    (sorry B)

    * On my way... Baldrick. June '94

    Ummm this is designed to be as similar to the weapon_logic
    as I could get it ... there are a few notable differences
    but not enough to be significant ..
    Okay due to current problems the unarmed/armed combat
    is gonna yield the same actual combat result ....
    damage will still be better for martial arts than for
    standard brawling though ......
    Oh - also most of the stuff in this file will come from
    an include file cause this doesn't rely on different weapons
    classes and stuff ... bingles ...
    
    Reviewed by neverbot for ccmud, 04/2003, adding Iolo's localization
    system and using (at last) combat styles.

    Added new automatic learning system for the combat styles
    neverbot 7/03
    
*/

static int ovr_num, ovr_type;
static object defender,
              attacker;
// unarmed damage messages by combat style. stored only once with every
// style change, to avoid looking through mappings in every hit
static mixed * messages;

// mapping with ([ "style name": number ]), the number being how much you
// know about the style
mapping known_unarmed_styles;
string current_unarmed_style;
// how much you know about yout current combat style, in its own variable
// to minimize looking through the mapping
int unarmed_ability;

// num of consecutive unarmed hits (to improve in your style)
private int num_unarmed_hits;

int query_num_unarmed_hits(){ return num_unarmed_hits; }
void reset_num_unarmed_hits(){ num_unarmed_hits = 0; }

int unarmed_attack(object def, object att);
mixed * workout_attack(string unarmed_type);
mixed * recalc_damage(int i);

void write_message(int damage, string local, object att, object defdr);
void unarmed_combat_commands();

// unarmed styles
int set_unarmed_combat_style(string style);
int add_known_unarmed_style(string style, varargs int dominio);
int remove_known_unarmed_style(string style);
mapping query_known_unarmed_combat_styles();
int adjust_unarmed_ability(string style, int i);

void unarmed_combat_commands()
{
  add_private_action("do_combat_styles", _LANG_UNARMED_STYLE_COMMANDS);
}

void create()
{
  ovr_num = 0;
  ovr_type = 0;
  num_unarmed_hits = 0;
  // every being has a minimum 50% in the default combat style
  known_unarmed_styles= ([ DEF_UNARMED_STYLE_NAME:50, ]);
  current_unarmed_style = DEF_UNARMED_STYLE_NAME;
  unarmed_ability = 50;
  messages = ({ });
}

int unarmed_attack(object def, object att)
{
  mixed *att_val;
  mixed damage;
  int absorbed_damage, j, percent;
  object * obs;
  
  defender = def;
  attacker = att;
  att_val = ({ });
  att_val = workout_attack(current_unarmed_style);

  if (att_val[0] == HIT) 
  {
    damage = recalc_damage(att_val[1]);
    absorbed_damage = 0;

    // look for the item you use to protect yourself
    if (damage[1] != "")
    {
      obs = defender->query_worn_ob();
      for (j = 0; j < sizeof(obs); j++)
      {
        if (obs[j]->query_localization() == damage[1])
        {
          // % of damage taken by the armour
          if (obs[j]->query_ac() + obs[j]->query_ac_bon_against(BLUNT) > 0) 
          {
              if ((obs[j]->query_ac() + obs[j]->query_ac_bon_against(BLUNT)) > 10)
                percent = 100;
              else
                percent = (obs[j]->query_ac() + obs[j]->query_ac_bon_against(BLUNT)) * 10;

              absorbed_damage = damage[0] * percent / 100;
          }
          break;
        }
      }
    }
 
    // final damage
    defender->adjust_hp(-(damage[0] - absorbed_damage), attacker);

    write_message(damage[0], damage[1], attacker, defender);

    // learning unarmed styles, neverbot 7/03
    num_unarmed_hits++;
    if ((num_unarmed_hits > 0) && (num_unarmed_hits%100 == 0))
    {
      // the more we know about the style, more difficult to improve
      if ((unarmed_ability < MAX_UNARMED_AUTO_LEARN) && (random(100) > unarmed_ability))
        adjust_unarmed_ability(current_unarmed_style, 1);
      num_unarmed_hits = 0;
    }   
  }
  else 
  {
    if (att_val[0] == DODGE)
    {
      damage = ({ 0 });
      tell_object(attacker, _LANG_UNARMED_DODGE_MESSAGE_ATT);
      tell_object(defender, _LANG_UNARMED_DODGE_MESSAGE_DEF);
      tell_room(environment(attacker), _LANG_UNARMED_DODGE_MESSAGE_ROOM, ({ attacker, defender }));
    }
    else
    {
      // att_val[0] == MISS
      damage = ({ 0 });
      tell_object(attacker, _LANG_UNARMED_BLOCK_MESSAGE_ATT);
      tell_object(defender, _LANG_UNARMED_BLOCK_MESSAGE_DEF);
      tell_room(environment(attacker), _LANG_UNARMED_BLOCK_MESSAGE_ROOM, ({ attacker, defender }));
    }
  }

  return damage[0];
} // unarmed_attack

mixed * workout_attack(string unarmed_type)
{
  string happen;
  int result, attackerwc, defenderac, damage_done;
  int tmp;

  // should not happen, neverbot 4/03
  if (!defender || !attacker)
    return ({ MISS, 0 });
     
  //  ovr_num and ovr_type are nosave, so we do not have values when we start
  //  we store it the first time we need them, to avoid looking through the mapping
  if ((ovr_num == 0) || (ovr_type == 0))
  {
    tmp = table("unarmed_combat")->set_damage_dice(current_unarmed_style, this_object());
    if (tmp == 0)
    {
      ovr_num = BDEFNUM;
      ovr_type = BDEFTYPE;
    }
  }

  damage_done = 0;

  // luck factor  
  if (roll(1, 200) < 10 )
  {
    // fumble default message, we should add more messages here someday
    tell_object(this_object(), _LANG_UNARMED_DEFAULT_FUMBLE_MSG);
    return ({ FUMBLE, 0 });
  }

  attackerwc = (int)attacker->query_total_wc();

  // malus because of the equipment (applied to the AC only to see if there is a hit or not) 
  defenderac = defender->query_total_ac() + defender->query_skill_malus();
  
  // Idea taken from Iolo's bonification system for RlMud, the more attackers you
  // are facing, more hits you are not able to see
  //   1 or 2 attackers, same AC
  //   3 or 4 attackers, half
  //   5 or 6 attackers, a third...

  // if ( sizeof(defender->query_attacker_list()) > 2)
  //   defenderac = (defenderac / (sizeof(defender->query_attacker_list()) / 2));

  // Changed by neverbot, 10/2009
  // It does not depend of the number of your attackers, but the difference between your
  // attackers and your oponent attackers (so we are not modifying the AC in crowd combat ie. 6 vs 6)
  {
    int ac_diff;
    ac_diff = sizeof(defender->query_attacker_list()) - sizeof(attacker->query_attacker_list());
    if ( ac_diff >= 2 )
      defenderac = (defenderac / (ac_diff / 2));  
  }

  // to the attacker unarmed ability we add the luck factor for both players
  // and we substract the defender AC
  result = ((attackerwc + roll(1, 10)) - defenderac);

  if (result > 1)
  {
    happen = HIT;
    damage_done = (int)roll(ovr_num, ovr_type) + 
                  (int)attacker->query_damage_bonus() +
                  (int)attacker->query_stat_bonus_to_str();

    if (damage_done <= 0)
      damage_done = 1;

    // luck factor  
    if ( roll(1, 200) < 10 ) 
    {
      // critical hit messaged added... should add messages here someday
      tell_object(this_object(), _LANG_UNARMED_DEFAULT_CRITICAL_MSG);
      damage_done *= 2;
    }
  }
  else
  {
    // calculate if the miss is because the defender dodged the attack of
    // because the attacker missed (result only matther in narrative ways,
    // different messaages)
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
} // workout_attack

int set_unarmed_combat_style(string style)
{
  int tmp;

  if (this_object()->query_dead())
  {
     notify_fail(_LANG_NO_NEED_WHEN_DEAD);
     return 0;
  }

  if (member_array(style, keys(known_unarmed_styles))>-1)
  {
    current_unarmed_style = style;
    // minimum ability
    unarmed_ability = (known_unarmed_styles[style] >= MIN_UNARMED_ABILITY ? known_unarmed_styles[style] : MIN_UNARMED_ABILITY);

    // update the dice used
    tmp = table("unarmed_combat")->set_damage_dice(current_unarmed_style, this_object());
    if (tmp == 0)
    {
      ovr_num = BDEFNUM;
      ovr_type = BDEFTYPE;
    }

    write(_LANG_UNARMED_STYLE_INFO);

    // reset the counter
    reset_num_unarmed_hits();
    
    // update the messages to use
    messages = table("unarmed_combat")->query_messages(current_unarmed_style, unarmed_ability, this_object());      
    
    return 1;
  }

  notify_fail(_LANG_UNARMED_NOT_KNOWN);
  return 0;
}

int add_known_unarmed_style(string style, varargs int dominio)
{
  if (!style) 
    return 0;

  if (!dominio)
    known_unarmed_styles[style] = MIN_UNARMED_ABILITY;
  else
    known_unarmed_styles[style] = dominio;
   
  return 1;
}

int remove_known_unarmed_style(string style)
{
  if (!style) 
    return 0;

  map_delete(known_unarmed_styles, style);
  
  if (style == current_unarmed_style)
  {
    current_unarmed_style = DEF_UNARMED_STYLE_NAME;
    unarmed_ability = MIN_UNARMED_ABILITY;
  }

  return 1;
}

string query_current_unarmed_style_id()
{
  return current_unarmed_style;
}

string query_current_unarmed_style()
{
  mixed * info;
  info = table("unarmed_combat")->query_unarmed_style_info(current_unarmed_style);
  
  if (!sizeof(info))
    return "error";

  return info[0];
}

int query_current_unarmed_ability()
{
  return unarmed_ability;
} 

// update the ability, return the final value
int adjust_unarmed_ability(string style, int i)
{
  if (!known_unarmed_styles[style])
    return 0;
  
  if (member_array(style, table("unarmed_combat")->query_unarmed_styles()) == -1)
    return 0;

  known_unarmed_styles[style] += i;

  if (style == current_unarmed_style)
    unarmed_ability += i;

  if (i > 0)
    tell_player(this_object(), _LANG_UNARMED_STYLE_IMPROVED);

  return known_unarmed_styles[style];   
}

mapping query_known_unarmed_combat_styles()
{
  return known_unarmed_styles;
}

int do_combat_styles(string style)
{
  int i;
  string ret;
  string line;
  string * _unarmed_styles;

  _unarmed_styles = keys(query_known_unarmed_combat_styles());

  if (this_object()->query_dead())
  {
    notify_fail(_LANG_NO_NEED_WHEN_DEAD);
    return 0;
  }

  if (!style || (style == ""))
  {
    int cols;
    string header;

    cols = this_object()->user()->query_cols();
    header = fix_string(_LANG_UNARMED_LIST_HEADER);

    line = sprintf("%p%|*s\n", '-', cols, "");
    ret = sprintf("%p%|*s\n\n", '-', cols + strlen(header) - visible_strlen(header), header);

    for (i = 0; i < sizeof(_unarmed_styles); i++)
    {
      mixed * info;
      info = table("unarmed_combat")->query_unarmed_style_info(_unarmed_styles[i]);

      ret += sprintf("\t%35s %25s (%s)\n", "%^BOLD%^" + capitalize(info[0]) + "%^RESET%^", 
                "[" + percentage_bar(query_known_unarmed_combat_styles()[_unarmed_styles[i]]) + "]",
                "" + query_known_unarmed_combat_styles()[_unarmed_styles[i]] + "%");
    }

    ret += "\n" + line;
    ret += _LANG_UNARMED_LIST_FOOTER;
    ret += line;
    tell_object(this_object(), ret);
    return 1;
  }

  if (member_array(style, _unarmed_styles) != -1)
  {
    set_unarmed_combat_style(style);
  }
  else
  {
    notify_fail(_LANG_UNARMED_NOT_KNOWN);
    return 0;
  }

  return 1;
}

// This needs the attacker's object too..
// Iolo's localization system, neverbot 4/03
mixed *recalc_damage(int i)
{
  int hps;
  mixed local;

  hps = 0;

  local = defender->obtain_location();
  
  if (!local) 
    local = ({ 1, "" });
  
  // modifier by body localization
  hps = (int)((float)i * (float)local[0]);
    
  return ({hps, local[1]});
}

string * parse_messages(string * messages, string local, string att_name, string def_name)
{
  int i, j;
  string * result;

  result = ({ });

  for (i = 0; i < sizeof(messages); i++)
  {
    string * pieces;
    pieces = explode(messages[i], " ");
    for (j = 0; j < sizeof(pieces); j++)
    {
      switch(pieces[j]) {
        case "$defname":
          pieces[j] = def_name;
          break;
        case "$attname":
          pieces[j] = att_name;
          break;
        case "$local":
          pieces[j] = local;
          break;
      }
    }
    result += ({ trim(implode(pieces, " ")) });
  }

  return result;
}

// Iolo@RL localization system
// messages generated depending of the style used, neverbot 4/03
void write_message(int damage, string local, object att, object def)
{
  string place;
  string * msgs;

  if (!local || local == "") 
    place = "";
  else 
    place = "" + _LANG_UNARMED_LOCALIZATION_PREP + " " + local;
  
  if (sizeof(messages) != 3)  
    messages = table("unarmed_combat")->query_messages(current_unarmed_style, unarmed_ability, att);
  
  if (sizeof(messages) != 3)
  {
    tell_object(att, _LANG_UNARMED_CODE_ERROR);
    return;
  }
  
  // with strings, simple messages
  if (stringp(messages[0]))
  {
    msgs = messages;
  }
  else // if every element is a message list too
  {
    int i;
    i = random(sizeof(messages[0]));
    msgs = ({ messages[0][i], messages[1][i], messages[2][i], });
  }

  // change placeholders in combat messages with the real deal
  msgs = parse_messages(msgs, place, att->query_cap_name(), def->query_cap_name());

  tell_object(att, ATT + capitalize(msgs[0]) + ".\n");
  tell_object(def, DFF + capitalize(msgs[1]) + ".\n");
  tell_room(environment(att), capitalize(msgs[2]) + ".\n", ({ att, def }));
}

mixed * query_messages() { return messages; }
int set_messages(mixed * list)
{
  // non default message system only with npcs
  if (interactive(this_object()))
    return 0;
  
  // three messages
  if (sizeof(list) != 3)
    return 0;
  
  // if it is a list of lists but with different sizes
  if ( arrayp(list[0]) && 
    ( (sizeof(list[0]) != sizeof(list[1])) || (sizeof(list[1]) != sizeof(list[2])) ))
    return 0;

  messages = list;
  return 1;
}

mixed query_damage_dice(){ return ({ ovr_num, ovr_type }); }

void set_damage_dice(int numdie, int dietype)
{
  if ((numdie > 0) && (dietype > 0))  
  {
    ovr_num = numdie;
    ovr_type = dietype;
  }
  return;
}

// stats added
mixed * stats() 
{
  mixed * ret;
  ret = ({ 
           ({ "Known Unarmed Styles", known_unarmed_styles, }),
           ({ "Current Unarmed Style", current_unarmed_style, }),
           ({ "Num Unarmed Hits", num_unarmed_hits, }),
           ({ "Messages", messages, }),
       });
  return ret;
}
