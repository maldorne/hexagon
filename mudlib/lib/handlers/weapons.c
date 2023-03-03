// *************************************************
//   old weapon_table.c
// *************************************************

#include <living/combat.h>
#include <translations/combat.h>
#include <language.h>

mixed query_message(int damage, 
                    int attack_type,
                    string localization, 
                    object attacker, 
                    object defender,
                    varargs object where)
{
  string msg_me, msg_him, msg_env, aux;
  int relative, i;

  relative = 1;
  msg_me = msg_him = msg_env = "";
  msg_him = msg_env = attacker->query_cap_name();
 
  // 1. you pierce

  switch (attack_type) 
  {
    case SLASHING:
      msg_me  += _LANG_WEAPONS_SLASH_MSG_ME;
      msg_him += _LANG_WEAPONS_SLASH_MSG_HIM;
      msg_env += _LANG_WEAPONS_SLASH_MSG_ENV;
      break;
    case PIERCING: 
      msg_me  += _LANG_WEAPONS_PIERCE_MSG_ME;
      msg_him += _LANG_WEAPONS_PIERCE_MSG_HIM;
      msg_env += _LANG_WEAPONS_PIERCE_MSG_ENV;
      break;
    case BLUNT:     
      msg_me  += _LANG_WEAPONS_BLUNT_MSG_ME;
      msg_him += _LANG_WEAPONS_BLUNT_MSG_HIM;
      msg_env += _LANG_WEAPONS_BLUNT_MSG_ENV;
      break;
    case FIRE:     
      msg_me  += _LANG_WEAPONS_FIRE_MSG_ME;
      msg_him += _LANG_WEAPONS_FIRE_MSG_HIM;
      msg_env += _LANG_WEAPONS_FIRE_MSG_ENV;
      relative = 0;
      break;
    case COLD:     
      msg_me  += _LANG_WEAPONS_COLD_MSG_ME;
      msg_him += _LANG_WEAPONS_COLD_MSG_HIM;
      msg_env += _LANG_WEAPONS_COLD_MSG_ENV;
      relative = 0;
      break;
    default:
      msg_me  += _LANG_WEAPONS_BLUNT_MSG_ME;
      msg_him += _LANG_WEAPONS_BLUNT_MSG_HIM;
      msg_env += _LANG_WEAPONS_BLUNT_MSG_ENV;
      break;
  }

  // 2. you pierce john

  msg_me  += _LANG_WEAPONS_WHO_ME;
  msg_him += _LANG_WEAPONS_WHO_HIM;
  msg_env += _LANG_WEAPONS_WHO_ENV;

  // if we have the object that hits
  if (where)
  {
    if (relative)
    {
      // 3. you pierce john in his armour

      msg_me  += _LANG_WEAPONS_WHERE_RELATIVE_ME;
      msg_him += _LANG_WEAPONS_WHERE_RELATIVE_HIM;
      msg_env += _LANG_WEAPONS_WHERE_RELATIVE_ENV;
    }
    else
    {
      // 3. you burn john's armour

      msg_me  += _LANG_WEAPONS_WHERE_ME;
      msg_him += _LANG_WEAPONS_WHERE_HIM;
      msg_env += _LANG_WEAPONS_WHERE_ENV;
    }
  }
  // do not have the object but we have the localization
  else if (localization && (localization != ""))
  {
    // 3. you pierce john in his arm

    msg_me  += _LANG_WEAPONS_LOC_ME;
    msg_him += _LANG_WEAPONS_LOC_HIM;
    msg_env += _LANG_WEAPONS_LOC_ENV;
  }
  // do not have object nor localization
  // else
  // {
  //   msg_me  += _LANG_WEAPONS_NO_LOC_ME;
  //   msg_env += _LANG_WEAPONS_NO_LOC_ENV;
  // }

  aux = "";

  if (relative)
  {
    if (damage <= 0) 
      aux = _LANG_WEAPONS_NO_EFFECT;
    else 
      switch (damage) 
      {
        case 1..4:   aux = _LANG_WEAPONS_WEAKLY;                 break;
        case 5..8:   aux = _LANG_WEAPONS_WITH_LITTLE_FORCE;      break;
        case 9..12:  aux = _LANG_WEAPONS_NORMAL;                 break;
        case 13..16: aux = _LANG_WEAPONS_WITH_FORCE;             break;
        case 17..21: aux = _LANG_WEAPONS_WITH_MUCH_FORCE;        break;
        case 22..28: aux = _LANG_WEAPONS_VIOLENTLY;              break;
        case 29..60: aux = _LANG_WEAPONS_INCREDIBLE_FORCE;       break;
        default:     aux = _LANG_WEAPONS_SUPERHUMAN_FORCE;       break;
      }
  }

  // 4. you pierce john in his armour weakly

  msg_me  += aux;
  msg_him += aux;
  msg_env += aux;

  // relative damage
  i = (100 * (defender->query_hp() - damage)) / defender->query_max_hp();
  
  if (damage <= 0) 
   aux = "";
  else switch (attack_type) 
  {
    case SLASHING: 
      switch (i) 
      {
        case 0..10 :  aux = _LANG_WEAPONS_SLASHING_MSG_1; break;
        case 11..25:  aux = _LANG_WEAPONS_SLASHING_MSG_2; break;
        case 26..40:  aux = _LANG_WEAPONS_SLASHING_MSG_3; break;
        case 41..60:  aux = _LANG_WEAPONS_SLASHING_MSG_4; break;
        case 61..80:  aux = _LANG_WEAPONS_SLASHING_MSG_5; break;
        case 81..100: aux = _LANG_WEAPONS_SLASHING_MSG_6; break;
        default:      aux = _LANG_WEAPONS_SLASHING_MSG_7; break;
      }
    default: 
      switch (i) 
      {
        case 0..10 :  aux = _LANG_WEAPONS_NON_SLASHING_MSG_1; break;
        case 11..25:  aux = _LANG_WEAPONS_NON_SLASHING_MSG_2; break;
        case 26..40:  aux = _LANG_WEAPONS_NON_SLASHING_MSG_3; break;
        case 41..60:  aux = _LANG_WEAPONS_NON_SLASHING_MSG_4; break;
        case 61..80:  aux = _LANG_WEAPONS_NON_SLASHING_MSG_5; break;
        case 81..100: aux = _LANG_WEAPONS_NON_SLASHING_MSG_6; break;
        default:      aux = _LANG_WEAPONS_NON_SLASHING_MSG_7; break;
      }
  }

  // 5. you pierce john in his armour weakly, making only scratches

  msg_me  += aux + ".\n";
  msg_him += aux + ".\n";
  msg_env += aux + ".\n";

  return ({ msg_me, msg_him, msg_env });  
}

void write_message(int damage,           // damage from the hit
                   int attack_type,      // attack type (slashing, etc)
                   string localization,  // localization in the body
                   object attacker,      // 
                   object defender,      // 
                   varargs object where) // armour piece that receives the hit
{
  mixed *messages;
  if (where)
    messages = query_message(damage, attack_type, localization, attacker, defender, where);
  else
    messages = query_message(damage, attack_type, localization, attacker, defender);  

  tell_room(environment(attacker), messages[2], ({attacker,defender}));
  tell_object(attacker, ATT + messages[0]);
  tell_object(defender, DFF + messages[1]);

  return;
} /* _write_message */
