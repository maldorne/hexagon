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
                    varargs object where, int critical)
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
      if (attack_type >= sizeof(AC_TYPES))
      {
        // any other elemental type without a message of its own
        msg_me  += _LANG_WEAPONS_ELEMENTAL_MSG_ME;
        msg_him += _LANG_WEAPONS_ELEMENTAL_MSG_HIM;
        msg_env += _LANG_WEAPONS_ELEMENTAL_MSG_ENV;
      }
      else
      {
        msg_me  += _LANG_WEAPONS_BLUNT_MSG_ME;
        msg_him += _LANG_WEAPONS_BLUNT_MSG_HIM;
        msg_env += _LANG_WEAPONS_BLUNT_MSG_ENV;
      }
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

  // The wound describes what this hit did: its share of the victim's hit
  // points, or a mortal wound when it killed. The hit is already applied when
  // this is called, so the victim's hit points are those it is left with.
  i = defender->query_max_hp() > 0 ? (100 * damage) / defender->query_max_hp() : 0;

  // The wound always shows on a hit that matters (a critical, a killing blow or
  // a quarter or more of the victim's hit points); on the small ones, only now
  // and then, so the same tail does not close every line of a fight.
  if (damage <= 0)
    aux = "";
  else if (!critical && defender->query_hp() >= 0 &&
           i < HIT_MESSAGE_WOUND_ALWAYS_PERCENT &&
           random(HIT_MESSAGE_WOUND_SMALL_ODDS))
    aux = "";
  else if (defender->query_hp() < 0)
    aux = (attack_type == SLASHING) ? _LANG_WEAPONS_SLASHING_MSG_7 :
                                      _LANG_WEAPONS_NON_SLASHING_MSG_7;
  else if (attack_type == SLASHING)
  {
    switch (i)
    {
      case 0..7:   aux = _LANG_WEAPONS_SLASHING_MSG_6; break;
      case 8..14:  aux = _LANG_WEAPONS_SLASHING_MSG_5; break;
      case 15..24: aux = _LANG_WEAPONS_SLASHING_MSG_4; break;
      case 25..34: aux = _LANG_WEAPONS_SLASHING_MSG_3; break;
      case 35..49: aux = _LANG_WEAPONS_SLASHING_MSG_2; break;
      default:     aux = _LANG_WEAPONS_SLASHING_MSG_1; break;
    }
  }
  else
  {
    switch (i)
    {
      case 0..7:   aux = _LANG_WEAPONS_NON_SLASHING_MSG_6; break;
      case 8..14:  aux = _LANG_WEAPONS_NON_SLASHING_MSG_5; break;
      case 15..24: aux = _LANG_WEAPONS_NON_SLASHING_MSG_4; break;
      case 25..34: aux = _LANG_WEAPONS_NON_SLASHING_MSG_3; break;
      case 35..49: aux = _LANG_WEAPONS_NON_SLASHING_MSG_2; break;
      default:     aux = _LANG_WEAPONS_NON_SLASHING_MSG_1; break;
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
                   varargs object where, // armour piece that receives the hit
                   int critical)         // whether it was a critical hit
{
  mixed *messages;

  messages = query_message(damage, attack_type, localization, attacker, defender,
                           where, critical);

  tell_room(environment(attacker), messages[2], ({attacker,defender}));
  tell_object(attacker, ATT + messages[0]);
  tell_object(defender, DFF + messages[1]);

  return;
} /* _write_message */

// extra line after a hit, for the elemental damage a weapon adds to it.
// messages is the optional ({ for the attacker, for the defender, for the room })
// of the weapon, with the tokens $weapon$ (the weapon, already with its owner),
// $attacker$ and $defender$; without it the line depends on the element.
void write_extra_damage_message(int attack_type,      // elemental type of the extra damage
                         object attacker,
                         object defender,
                         object weapon,
                         varargs mixed * messages)
{
  string * verbs, * texts, mine, others;
  int i;

  if (!messages || sizeof(messages) < 3)
  {
    verbs = _LANG_WEAPONS_EXTRA_DAMAGE_VERBS[attack_type - sizeof(AC_TYPES)];
    messages = ({ "$weapon$ " + verbs[0] + ".", "$weapon$ " + verbs[1] + ".",
                  "$weapon$ " + verbs[0] + "." });
  }

  mine = _LANG_WEAPONS_EXTRA_DAMAGE_WEAPON_ME;
  others = _LANG_WEAPONS_EXTRA_DAMAGE_WEAPON_OTHER;

  texts = allocate(3);
  for (i = 0; i < 3; i++)
  {
    texts[i] = replace(messages[i], ({ "$attacker$", attacker->query_cap_name(),
                                      "$defender$", defender->query_cap_name(),
                                      "$weapon$", (i == 0) ? mine : others }));
    texts[i] += "\n";
  }

  tell_room(environment(attacker), texts[2], ({attacker,defender}));
  tell_object(attacker, texts[0]);
  tell_object(defender, texts[1]);
}

/**
 * What detect magic reads in a weapon: one exact sentence for each extra
 * damage (its own text, or the one of its element with the dice), for the
 * enchant and for what a spell put on it, with the level of who cast it.
 *
 * @param  weapon  the weapon
 * @return the sentences, none when it holds no magic
 */
string * query_magic_descriptions(object weapon)
{
  string * lines, * attacks, * levelled, text;
  mixed * entries, * mark;
  mapping extras, marks;
  int i, j, enchant;

  lines = ({ });
  extras = weapon->query_extra_damage();
  attacks = map_indices(extras);

  for (i = 0; i < sizeof(attacks); i++)
  {
    entries = extras[attacks[i]];
    for (j = 0; j < sizeof(entries); j++)
    {
      if (sizeof(entries[j]) > 5 && stringp(entries[j][5]))
        text = entries[j][5];
      else
        text = _LANG_WEAPONS_EXTRA_DAMAGE_DETECT[entries[j][0] - sizeof(AC_TYPES)] +
               " (" + entries[j][1] + "d" + entries[j][2] +
               (entries[j][3] ? (entries[j][3] > 0 ? "+" : "") + entries[j][3] : "") +
               " " + _LANG_WEAPONS_DETECT_DAMAGE_OF + ")";
      lines += ({ text + "." });
    }
  }

  enchant = weapon->query_enchant();
  if (enchant > 0)
    lines += ({ _LANG_WEAPONS_DETECT_ENCHANT_PLUS + enchant + "." });
  else if (enchant < 0)
    lines += ({ _LANG_WEAPONS_DETECT_ENCHANT_MINUS + enchant + "." });

  marks = weapon->query_magic_marks();
  levelled = map_indices(marks);
  for (i = 0; i < sizeof(levelled); i++)
  {
    mark = marks[levelled[i]];
    text = mark[2]["text"];
    if (!text)
      continue;
    lines += ({ text + " (" + _LANG_WEAPONS_DETECT_LEVEL + mark[1] + ")." });
  }

  return lines;
}
