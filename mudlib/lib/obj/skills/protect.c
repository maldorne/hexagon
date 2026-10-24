// Protect (proteger) -- active skill: 'protect <someone>' / 'protect off
// [<someone>]'. The character stands by someone in the same place, so blows
// aimed at them may fall on the character instead (query_protect_valid in
// /lib/living/combat.c decides each blow). Was the protect and unprotect
// commands of /lib/living/combat.c (dw, Radix).
//
// Whoever is protected must consent to it (the protect consent); every NPC
// consents by default. An NPC uses it through cast_effect with the name.

#include <living/effects.h>
#include <living/skills.h>
#include <living/consents.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  set_common_effect(1);

  // Display name + aliases; the learning key is SKILL_PROTECT.
  set_effect_name(_LANG_SKILL_PROTECT_NAME);
  set_aliases(_LANG_SKILL_PROTECT_ALIASES);
  set_effect_intent(EFFECT_INTENT_HELP);

  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_mounted = 1;
  allow_while_hidden = 1;
  allow_in_combat = 1;

  set_help_desc(_LANG_SKILL_PROTECT_HELP);

  set_start_phrases("", "");
  set_fast_casting(1);
  set_rounds(({ "do_protect" }));
}

/**
 * Whether the words ask to stop protecting, and who: the words after "off",
 * "" for everyone protected in the place.
 *
 * @return nil when the words do not start with an off word
 */
private string off_target(string str)
{
  string * words;

  words = explode(lower_case(trim(str)), " ");
  if (!sizeof(words) ||
      member_array(words[0], _LANG_SKILL_PROTECT_OFF_WORDS) == -1)
    return nil;

  return implode(words[1..], " ");
}

mixed extra_checks(string str, object caster)
{
  object ob;

  if (!str || !strlen(trim(str)))
    return _LANG_SKILL_PROTECT_SYNTAX;

  if (off_target(str))
    return nil;

  ob = find_one_match(str, environment(caster), 1);
  if (!ob || !living(ob))
    return _LANG_SKILL_PROTECT_WHO;
  if (ob == caster)
    return _LANG_SKILL_PROTECT_SELF;

  return nil;
}

/**
 * Stops protecting one living, telling those involved.
 */
private void stop_protecting(object caster, object ob)
{
  ob->remove_protector(caster);

  if (environment(ob) == environment(caster))
  {
    tell_object(ob, _LANG_SKILL_PROTECT_UNPROTECT_THEM_HERE);
    tell_room(environment(caster), _LANG_SKILL_PROTECT_UNPROTECT_ROOM,
              ({ ob, caster }));
  }
  else
    tell_object(ob, _LANG_SKILL_PROTECT_UNPROTECT_THEM);

  tell_object(caster, _LANG_SKILL_PROTECT_UNPROTECT_ME);
}

/**
 * 'protect off [<someone>]': the one named, found here or anywhere, or everyone
 * the caster protects in the place.
 */
private int do_protect_off(object caster, string who)
{
  object * inv;
  object ob;
  int i, stopped;

  if (strlen(who))
  {
    ob = find_one_match(who, environment(caster), 1);
    if (!ob)
      ob = find_living(who);
    if (!ob || ob->query_protector() != caster)
    {
      tell_object(caster, _LANG_SKILL_PROTECT_NOT_PROTECTING);
      return 0;
    }

    stop_protecting(caster, ob);
    return 1;
  }

  inv = all_inventory(environment(caster));
  stopped = 0;
  for (i = 0; i < sizeof(inv); i++)
    if (living(inv[i]) && inv[i]->query_protector() == caster)
    {
      stop_protecting(caster, inv[i]);
      stopped++;
    }

  if (!stopped)
    tell_object(caster, _LANG_SKILL_PROTECT_NOBODY);
  return stopped > 0;
}

int do_protect(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object ob;
  string who;

  if (who = off_target(target))
    return do_protect_off(caster, who);

  ob = find_one_match(target, environment(caster), 1);
  if (!ob || !living(ob) || ob == caster)
  {
    tell_object(caster, _LANG_SKILL_PROTECT_WHO);
    return 0;
  }

  if (!ob->query_consent(PROTECT_CONS))
  {
    tell_object(caster, _LANG_SKILL_PROTECT_REFUSED);
    return 0;
  }

  ob->set_protector(caster);

  tell_object(caster, _LANG_SKILL_PROTECT_ME);
  tell_object(caster, _LANG_SKILL_PROTECT_HOW_TO_STOP);
  tell_object(ob, _LANG_SKILL_PROTECT_THEM);
  tell_room(environment(caster), _LANG_SKILL_PROTECT_ROOM, ({ ob, caster }));
  return 1;
}
