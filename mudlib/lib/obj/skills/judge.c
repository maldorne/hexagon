// Judge (juzgar) — active skill: 'judge <item>'. Rates, with a word, how much
// harm a weapon carried does (one line for each of its attacks) or how well an
// armour or shield carried protects. Ported from the juzgar command of the RL
// muds (std/commands/juzgar.c of ancient-kingdoms, Timion): there it was a
// level table over the damage of the weapon or a fifth of the ac of the armour;
// here the scale is the one of the weapon and armour tables.
//
// The rating goes from 1 to 12. The better the skill, the less the verdict can
// be off: the word is shifted by up to (100 - ability) / 25 steps either way.
//
//   weapon  : 1 + the best damage of the attack (dice * faces + bonus + enchant) / 2
//   armour  : 2 + ac * 3 / 2
//   shield  : 2 + ac * 3

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

#define JUDGE_MAX_RATING 12

void setup()
{
  set_common_effect(1);

  // Display name + aliases; the learning key is SKILL_JUDGE.
  set_effect_name(_LANG_SKILL_JUDGE_NAME);
  set_aliases(_LANG_SKILL_JUDGE_ALIASES);

  set_gp_cost(0);
  set_lockout_time(2);

  set_target_type(TARGET_TYPE_ITEM);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_mounted = 1;
  allow_while_pacified = 1;
  allow_while_hidden = 1;
  allow_while_moving = 1;

  set_help_desc(_LANG_SKILL_JUDGE_HELP);

  set_start_phrases("", "");
  set_fast_casting(1);
  set_rounds(({ "do_judge" }));
}

/**
 * The word for a rating, shifted by how well the caster can tell.
 */
private string verdict(object caster, int rating)
{
  string * words;
  int spread;

  words = _LANG_SKILL_JUDGE_WORDS;
  spread = (100 - caster->query_skill_ability(SKILL_JUDGE)) / 25;
  rating += random(2 * spread + 1) - spread;

  if (rating < 1)
    rating = 1;
  if (rating > JUDGE_MAX_RATING)
    rating = JUDGE_MAX_RATING;

  return words[rating - 1];
}

private void judge_weapon(object caster, object item)
{
  mapping attacks;
  mixed * attack;
  string * names;
  string word, type_name;
  int i, best;

  attacks = item->query_attacks();
  names = map_indices(attacks);

  for (i = 0; i < sizeof(names); i++)
  {
    attack = attacks[names[i]];

    // the attack is ({ type, dice, faces, bonus })
    best = (int)attack[1] * (int)attack[2] + (int)attack[3] +
           (int)item->query_enchant();

    word = verdict(caster, 1 + best / 2);
    type_name = item->query_attack_type_name(attack[0]);

    tell_object(caster, _LANG_SKILL_JUDGE_WEAPON);
  }
}

private void judge_armour(object caster, object item)
{
  string word;
  int rating;

  if (item->query_shield())
    rating = 2 + (int)item->query_ac() * 3;
  else
    rating = 2 + (int)item->query_ac() * 3 / 2;

  word = verdict(caster, rating);
  tell_object(caster, _LANG_SKILL_JUDGE_ARMOUR);
}

int do_judge(object caster, mixed target, mixed out_range, int time, int quiet)
{
  if (!objectp(target) || environment(target) != caster)
  {
    tell_object(caster, _LANG_SKILL_JUDGE_NOT_CARRIED);
    return 0;
  }

  if (target->query_weapon())
    judge_weapon(caster, target);
  else if (target->query_armour() || target->query_shield())
    judge_armour(caster, target);
  else
    tell_object(caster, _LANG_SKILL_JUDGE_USELESS);

  return 1;
}
