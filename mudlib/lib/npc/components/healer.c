// healer.c -- an NPC that heals those fighting on its side, as an NPC
// component.
//
// While a fight goes on in its place, every few seconds it casts one of its
// healing effects on the most wounded of its side, when somebody is below the
// threshold. Its side are the people of its community (the census of its
// area's root) and whoever fights their attackers there, itself included.
// It does not need to be fighting; pair it with the pacifist template field
// for one who answers calls for help only to heal.
//
// Attributes:
//   "effects" - files of the effects it heals with (spells or skills it knows)
//   "below"   - percent of hit points under which somebody is healed (default 70)
//   "delay"   - seconds between two attempts (default 4)

#include <areas/area.h>
#include <common/properties.h>
#include <living/effects.h>
#include <ecs/ecs.h>

inherit component "/lib/npc/component.c";

#define DEFAULT_BELOW 70
#define DEFAULT_DELAY 4

private string * heal_effects;
private int heal_below, heal_delay;
static int heal_handle;

void create()
{
  component::create();
  heal_effects = ({ });
  heal_below = DEFAULT_BELOW;
  heal_delay = DEFAULT_DELAY;
  heal_handle = 0;
}

string query_info()
{
  return sizeof(heal_effects) + " effects under " + heal_below + "%";
}

// The community a living belongs to: the root of its area, nil for anybody
// who is not a person of an area.
private object community_of(object who)
{
  object area;
  string path;

  path = who->query_npc_area_path();
  if (!stringp(path) || !strlen(path))
    return nil;

  area = (object)load_object(AREA_HANDLER)->query_area(path);
  return area ? (object)area->query_root_area() : nil;
}

/**
 * Those on the healer's side in its place: the people of its community there,
 * and whoever fights one of their attackers. Their attackers never are.
 */
private object * allies_here(object me)
{
  object * all, * friends, * enemies, * allies, * theirs;
  object mine;
  int i, j;

  mine = community_of(me);
  all = all_inventory(environment(me));
  friends = ({ me });
  for (i = 0; i < sizeof(all); i++)
    if (all[i] != me && living(all[i]) && !all[i]->query_dead() &&
        mine && community_of(all[i]) == mine)
      friends += ({ all[i] });

  enemies = ({ });
  for (i = 0; i < sizeof(friends); i++)
    enemies |= friends[i]->query_attacker_list();
  enemies -= friends + ({ nil });

  allies = friends;
  for (i = 0; i < sizeof(all); i++)
  {
    if (!living(all[i]) || member_array(all[i], allies) != -1 ||
        member_array(all[i], enemies) != -1)
      continue;

    theirs = all[i]->query_attacker_list();
    for (j = 0; j < sizeof(enemies); j++)
      if (member_array(enemies[j], theirs) != -1)
      {
        allies += ({ all[i] });
        break;
      }
  }

  return allies - enemies;
}

// Whether somebody in the place is fighting somebody else there.
private int fight_here(object me)
{
  object * all, * rivals;
  int i, j;

  all = all_inventory(environment(me));
  for (i = 0; i < sizeof(all); i++)
  {
    if (!living(all[i]))
      continue;

    rivals = all[i]->query_attacker_list();
    for (j = 0; j < sizeof(rivals); j++)
      if (rivals[j] && environment(rivals[j]) == environment(me))
        return 1;
  }

  return 0;
}

// The most wounded ally under the threshold, nil when nobody needs it.
private object most_wounded(object me)
{
  object * allies;
  object worst;
  int i, max, pct, worst_pct;

  allies = allies_here(me);
  worst = nil;
  worst_pct = heal_below;
  for (i = 0; i < sizeof(allies); i++)
  {
    max = allies[i]->query_max_hp();
    if (max <= 0)
      continue;

    pct = allies[i]->query_hp() * 100 / max;
    if (pct < worst_pct)
    {
      worst = allies[i];
      worst_pct = pct;
    }
  }

  return worst;
}

// One of the healing effects it knows and can start now, at random.
private object usable_effect(object me)
{
  object * known, * usable;
  object effect;
  int i;

  known = me->query_known_effects();
  usable = ({ });
  for (i = 0; i < sizeof(heal_effects); i++)
  {
    effect = find_object(heal_effects[i]);
    if (!effect)
      catch(effect = load_object(heal_effects[i]));
    if (effect && member_array(effect, known) != -1 &&
        me->can_use_effect(effect))
      usable += ({ effect });
  }

  return sizeof(usable) ? usable[random(sizeof(usable))] : nil;
}

mapping query_hooks()
{
  return ([ "choose_effect": HOOK_PRIORITY_GATE ]);
}

// Its healing effects are cast by heal_round on whoever needs them; they are
// kept out of the pool the NPC casts from in a fight.
mixed * hook_choose_effect(mixed * args)
{
  mapping ctx;
  object effect;
  int i;

  ctx = args[0];
  for (i = 0; i < sizeof(heal_effects); i++)
  {
    effect = find_object(heal_effects[i]);
    if (effect)
      ctx["usable"] -= ({ effect });
  }

  return args;
}

// A blow in the healer's place: start looking after its side.
void event_fight_in_progress(object ob, object * fighters)
{
  if (!heal_handle && sizeof(heal_effects))
    heal_handle = call_out("heal_round", heal_delay);
}

/**
 * Heals the most wounded of its side, if it can, and looks again later while
 * the fight lasts.
 */
void heal_round()
{
  object me, target, effect;

  heal_handle = 0;
  me = query_entity();
  if (!me || me->query_dead() || !environment(me))
    return;

  if (!fight_here(me))
    return;

  // already casting, or locked out of casting
  if (!me->query_active_effect("effect") &&
      !me->query_property(PROPERTY_IS_CASTING) &&
      !me->check_props(NOSKILL_PROP) &&
      !me->query_property(PASSED_OUT_PROP) &&
      !environment(me)->query_property(NOSKILL_PROP))
  {
    target = most_wounded(me);
    effect = target ? usable_effect(me) : nil;
    if (effect)
      effect->cast_effect_on(({ target }), me);
  }

  heal_handle = call_out("heal_round", heal_delay);
}

void dest_me()
{
  if (heal_handle)
    remove_call_out(heal_handle);
  component::dest_me();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "effects" : heal_effects,
    "below"   : heal_below,
    "delay"   : heal_delay,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["effects"]))
    heal_effects = ({ }) + args["effects"];
  if (intp(args["below"]))
    heal_below = args["below"];
  if (intp(args["delay"]) && args["delay"] > 0)
    heal_delay = args["delay"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Effects", heal_effects }),
    ({ "Below", heal_below }),
    ({ "Delay", heal_delay }),
  });
}
