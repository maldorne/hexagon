// Abilities of a guild or a job: the skills and spells it teaches by level
// and the ones it only relates to itself. Inherited by /lib/guild.c and
// /lib/job.c, which wrap the neutral names here in their own (add_guild_skill,
// add_job_skill...); the level a member needs is the guild level for a guild
// and the job level for a job, always passed in by the wrapper.

#include <living/effects.h>

// the skills taught: skill id -> ({ level needed, least ability the member
// starts with })
private mapping ability_skills;
// the spells taught: spell object path -> level needed; members learn them
// with aprender in the hall
private mapping ability_spells;
// the spells and skills tied but not taught (learned through quests, items...):
// spell object path -> 1, skill id -> 1
private mapping related_ability_spells, related_ability_skills;
// the power given in some of those spells, spell object path -> 1 minor,
// 2 neutral, 3 major; see add_ability_spell
private mapping ability_spell_powers;

/**
 * Starts the mappings; the create of the inheritor calls it first.
 */
static void create_abilities()
{
  ability_skills = ([ ]);
  ability_spells = ([ ]);
  related_ability_spells = ([ ]);
  related_ability_skills = ([ ]);
  ability_spell_powers = ([ ]);
}

/**
 * Teaches a skill from a level on.
 *
 * @param  id       the skill id, see <living/skills.h>
 * @param  level    level needed to know it
 * @param  ability  least value the member has in it when taught (an ability
 *                  for a percentage skill, a level for a levelled one)
 */
static void add_ability_skill(string id, int level, varargs int ability)
{
  ability_skills[id] = ({ level, ability });
}

/**
 * The skills taught: id -> ({ level needed, least ability }).
 */
mapping query_ability_skills() { return ability_skills; }

/**
 * Teaches a spell from a level on.
 *
 * @param  path   the spell object, inheriting BASE_EFFECT as a spell
 * @param  level  level needed to learn it
 * @param  power  optional EFFECT_CAT_MINOR, EFFECT_CAT_NEUTRAL or
 *                EFFECT_CAT_MAJOR: the power the members get in this spell,
 *                instead of the one their categories give; it also lets them
 *                cast it without knowing any of its categories
 */
static void add_ability_spell(string path, int level, varargs string power)
{
  ability_spells[path] = level;

  if (power == EFFECT_CAT_MINOR)
    ability_spell_powers[path] = 1;
  else if (power == EFFECT_CAT_NEUTRAL)
    ability_spell_powers[path] = 2;
  else if (power == EFFECT_CAT_MAJOR)
    ability_spell_powers[path] = 3;
  else
    ability_spell_powers[path] = nil;
}

/**
 * The spells taught: path -> level needed.
 */
mapping query_ability_spells() { return ability_spells; }

/**
 * The power given in one spell when it is set apart from the spell's
 * categories (see add_ability_spell).
 * @param  path  the spell object
 * @return 0 when none is set, else 1 minor, 2 neutral, 3 major
 */
int query_ability_spell_power(string path)
{
  return ability_spell_powers[path] ? ability_spell_powers[path] : 0;
}

/**
 * Ties a skill without teaching it: it may be used, but it is not offered by
 * aprender, not granted and not listed among the taught ones (it is learned
 * through quests, items...).
 * @param  id  the skill id, compared exactly
 */
static void add_related_ability_skill(string id) { related_ability_skills[id] = 1; }

/**
 * Ties a spell without teaching it, as add_related_ability_skill does for a
 * skill.
 * @param  path  the spell object, full path with .c, compared exactly
 */
static void add_related_ability_spell(string path) { related_ability_spells[path] = 1; }

/**
 * The ids of the skills tied but not taught.
 */
string * query_related_ability_skills() { return map_indices(related_ability_skills); }

/**
 * The paths of the spells tied but not taught.
 */
string * query_related_ability_spells() { return map_indices(related_ability_spells); }

/**
 * Whether an effect is taught, at any level. Related effects are not taught,
 * see query_allows.
 * @param  key  the spell's path (with .c) or the skill's id, see
 *              query_effect_key in /lib/effects/base_effect.c
 * @return 1 when a skill or a spell was added with that key
 */
int query_teaches(string key)
{
  return ability_spells[key] != nil || ability_skills[key] != nil;
}

/**
 * Whether those who belong may use an effect that is not common: it is taught
 * or related.
 * @param  key  the spell's path (with .c) or the skill's id
 * @return 1 when taught or related
 */
int query_allows(string key)
{
  return query_teaches(key) || related_ability_spells[key] != nil ||
         related_ability_skills[key] != nil;
}

/**
 * A line added to the help of an effect for those who belong, e.g. what the
 * guild or job gives for using it. A guild or a job with something to say
 * masks it.
 * @param  key     the spell's path (with .c) or the skill's id
 * @param  player  who reads the help
 * @return the text, ending in a newline, or nil for none
 */
string query_effect_note(string key, object player)
{
  return nil;
}

/**
 * Gives a member every skill their level allows.
 *
 * Idempotent: a skill already known is left as it is, only raised to the
 * least value promised.
 * @param  player  the member
 * @param  level   the guild or job level the member has
 */
void grant_ability_skills(object player, int level)
{
  string * ids;
  int i, ability;

  ids = map_indices(ability_skills);

  for (i = 0; i < sizeof(ids); i++)
  {
    if (ability_skills[ids[i]][0] > level)
      continue;

    player->add_known_skill(ids[i]);

    ability = ability_skills[ids[i]][1];
    if (player->query_known_skill(ids[i]) &&
        player->query_skill_value(ids[i]) < ability)
      player->set_skill_value(ids[i], ability, 1);
  }
}

/**
 * The id a spell is known by: its file name without the extension.
 * @param  path  the spell object
 * @return the id
 */
string query_spell_id(string path)
{
  string * parts;

  parts = explode(path, "/");
  path = parts[sizeof(parts) - 1];
  if (strlen(path) > 2 && path[strlen(path) - 2..] == ".c")
    path = path[0..strlen(path) - 3];
  return path;
}

/**
 * Gives a member every spell their level allows and they do not know yet.
 * Asking for the level now, not when it was reached, lets a spell moved to an
 * earlier level reach whoever is past it already.
 *
 * @param  player  the member
 * @param  top     the guild or job level the member has
 * @return the paths of the spells learned, in the order of their levels
 */
string * teach_ability_spells(object player, int top)
{
  string * paths, * learned;
  int i, level;

  paths = map_indices(ability_spells);
  learned = ({ });

  for (level = 0; level <= top; level++)
    for (i = 0; i < sizeof(paths); i++)
    {
      if (ability_spells[paths[i]] != level ||
          player->query_spell(query_spell_id(paths[i])))
        continue;

      player->add_spell(query_spell_id(paths[i]), paths[i]);
      learned += ({ paths[i] });
    }

  return learned;
}

/**
 * The spell that comes next after a level: the one taught at the lowest level
 * above it.
 *
 * @param  top  the guild or job level the member has
 * @return ({ path, level }), or nil when nothing more is taught
 */
mixed * query_next_ability_spell(int top)
{
  string * paths;
  string next_path;
  int i, next_level;

  paths = map_indices(ability_spells);
  next_path = nil;
  next_level = 0;

  for (i = 0; i < sizeof(paths); i++)
  {
    if (ability_spells[paths[i]] <= top)
      continue;
    if (next_level && ability_spells[paths[i]] >= next_level)
      continue;

    next_level = ability_spells[paths[i]];
    next_path = paths[i];
  }

  return next_path ? ({ next_path, next_level }) : nil;
}
