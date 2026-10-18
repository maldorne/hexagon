// Default casting of monsters and NPCs: in a fight, now and then, they use one
// of the spells or skills they know, the same ones and under the same rules as
// a player (the effect itself refuses what the caster may not do).

#include <npc/npc.h>
#include <living/effects.h>
#include <living/skills.h>
#include <common/properties.h>

/**
 * Every spell and skill object the monster knows and could run as an action:
 * the passive skills have nothing to cast.
 *
 * @return the effect objects, empty when it knows none
 */
object * query_known_effects()
{
  mapping skills;
  mixed spells;
  object effect;
  string * ids;
  string path;
  object * list;
  int i;

  list = ({ });

  spells = this_object()->query_spells();
  if (mappingp(spells))
  {
    ids = map_indices(spells);
    for (i = 0; i < sizeof(ids); i++)
    {
      // the catalogue says where a spell is now; the path kept is the fallback
      path = table("spells")->query_spell_path(ids[i]);
      if (!path && pointerp(spells[ids[i]]) && sizeof(spells[ids[i]]))
        path = spells[ids[i]][S_OBJECT];

      effect = nil;
      if (stringp(path))
        catch(effect = load_object(path));
      if (effect)
        list += ({ effect });
    }
  }

  skills = this_object()->query_skill_list();
  ids = map_indices(skills);
  for (i = 0; i < sizeof(ids); i++)
  {
    if (skills[ids[i]][3] == PASSIVE_SKILL)
      continue;

    effect = nil;
    catch(effect = load_object(skills[ids[i]][SKILL_DATA_PATH]));
    if (effect)
      list += ({ effect });
  }

  return list;
}

/**
 * Whether the monster can start the effect right now, as far as it can tell
 * without casting: it has a target intent, the guild teaches it, there is gp
 * enough and no lockout of the effect is running. The cast checks the rest.
 *
 * @param  effect  a spell or skill object
 * @return 1 when it is worth trying
 */
int can_use_effect(object effect)
{
  string * checks;
  int i;

  if (effect->query_effect_intent() == EFFECT_INTENT_NONE)
    return 0;

  if (!effect->query_usable_by(this_object()))
    return 0;

  if (this_object()->query_gp() < effect->query_effect_gp_cost(this_object()))
    return 0;

  checks = effect->query_property_checks();
  for (i = 0; i < sizeof(checks); i++)
    if (this_object()->query_property(checks[i]))
      return 0;

  return 1;
}

/**
 * The living things fighting the monster in its own place.
 */
private object * present_attackers()
{
  object * list;
  int i;

  list = ({ }) + this_object()->query_attacker_list();
  for (i = sizeof(list) - 1; i >= 0; i--)
    if (!list[i] || !living(list[i]) || environment(list[i]) != environment())
      list = delete(list, i, 1);

  return list;
}

/**
 * Those in the place who fight on the monster's side: they are fighting one of
 * its attackers.
 *
 * @param  attackers  who is fighting the monster
 */
private object * present_allies(object * attackers)
{
  object * all, * allies, * theirs;
  int i, j;

  allies = ({ });
  all = all_inventory(environment());
  for (i = 0; i < sizeof(all); i++)
  {
    if (all[i] == this_object() || !living(all[i]) ||
        member_array(all[i], attackers) != -1)
      continue;

    theirs = all[i]->query_attacker_list();
    for (j = 0; j < sizeof(attackers); j++)
      if (member_array(attackers[j], theirs) != -1)
      {
        allies += ({ all[i] });
        break;
      }
  }

  return allies;
}

/**
 * Up to a number of random members of a list.
 */
private object * take_some(object * list, int n)
{
  object * taken;
  int i;

  taken = ({ });
  while (sizeof(list) && sizeof(taken) < n)
  {
    i = random(sizeof(list));
    taken += ({ list[i] });
    list = delete(list, i, 1);
  }

  return taken;
}

/**
 * The choice of what the monster casts in a fight: one of the effects it can
 * use now, at random, and whom it goes at, by the effect's target type and
 * intent. A harm goes at its attackers, a help at itself (and its allies for a
 * many-target one); an effect that reaches a place or needs no target is just
 * cast. This is the hook an AI component replaces: override it (or the
 * component answers before it) to choose with some reason; cast_chosen_effect
 * only carries out what it returns.
 *
 * @return ({ effect object, targets }) (targets empty when the effect finds
 *         its own), or nil when there is nothing to cast
 */
mixed * choose_effect()
{
  object * known, * usable, * attackers, * targets;
  object effect;
  string type, intent;
  int i;

  attackers = present_attackers();
  if (!sizeof(attackers))
    return nil;

  known = query_known_effects();
  usable = ({ });
  for (i = 0; i < sizeof(known); i++)
    if (can_use_effect(known[i]))
      usable += ({ known[i] });

  if (!sizeof(usable))
    return nil;

  effect = usable[random(sizeof(usable))];
  type = effect->query_target_type();
  intent = effect->query_effect_intent();
  targets = ({ });

  switch (type)
  {
    case TARGET_TYPE_ONE:
    case TARGET_TYPE_TOUCH:
      if (intent == EFFECT_INTENT_HARM)
        targets = take_some(attackers, 1);
      else
        targets = ({ this_object() });
      break;

    case TARGET_TYPE_MANY:
      if (intent == EFFECT_INTENT_HARM)
        targets = take_some(attackers, effect->query_max_targets());
      else
        targets = take_some(present_allies(attackers),
          effect->query_max_targets() - 1) + ({ this_object() });
      break;

    case TARGET_TYPE_SELF:
    case TARGET_TYPE_ALL:
    case TARGET_TYPE_NONE:
      // it works out its own targets
      break;

    default:
      // an item or a pass: nothing a monster aims in a fight
      return nil;
  }

  return ({ effect, targets });
}

/**
 * Casts what choose_effect returned, through the entry points a player's
 * command uses too: cast_effect_on for the targets chosen, cast_effect for
 * the effects that find their own.
 *
 * @param  choice  ({ effect object, targets })
 * @return 1 when the effect started
 */
int cast_chosen_effect(mixed * choice)
{
  object effect;
  object * targets;

  if (!choice || sizeof(choice) < 2)
    return 0;

  effect = choice[0];
  targets = choice[1];

  if (sizeof(targets))
    return effect->cast_effect_on(targets, this_object());

  return effect->cast_effect("", this_object(), 0);
}

/**
 * Called each heart beat: in a fight, with a modest chance, and never while
 * the monster is casting or cannot use skills, it casts something it knows.
 */
void casting_heart_beat()
{
  if (!sizeof(this_object()->query_attacker_list()))
    return;

  if (random(100) >= NPC_CAST_CHANCE)
    return;

  // already casting, or locked out of casting
  if (this_object()->query_active_effect("effect") ||
      this_object()->query_property(PROPERTY_IS_CASTING) ||
      this_object()->check_props(NOSKILL_PROP) ||
      this_object()->query_property(PASSED_OUT_PROP) ||
      (environment() && environment()->query_property(NOSKILL_PROP)))
    return;

  cast_chosen_effect(choose_effect());
}
