// Pray (rezar) — 'pray <what>': asks one's deity for a favour. Ported from the
// rezar command of the RL muds (std/commands/rezar.c of ancient-kingdoms).
// 10/2026 - Moved to the mudlib: a fixed skill every worshipper is given, and
//           the prayers, their odds and their effects belong to the deity
//           (see /lib/deity.c and /lib/room/temple/temple.md).
//
// The skill only dispatches: it checks the caster has a deity and stands where
// that deity is worshipped -- a temple of it (room or location) or an altar or
// statue dedicated to it (the prop's overrides.deity) -- finds the prayer the
// words ask for, and lets the deity answer it.

#include <living/effects.h>
#include <living/skills.h>
#include <room/location.h>
#include <room/prop.h>
#include <deities.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  // Display name + aliases; the learning key is SKILL_PRAY.
  set_effect_name(_LANG_SKILL_PRAY_NAME);
  set_aliases(_LANG_SKILL_PRAY_ALIASES);

  set_gp_cost(0);
  set_lockout_time(5);

  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_mounted = 1;

  set_help_desc(_LANG_SKILL_PRAY_HELP);

  set_start_phrases(_LANG_SKILL_PRAY_START, _LANG_SKILL_PRAY_START_ROOM);

  set_fast_casting(1);
  set_rounds(({ "do_pray" }));
}

/**
 * The caster's deity object, nil if they have none.
 */
private object deity_of(object caster)
{
  mixed path;

  path = caster->query_deity_ob();
  if (!stringp(path) || !strlen(path))
    return nil;

  return load_object(path);
}

/**
 * The deities the place the caster stands in is given to: its temple's, and
 * the ones of its dedicated altars and statues.
 */
private string * place_deities(object env)
{
  string * ret, * types;
  mapping * insts;
  object temple, props;
  mixed deity;
  int i, j;

  ret = ({ });
  if (!env)
    return ret;

  // a temple room answers itself, a location through its temple component
  if (env->query_location())
    temple = env->query_component_by_type(LOCATION_COMPONENT_TEMPLE);
  else if (env->query_temple())
    temple = env;

  if (temple)
  {
    deity = temple->query_temple_deity();
    if (stringp(deity) && strlen(deity))
      ret += ({ deity });
  }

  props = env->query_location() ?
          env->query_component_by_type(LOCATION_COMPONENT_PROPS) : nil;
  if (!props)
    return ret;

  types = DEITY_PROP_TYPES;
  for (i = 0; i < sizeof(types); i++)
  {
    insts = props->query_instances_by_type(types[i]);
    for (j = 0; j < sizeof(insts); j++)
    {
      if (!mappingp(insts[j][PROP_FIELD_OVERRIDES]))
        continue;
      deity = insts[j][PROP_FIELD_OVERRIDES][DEITY_PROP_OVERRIDE];
      if (stringp(deity) && strlen(deity))
        ret += ({ deity });
    }
  }

  return ret;
}

/**
 * Whether the caster stands where their own deity is worshipped.
 */
private int at_own_place(object caster)
{
  return member_array(caster->query_deity_ob(),
                      place_deities(environment(caster))) != -1;
}

/** The syntax, naming the prayers the deity hears. */
private string syntax(object deity)
{
  return deity->fill_text(replace_string(_LANG_SKILL_PRAY_SYNTAX, "$prayers$",
                          implode(deity->query_prayer_names(), ", ")));
}

mixed extra_checks(string str, object caster)
{
  object deity;

  deity = deity_of(caster);
  if (!deity)
    return _LANG_SKILL_PRAY_NO_DEITY;

  if (caster->query_time_remaining(DEITY_PRAY_CURSE_PROP))
    return _LANG_SKILL_PRAY_CURSED;

  // only where one's own deity is worshipped: anywhere else it could take
  // offence
  if (!at_own_place(caster))
    return deity->fill_text(_LANG_SKILL_PRAY_WRONG_PLACE);

  if (!str || !strlen(trim(str)))
    return syntax(deity);

  if (!deity->query_prayer_for(str))
  {
    if (deity->query_refused_prayer(str))
      return deity->fill_text(_LANG_SKILL_PRAY_REFUSED);

    return syntax(deity);
  }

  return nil;
}

int do_pray(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object deity;
  string prayer;

  deity = deity_of(caster);
  if (!deity || !stringp(target))
    return 0;

  prayer = deity->query_prayer_for(target);
  if (!prayer)
    return 0;

  return deity->answer_prayer(caster, prayer) == DEITY_PRAYER_HEARD;
}
