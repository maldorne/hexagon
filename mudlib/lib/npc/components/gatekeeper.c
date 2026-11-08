// gatekeeper.c -- an NPC that opens the door it stands by for those it lets in.
//
// The NPC stands in a location with a door. Whoever arrives there who may go
// through that door finds it opened (and unlocked, if it was locked); a while
// later the NPC closes it again and locks it if it had to unlock it. Whoever
// arrives who may not pass finds the door shut and locked at once if it was
// open. The NPC never moves anybody and never looks at movement commands: what
// stops somebody walking through an open door is the guard component of the
// same NPC, which this one asks (guardian_check) who is allowed.
//
// Without a guard (or toll) component the NPC decides with its own rules, the
// ones of the guard component of the same name.
//
// It does nothing while its NPC is dead, fighting or away from the door, and
// ignores arrivals that are hidden.
//
// Attributes:
//   "direction"       - the exit with the door
//   "delay"           - seconds the door stays open after the last allowed
//                       arrival (default 15)
//   "open_message"    - told to the room when the NPC opens the door
//   "close_message"   - told to the room when it closes it
//   "lock_message"    - told to the room when it closes and locks it
//   "guilds"          - guild objects whose members pass (own rules only)
//   "guildless"       - 1 to let those with no guild through the guild rules
//   "min_guild_level" - the guild level a member needs to pass
//   "min_class_level" - the class level anybody needs to pass
//   "barred_races"    - race ids (a lineage or its base race) never let in

#include <language.h>

inherit component "/lib/npc/component.c";

#define DEFAULT_DELAY 15

private string gate_direction, gate_open_message, gate_close_message,
               gate_lock_message;
private int gate_delay, gate_guildless, gate_min_guild_level,
            gate_min_class_level;
private string * gate_guilds, * gate_barred_races;
// the door this component opened, the pending close and whether it was locked
static object opened_door;
static int close_handle, relock;

void create()
{
  component::create();
  gate_direction = nil;
  gate_open_message = nil;
  gate_close_message = nil;
  gate_lock_message = nil;
  gate_delay = DEFAULT_DELAY;
  gate_guildless = 0;
  gate_min_guild_level = 0;
  gate_min_class_level = 0;
  gate_guilds = ({ });
  gate_barred_races = ({ });
  opened_door = nil;
  close_handle = 0;
  relock = 0;
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

// The own rules, for an NPC without a guard component: the ones of the guard.
private int _passes_own_rules(object who)
{
  string guild;

  if (member_array((string)who->query_race_id(), gate_barred_races) != -1 ||
      member_array((string)who->query_base_race_id(), gate_barred_races) != -1)
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

// Whether `who` may go through the door: what the guard (or toll) component of
// the NPC says, or the own rules when it has neither.
private int _is_allowed(object who)
{
  object me;

  me = query_entity();
  if (me->has_component("guard") || me->has_component("toll"))
    return (int)me->guardian_check(who);

  return _passes_own_rules(who);
}

// Tells the NPC's room how the door changed, and the one beyond it.
private void _announce(object door, string own, string local, string beyond)
{
  object me, other;

  me = query_entity();
  tell_room(environment(me), own ? own + "\n" : local);

  other = (object)door->query_other_side_door(0);
  if (other && environment(other))
    tell_room(environment(other), beyond);
}

// Opens the door on both sides, unlocking it first. Remembers that it was
// locked, but only when it was shut before: a door left open is not ours.
private void _open(object door)
{
  object me, other;
  string npc_name, phrase;

  me = query_entity();
  npc_name = me->query_cap_name();
  phrase = door->query_door_phrase(door->query_dest());

  relock = door->query_lockable() && !door->query_lock_status();
  door->set_status(1);
  other = (object)door->query_other_side_door(1);
  if (other)
    other->set_status(1);

  opened_door = door;
  _announce(door, gate_open_message, _LANG_GATEKEEPER_OPENS,
            _LANG_GATEKEEPER_OPENS_BEYOND);
}

// Closes the door on both sides, locking it when `lock` is set and it can be.
private void _close(object door, int lock)
{
  object me, other;
  string npc_name, phrase;

  me = query_entity();
  npc_name = me->query_cap_name();
  phrase = door->query_door_phrase(door->query_dest());
  other = (object)door->query_other_side_door(1);

  door->set_status(0);
  if (other)
    other->set_status(0);

  if (lock && door->query_lockable())
  {
    door->set_lock_status(0);
    if (other)
      other->set_lock_status(0);
    _announce(door, gate_lock_message, _LANG_GATEKEEPER_LOCKS,
              _LANG_GATEKEEPER_CLOSES_BEYOND);
  }
  else
    _announce(door, gate_close_message, _LANG_GATEKEEPER_CLOSES,
              _LANG_GATEKEEPER_CLOSES_BEYOND);
}

// Forgets the pending close.
private void _cancel_close()
{
  if (close_handle)
    remove_call_out(close_handle);
  close_handle = 0;
}

/**
 * Closes the door the NPC opened, and locks it again if it had to unlock it.
 * Public because it runs from a call_out.
 */
void close_door()
{
  object door;

  close_handle = 0;
  door = opened_door;
  opened_door = nil;
  if (!door || !door->is_open() || door->is_broken())
    return;

  _close(door, relock);
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
    // a door somebody else left open is not ours to close
    if (door->is_open() && door != opened_door)
      return;

    if (!door->is_open())
      _open(door);

    _cancel_close();
    close_handle = call_out("close_door", gate_delay);
    return;
  }

  if (!door->is_open())
    return;

  _cancel_close();
  opened_door = nil;
  _close(door, 1);
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
  if (gate_open_message)
    attrs["open_message"] = gate_open_message;
  if (gate_close_message)
    attrs["close_message"] = gate_close_message;
  if (gate_lock_message)
    attrs["lock_message"] = gate_lock_message;
  if (sizeof(gate_guilds))
    attrs["guilds"] = gate_guilds;
  if (gate_guildless)
    attrs["guildless"] = gate_guildless;
  if (gate_min_guild_level)
    attrs["min_guild_level"] = gate_min_guild_level;
  if (gate_min_class_level)
    attrs["min_class_level"] = gate_min_class_level;
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
  if (stringp(args["open_message"]))
    gate_open_message = args["open_message"];
  if (stringp(args["close_message"]))
    gate_close_message = args["close_message"];
  if (stringp(args["lock_message"]))
    gate_lock_message = args["lock_message"];
  if (pointerp(args["guilds"]))
    gate_guilds = ({ }) + args["guilds"];
  if (intp(args["guildless"]))
    gate_guildless = args["guildless"];
  if (intp(args["min_guild_level"]))
    gate_min_guild_level = args["min_guild_level"];
  if (intp(args["min_class_level"]))
    gate_min_class_level = args["min_class_level"];
  if (pointerp(args["barred_races"]))
    gate_barred_races = ({ }) + args["barred_races"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Direction", gate_direction }),
    ({ "Delay", gate_delay }),
    ({ "Open", opened_door ? 1 : 0 }),
  });
}
