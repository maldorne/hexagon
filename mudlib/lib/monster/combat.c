
// code taken from monster.c, neverbot 04/2009

#include <living/combat.h>
#include <common/properties.h>
#include <npc/npc.h>
#include <living/effects.h>
#include <living/skills.h>

string * attack_effects;    // effect objects used in fights, see add_attack_effect
int protecting;             /* Am I protecting someone currently ? */
string * protect;           /* People that I am protecting */
int follow_speed;           /* how long before we follow someone */

void create()
{
  attack_effects = ({ });
  protect = ({ });
  protecting = 0;
  follow_speed = 3;
}

// Para facilitar la utilizacion de estilos de combate:
void set_unarmed_style(string name)
{
  // Siempre al 100%
  this_object()->add_known_unarmed_style(name, 100);
  this_object()->set_unarmed_combat_style(name);
}

void set_armed_style(string name)
{
  // Siempre al 100%
  this_object()->add_known_armed_style(name, 100);
  this_object()->set_armed_combat_style(name);
}

void add_attacks( string * names )
{
  int i;
  mixed * tmp;
  mixed * messages;

  messages = allocate(3);
  messages[0] = ({ });
  messages[1] = ({ });
  messages[2] = ({ });
  
  for (i = 0; i < sizeof(names); i++)
  {
    // Si el estilo existe
    if (table("unarmed-combat")->style_exists(names[i]))
    {
      // a creature fights its own way at its best: the messages of the top
      // ability band of the style
      tmp = table("unarmed-combat")->query_messages(names[i], 100, this_object());

      // Si es una lista, la añadimos
      if (arrayp(tmp[0]))
      {
        messages[0] += tmp[0];
        messages[1] += tmp[1];
        messages[2] += tmp[2];
      }
      else // En otro caso sera un string individual
      {
        messages[0] += ({ tmp[0] });
        messages[1] += ({ tmp[1] });
        messages[2] += ({ tmp[2] });
      }
    }
  }
  
  // si tenemos mensajes
  if (sizeof(messages[0]))
    this_object()->set_messages(messages);
}

/**
 * The effect objects of the attack effects the monster uses in a fight without
 * knowing them as a spell or skill, loaded; one that fails to load is skipped.
 */
private object * load_attack_effects()
{
  object * list;
  mixed * stored;
  object effect;
  int i;

  list = ({ });

  // a list saved by the older mechanism, ({ chance, name, call, ... }), only
  // repeats skills and spells the monster already uses as known ones; it is
  // read untyped, since indexing a string * checks every element is a string
  stored = attack_effects;
  for (i = 0; i < sizeof(stored); i++)
    if (!stringp(stored[i]))
    {
      attack_effects = ({ });
      return list;
    }

  for (i = 0; i < sizeof(attack_effects); i++)
  {
    effect = nil;
    catch(effect = load_object(attack_effects[i]));
    if (effect)
      list += ({ effect });
  }

  return list;
}

/**
 * The effects (full paths, with .c) the monster uses in a fight.
 */
string * query_attack_effects() { return attack_effects; }

/**
 * Adds an effect the monster uses in a fight without knowing it as a spell or
 * skill, like a creature's own bite or breath. It goes through the same
 * checks as any other (see can_use_effect and the effect's query_usable_by),
 * so an effect meant only for creatures marks itself common in its own file
 * with set_common_effect(1). Its targets come from its own target type and
 * intent, as in choose_effect.
 *
 * @param  path  full path of the effect object, with .c
 */
void add_attack_effect(string path)
{
  if (member_array(path, attack_effects) == -1)
    attack_effects += ({ path });
}

/**
 * @param  path  full path of the effect object, with .c
 * @return 1 when it was one of the attack effects
 */
int remove_attack_effect(string path)
{
  int i;

  if ((i = member_array(path, attack_effects)) == -1)
    return 0;

  attack_effects = delete(attack_effects, i, 1);
  return 1;
}

// Effects in a fight: the spells and skills the monster knows and its attack
// effects, used now and then under the same rules as a player (the effect
// itself refuses what the caster may not do).

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
 * Every effect the monster may use in a fight: the spells and active skills it
 * knows plus its attack effects, each once.
 *
 * @return the effect objects, empty when it has none
 */
object * query_combat_effects()
{
  object * list, * extra;
  int i;

  list = query_known_effects();
  extra = load_attack_effects();
  for (i = 0; i < sizeof(extra); i++)
    if (member_array(extra[i], list) == -1)
      list += ({ extra[i] });

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
 * The choice of what the monster casts in a fight: one of its combat effects
 * it can use now, at random, and whom it goes at, by the effect's target type and
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

  known = query_combat_effects();
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
      // every attacker for a harm, itself and every ally for a help
      if (intent == EFFECT_INTENT_HARM)
        targets = attackers;
      else
        targets = present_allies(attackers) + ({ this_object() });
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
 * the monster is casting or cannot use skills, it uses one of its combat
 * effects.
 */
void combat_effects_heart_beat()
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

void add_protect(mixed shield)
{
  int i;

  if (pointerp(shield))
    for (i = 0; i < sizeof(shield); i++)
      add_protect(shield[i]);
  else if (member_array(shield, protect) != -1)
    return;
  else
    protect += ({ shield });
}

int query_protecting() { return protecting; }
int is_protecting() { return protecting; }

void set_protecting(int i) { protecting = i; }

string *query_protect() { return protect; }
void set_protect(string *shields)
{
  int i;

  for (i = 0; i < sizeof(shields); i++)
    add_protect(shields[i]);
}

void remove_protect(mixed shield) 
{
  int i;

  if (pointerp(shield))
    for(i = 0; i < sizeof(shield); i++)
      remove_protect(shield[i]);
  else
  {
    if ((i = member_array(shield, protect)) == -1)
      return;
    protect = delete(protect, i, 1);
  }
}

void do_follow_move(string dir) 
{
  this_object()->queue_action(dir);
}

int query_follow_speed() { return follow_speed; }
void set_follow_speed(int f) { follow_speed = f; }

void log_death(string filename, object attacker)
{
  object * obs;
  int i;
  string log;

  obs = this_object()->query_attacker_list() + this_object()->query_call_outed();
  
  /*
  if (file_size(filename) == -1)
    return 0;
  */
    
  log = "["+ctime(time(),4)+"] "+this_object()->query_cap_name() + " killed by " + 
    attacker->query_cap_name() + 
    " ("+attacker->query_class_name()+" lvl "+attacker->query_level()+")\n";
    
  if (sizeof(obs) > 1)
    log += "\thelped by ";
  
  for (i = 0; i < sizeof(obs); i++)
  {
    if (obs[i] == attacker)
      continue;
      
    log += obs[i]->query_cap_name() +" ("+obs[i]->query_class_name()+" "+obs[i]->query_level()+")";
    
    if (i<sizeof(obs)-1)
      log += ", ";
    else
      log += ".\n";
  }

#ifdef COMMON_SAVE  
  log_file("common/npcs/"+implode(explode(base_name(this_object()), "/"), "_") + "_log", log);  
#else  
  write_file(filename, log);  
#endif
}

mixed * stats() 
{
  int i;
  mixed * ret;
  mixed * shield;

  shield = ({ });

  for (i = 0; i < sizeof(protect); i++)
    shield += ({ ({ "Protect "+i, protect[i] }) });

  ret = ({
      ({ "Protecting (nosave)", protecting }),
      ({ "Attack effects", attack_effects }),
      ({ "Follow Speed", query_follow_speed() }),
    });

  return ret + shield;
}
