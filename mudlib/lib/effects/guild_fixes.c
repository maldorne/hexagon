/*
 * Per-guild damage modifiers:
 *
 * Mages and clerics cast differently, so each spell provides a
 * wiz_fix_damage or cleric_fix_damage function respectively.
 *
 * Bards have no spells of their own; they generally use the mage ones, so
 * wiz_fix_damage must check the caster's guild.
 */

#include <language.h>

#define BASESTAT 17         /* Spell damage modifier based on the guild's    */
                            /*  primary stat (int, wis, cha)                 */
#define LUCK_SAVE 12        /* Damage modifier from bad luck                 */

inherit "/lib/effects/environment_fixes.c";

/**
 * Whether the caster belongs to the guild at a path (with or without ".c").
 */
private int is_member_of(object caster, string guild_path)
{
  string current;

  current = caster->query_guild_ob();
  if (!current)
    return 0;

  if (strlen(current) > 2 && current[strlen(current) - 2 ..] == ".c")
    current = current[.. strlen(current) - 3];
  if (strlen(guild_path) > 2 && guild_path[strlen(guild_path) - 2 ..] == ".c")
    guild_path = guild_path[.. strlen(guild_path) - 3];

  return current == guild_path;
}

/**
 * Why a guild withholds its powers from the caster, if it does.
 *
 * The skills a guild teaches call it from extra_checks. A skill that only the
 * members of one guild may use (the guild's skills stay with whoever leaves it)
 * passes the guild's path: a caster outside it gets the guild's non-member
 * message. Then, or without a path, the caster's own guild may still withhold
 * its powers (a paladin who turned evil).
 *
 * @param   caster      who uses the skill
 * @param   guild_path  guild whose members only may use it, if any
 * @return  the message to show, or nil when nothing is withheld
 */
string query_guild_refusal(object caster, varargs string guild_path)
{
  string path;
  object guild;

  // an outsider gets the message of the guild the skill belongs to
  if (guild_path && !is_member_of(caster, guild_path))
  {
    guild = load_object(guild_path);
    if (!guild)
      return nil;

    return guild->query_non_member_refusal(caster);
  }

  path = caster->query_guild_ob();
  if (!path)
    return nil;

  guild = load_object(path);
  if (!guild)
    return nil;

  return guild->query_powers_refusal(caster);
}

int warrior_fix_damage(object caster, object victim, int damage, string type)
{
  // Strength-based damage modifier (disabled):
  // damage = (damage * (int)caster->query_str()) / BASESTAT;

  // if((damage < 0) && (random(victim->query_con()) > LUCK_SAVE))
  //    damage = damage / 2;

  // Environment-type modifier:
  damage = implement_environment_mods(caster, damage);

  return damage;
}


int bard_fix_damage(object caster, object victim, int damage, string type)
{
  // Charisma-based damage modifier:
  damage = (damage * (int)caster->query_cha()) / BASESTAT;
  // Removed (Eressea 2/2002): after an xxx_fix_damage we always deal the
  // damage through a spell_damage, which would subtract the resistance
  // percentage twice.
  // damage = (damage * (100 - (int)calc_res(victim, type))) / 100;

  // LUCK! Taniwha 1995
  // if((damage < 0) && (random(victim->query_con()) > LUCK_SAVE))
  //    damage = damage / 2;

  // Environment-type modifier:
  damage = implement_environment_mods(caster, damage);

  return damage;
}

int wiz_fix_damage(object caster, object victim, int damage, string type)
{
  // If the spell owner is a bard, apply the bard's own fix_damage.
  if (caster->query_guild_name() == "Bardo")
      return bard_fix_damage(caster, victim, damage, type);

  // Intelligence-based damage modifier:
  damage = (damage * (int)caster->query_int()) / BASESTAT;
  // Removed (Eressea 2/2002), see bard_fix_damage.
  // damage = (damage * (100 - (int)calc_res(victim, type))) / 100;

  // LUCK! Taniwha 1995
  // if ((damage < 0) && (random(victim->query_con()) > LUCK_SAVE))
  //    damage = damage / 2;

  // Environment-type modifier:
  damage = implement_environment_mods(caster, damage);

  return damage;
}

int cleric_fix_damage(object caster, object victim, int damage, string type)
{
  // Important for this to work: query_guild_name must return the names
  // capitalised.
  int align;

  // Paladin spells do not harm the good, nor heal the evil.
  if((string)caster->query_guild_name() == "Paladin") {
    align = (int)victim->query_align();

    /* harm good */
    if((damage < 0) && (align < 0)) return 0;
    /* heal evil */
    if((damage > 0) && (align > 0)) return 0;
  }

  // Antipaladin spells do not harm the evil, nor heal the good.
  if((string)caster->query_guild_name() == "Antipaladin") {
    align = (int)victim->query_align();
    /* harm evil */
    if((damage < 0) && (align > 0)) return 0;
    /* heal good */
    if((damage > 0) && (align < 0)) return 0;
  }

  // Wisdom-based damage modifier:
  damage = (damage * (int)caster->query_wis()) / BASESTAT;
  // Removed (Eressea 2/2002), see bard_fix_damage.
  // damage = (damage * (100-(int)calc_res(victim,type)))/100;

  // LUCK! Taniwha 1995
  // if((damage < 0) && (random(victim->query_con()) > LUCK_SAVE))
  //    damage = damage / 2;

  // Environment-type modifier:
  damage = implement_environment_mods(caster, damage);

  return damage;
}

/* Whether object ob has the right alignment for what it is trying to do. */
int wrong_alignment(object ob) {
  switch( (string)ob->query_guild_name() )
  {
  case "Paladin":
    if ( (int)ob->query_align() > 0 )
        return 1;
    break;
  case "Antipaladin":
    if ( (int)ob->query_align() < 0 )
        return 1;
    break;
  default:
    break;
  }

  return 0;
}

/* For the (good)evil (anti-)paladins who try to use their abilities. */
int punish_align(object ob)
{
  tell_object(ob, _LANG_EFFECT_PUNISH_ALIGN);

  ob->set_xp(0);
  return 1;
}
