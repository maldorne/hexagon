// gatekeeper.c -- an NPC that works the door it stands by for those it lets in.
//
// The NPC stands in a location with a door and carries its key. Whoever
// arrives there who may go through finds the NPC running the door's own
// "open" command; a while later it runs "close ... with key", which also
// locks. Whoever arrives who may not pass finds it closing and locking the
// door at once if it was open. The commands go through the NPC's command
// queue, so the door gives the messages and checks the key itself. Every
// gatekeeper in the location reacts; two doing the same thing only get the
// door's "already open/closed" answer.
//
// The component never moves anybody and is independent of the guard
// component: what stops somebody walking through an open door is the guard,
// and the rules here only say for whom the door is opened.
//
// It does nothing while its NPC is dead or fighting, and ignores arrivals
// that are hidden.
//
// Attributes:
//   "direction"       - the exit with the door
//   "delay"           - seconds the door stays open after the last allowed
//                       arrival (default 15)
//   "guilds"          - guild objects whose members pass
//   "guildless"       - 1 to let those with no guild through the guild rules
//   "min_guild_level" - the guild level a member needs to pass
//   "min_class_level" - the class level anybody needs to pass
//   "races"           - race ids (a lineage or its base race) it opens for;
//                       anybody of another race finds the door shut (empty:
//                       any race)
//   "barred_races"    - race ids (a lineage or its base race) it never opens
//                       for, even when listed in "races"; "*" opens for nobody

#include <language.h>

inherit component "/lib/npc/component.c";

#define DEFAULT_DELAY 15

private string gate_direction;
private int gate_delay, gate_guildless, gate_min_guild_level,
            gate_min_class_level;
private string * gate_guilds, * gate_races, * gate_barred_races;
// the pending close
static int close_handle;

void create()
{
  component::create();
  gate_direction = nil;
  gate_delay = DEFAULT_DELAY;
  gate_guildless = 0;
  gate_min_guild_level = 0;
  gate_min_class_level = 0;
  gate_guilds = ({ });
  gate_races = ({ });
  gate_barred_races = ({ });
  close_handle = 0;
}

string query_direction() { return gate_direction; }

// The door of the exit this NPC watches, nil when the place has none.
private object _door()
{
  object me, env;

  me = query_entity();
  env = me ? environment(me) : nil;
  if (!env || !gate_direction || !strlen(gate_direction))
    return nil;

  return env->query_door_ob(gate_direction);
}

// Whether the NPC is in a state to work the door.
private int _can_act()
{
  object me;

  me = query_entity();
  if (!me || !environment(me) || me->query_dead())
    return 0;

  return !sizeof(me->query_attacker_list());
}

// Whether `who` may go through the door by the attributes above.
private int _is_allowed(object who)
{
  string guild, race, base;

  race = (string)who->query_race_id();
  base = (string)who->query_base_race_id();

  if (member_array("*", gate_barred_races) != -1 ||
      member_array(race, gate_barred_races) != -1 ||
      member_array(base, gate_barred_races) != -1)
    return 0;

  if (sizeof(gate_races) && member_array(race, gate_races) == -1 &&
      member_array(base, gate_races) == -1)
    return 0;

  if (gate_min_class_level &&
      (int)who->query_class_level() < gate_min_class_level)
    return 0;

  if (!sizeof(gate_guilds) && !gate_min_guild_level)
    return 1;

  guild = who->query_guild_ob();
  if (!guild || !strlen(guild))
    return gate_guildless;

  if (sizeof(gate_guilds) && member_array(guild, gate_guilds) == -1)
    return 0;

  return (int)who->query_guild_level() >= gate_min_guild_level;
}

// Makes the NPC open the door, unlocking it with its key if needed.
private void _open(object door)
{
  query_entity()->queue_action(door->query_open_verb() + " " + gate_direction);
}

// Makes the NPC close the door and lock it with its key.
private void _close(object door)
{
  object me;
  string * cmds;
  int i;

  me = query_entity();
  cmds = _LANG_GATEKEEPER_LOCK_COMMANDS(door->query_close_verb(), gate_direction);
  for (i = 0; i < sizeof(cmds); i++)
    me->queue_action(cmds[i]);
}

// Forgets the pending close.
private void _cancel_close()
{
  if (close_handle)
    remove_call_out(close_handle);
  close_handle = 0;
}

/**
 * Closes and locks the door if it is still open. Public because it runs from
 * a call_out.
 */
void close_door()
{
  object door;

  close_handle = 0;
  door = _door();
  if (!door || !_can_act() || !door->is_open() || door->is_broken())
    return;

  _close(door);
}

/**
 * Looks at somebody who arrived: opens the door for whoever may pass and shuts
 * and locks it for whoever may not. Public because it runs from a call_out.
 */
void watch(object who)
{
  object door;

  if (!who || !_can_act() || environment(who) != environment(query_entity()))
    return;
  if (who->query_hidden() || who->query_dead())
    return;

  door = _door();
  if (!door || door->is_broken())
    return;

  if (_is_allowed(who))
  {
    if (!door->is_open())
      _open(door);

    _cancel_close();
    close_handle = call_out("close_door", gate_delay);
    return;
  }

  if (door->is_open())
  {
    _cancel_close();
    _close(door);
  }
}

/**
 * Somebody has arrived: queued, since an event never acts inside itself.
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (!ob || !living(ob) || ob == query_entity())
    return;
  call_out("watch", 1, ob);
}

void dest_me()
{
  _cancel_close();
  component::dest_me();
}

mapping query_auto_load_attributes()
{
  mapping attrs;

  attrs = component::query_auto_load_attributes();
  attrs["direction"] = gate_direction;
  if (gate_delay != DEFAULT_DELAY)
    attrs["delay"] = gate_delay;
  if (sizeof(gate_guilds))
    attrs["guilds"] = gate_guilds;
  if (gate_guildless)
    attrs["guildless"] = gate_guildless;
  if (gate_min_guild_level)
    attrs["min_guild_level"] = gate_min_guild_level;
  if (gate_min_class_level)
    attrs["min_class_level"] = gate_min_class_level;
  if (sizeof(gate_races))
    attrs["races"] = gate_races;
  if (sizeof(gate_barred_races))
    attrs["barred_races"] = gate_barred_races;

  return attrs;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (stringp(args["direction"]))
    gate_direction = args["direction"];
  if (intp(args["delay"]) && args["delay"] > 0)
    gate_delay = args["delay"];
  if (pointerp(args["guilds"]))
    gate_guilds = ({ }) + args["guilds"];
  if (intp(args["guildless"]))
    gate_guildless = args["guildless"];
  if (intp(args["min_guild_level"]))
    gate_min_guild_level = args["min_guild_level"];
  if (intp(args["min_class_level"]))
    gate_min_class_level = args["min_class_level"];
  if (pointerp(args["races"]))
    gate_races = ({ }) + args["races"];
  if (pointerp(args["barred_races"]))
    gate_barred_races = ({ }) + args["barred_races"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Direction", gate_direction }),
    ({ "Delay", gate_delay }),
    ({ "Races", implode(gate_races, ", ") }),
    ({ "Barred Races", implode(gate_barred_races, ", ") }),
  });
}
