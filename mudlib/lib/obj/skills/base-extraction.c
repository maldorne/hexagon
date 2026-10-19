// What the skills that take body parts from a corpse share (skinning, gutting,
// dismembering, pulling). The race of the dead says which parts its body has
// and the skill each one needs (see /lib/obj/death/body-parts.md); a skill
// takes only its own. Whoever knows the skill may use it, however they learnt
// it. A knife or a dagger has to be wielded, as in the original muds.
//
//   <verb> <corpse>              the first of the skill's parts still on it
//   <verb> <part> de <corpse>    that part ("from" / "of" in English)
//
// The roll gets easier the more the ability passes the part's minimum; a
// failure ruins the part, and it is gone from the body all the same.

#include <living/effects.h>
#include <living/skills.h>
#include <living/body-parts.h>
#include <basic/move.h>
#include <translations/weapon.h>
#include <language.h>

inherit BASE_EFFECT;

// the chance to take a part at the minimum ability, and its bounds
#define EXTRACT_BASE_CHANCE 50
#define EXTRACT_MIN_CHANCE  10
#define EXTRACT_MAX_CHANCE  95

private string extraction_skill;

/**
 * What every extraction skill shares; its setup calls this after its names.
 *
 * @param skill the skill id, the same as the file name
 */
void setup_extraction(string skill)
{
  extraction_skill = skill;

  set_common_effect(1);

  set_gp_cost(5);
  set_lockout_time(4);

  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_in_combat = 0;
  allow_while_mounted = 0;

  set_start_phrases(_LANG_SKILL_EXTRACT_START, _LANG_SKILL_EXTRACT_START_ROOM);
  set_fast_casting(1);
  set_rounds(({ "round_extract" }));
}

string query_extraction_skill() { return extraction_skill; }

// The corpse some words name, carried first and then in the room.
private object find_corpse(string words, object caster)
{
  mixed found;
  int i;

  if (!words || !strlen(words))
    return nil;

  found = find_match(words, caster);
  if (!arrayp(found))
    found = ({ });
  if (environment(caster) && arrayp(find_match(words, environment(caster))))
    found += find_match(words, environment(caster));

  for (i = 0; i < sizeof(found); i++)
    if (objectp(found[i]) && found[i]->query_corpse())
      return found[i];

  return nil;
}

// The part id some words name among the parts of a body, "" when none.
private string part_named(string words, mapping parts)
{
  object maker;
  string * ids;
  int i;

  maker = load_object(BODY_PART_OB);
  ids = map_indices(parts);
  words = lower_case(trim(words));

  for (i = 0; i < sizeof(ids); i++)
    if (member_array(words, maker->part_words(ids[i], parts[ids[i]])) != -1)
      return ids[i];

  return "";
}

// The least ability a part asks for.
private int part_minimum(mapping spec)
{
  return intp(spec[BODY_PART_MIN]) ? spec[BODY_PART_MIN] : 0;
}

/**
 * What the words ask for: the corpse and the part to take from it, or why it
 * cannot be done.
 *
 * @return ({ corpse, part id }), or the refusal as a string
 */
mixed resolve_extraction(string str, object caster)
{
  object corpse;
  mapping parts;
  string * separators, * ids;
  string part_words, id;
  int i, at;

  if (!str || !strlen(trim(str)))
    return _LANG_SKILL_EXTRACT_SYNTAX;

  str = trim(str);
  part_words = "";
  corpse = find_corpse(str, caster);

  // "<part> de <corpse>": the first separator that leaves a corpse after it
  separators = _LANG_SKILL_EXTRACT_SEPARATORS;
  for (i = 0; !corpse && i < sizeof(separators); i++)
  {
    at = strsrch(str, separators[i]);
    if (at <= 0)
      continue;

    corpse = find_corpse(str[at + strlen(separators[i]) ..], caster);
    if (corpse)
      part_words = str[0 .. at - 1];
  }

  if (!corpse)
    return _LANG_SKILL_EXTRACT_NO_CORPSE;

  if (!strlen(corpse->query_race_ob()))
    return _LANG_SKILL_EXTRACT_UNKNOWN_RACE;

  parts = corpse->query_body_parts();
  if (!map_sizeof(parts))
    return _LANG_SKILL_EXTRACT_NOTHING;

  if (strlen(part_words))
  {
    id = part_named(part_words, parts);
    if (!strlen(id))
      return _LANG_SKILL_EXTRACT_NO_SUCH_PART;
    if (parts[id][BODY_PART_SKILL] != extraction_skill)
      return _LANG_SKILL_EXTRACT_WRONG_SKILL;
    if (!corpse->query_parts_left(id))
      return _LANG_SKILL_EXTRACT_TAKEN;
  }
  else
  {
    // the first of this skill's parts still on the body
    id = "";
    ids = map_indices(parts);
    for (i = 0; i < sizeof(ids); i++)
    {
      if (parts[ids[i]][BODY_PART_SKILL] != extraction_skill)
        continue;
      if (corpse->query_parts_left(ids[i]))
      {
        id = ids[i];
        break;
      }
      // there is one, but taken: said if nothing else is left
      if (!strlen(id))
        id = "-";
    }

    if (id == "")
      return _LANG_SKILL_EXTRACT_NOTHING;
    if (id == "-")
      return _LANG_SKILL_EXTRACT_TAKEN;
  }

  if (caster->query_skill_ability(extraction_skill) < part_minimum(parts[id]))
    return _LANG_SKILL_EXTRACT_NOT_SKILLED;

  return ({ corpse, id });
}

// Why the caster cannot cut now, nil when they can.
private string blade_refusal(object caster)
{
  if (!check_weapon_type(caster, ({ KNIFE, DAGGER })))
    return _LANG_SKILL_EXTRACT_NO_BLADE;
  return nil;
}

mixed extra_checks(string str, object caster)
{
  mixed res;

  if (blade_refusal(caster))
    return blade_refusal(caster);

  res = resolve_extraction(str, caster);
  return stringp(res) ? res : 0;
}

int round_extract(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object corpse, part;
  mapping spec;
  mixed res;
  string id, file, part_name, corpse_name;
  int chance;

  if (blade_refusal(caster))
  {
    tell_object(caster, blade_refusal(caster));
    return 0;
  }

  // everything is asked again: the body may be gone, or a part taken
  res = resolve_extraction(stringp(target) ? target : "", caster);
  if (stringp(res))
  {
    tell_object(caster, res);
    return 0;
  }

  corpse = res[0];
  id = res[1];
  spec = corpse->query_body_parts()[id];
  part_name = load_object(BODY_PART_OB)->part_name(id, spec);
  corpse_name = corpse->query_name() ? corpse->query_name() : "";

  chance = EXTRACT_BASE_CHANCE +
           caster->query_skill_ability(extraction_skill) - part_minimum(spec);
  if (chance < EXTRACT_MIN_CHANCE)
    chance = EXTRACT_MIN_CHANCE;
  if (chance > EXTRACT_MAX_CHANCE)
    chance = EXTRACT_MAX_CHANCE;

  corpse->add_part_taken(id);

  if (random(100) >= chance)
  {
    tell_object(caster, _LANG_SKILL_EXTRACT_RUINED);
    tell_room(environment(caster), _LANG_SKILL_EXTRACT_RUINED_ROOM, ({ caster }));
    return 1;
  }

  file = stringp(spec[BODY_PART_FILE]) ? spec[BODY_PART_FILE] : BODY_PART_OB;
  part = clone_object(file);
  if (!part)
  {
    tell_object(caster, _LANG_SKILL_EXTRACT_BROKEN);
    return 0;
  }

  part->set_part(id, spec);
  part->set_source(corpse);
  part->set_freshness(corpse->query_freshness());

  if (part->move(caster) != MOVE_OK)
    part->move(environment(caster));

  tell_object(caster, _LANG_SKILL_EXTRACT_DONE);
  tell_room(environment(caster), _LANG_SKILL_EXTRACT_DONE_ROOM, ({ caster }));

  // a job may reward its trade (a hunter's skins)
  if (caster->query_job_ob())
    catch(load_object(caster->query_job_ob())->on_body_part_taken(caster, part, corpse));
  return 1;
}
