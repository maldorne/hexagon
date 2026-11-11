// rideable.c -- an NPC that can be ridden, as a component.
//
// The attributes are the spec of the mount (the keys of <living/riding.h>: the
// kind, the seats, the speed, what it may cross...), written over the row of its
// kind in the mounts table. The component only holds that data and gives the
// NPC the verbs of its kind; boarding, moving, falling and every text belong
// to the riding handler (/lib/handlers/riding.c).
//
// Template: "rideable": { "kind": "horse" }; any other key of the spec may be
// added to differ from the kind.

#include <living/riding.h>
#include <npc/npc.h>
#include <ecs/ecs.h>

inherit component "/lib/npc/component.c";

private mapping spec;
private int fear_handle;  // the call_out that looks for what it fears

void watch_for_fears();

void create()
{
  component::create();
  spec = ([ ]);
  fear_handle = 0;
}

/**
 * The spec this NPC carries on its own, over its kind's row. The NPC answers
 * for it (query_rideable of /lib/npc.c).
 */
mapping query_rideable() { return spec; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + spec;
}

void init_auto_load_attributes(mapping args)
{
  string * keys;
  int i;

  component::init_auto_load_attributes(args);

  keys = map_indices(args);
  spec = ([ ]);
  for (i = 0; i < sizeof(keys); i++)
    if (keys[i] != "type")
      spec[keys[i]] = args[keys[i]];
}

string query_info()
{
  return "kind " + (stringp(spec[RIDE_KIND]) ? spec[RIDE_KIND] : RIDING_DEFAULT_KIND);
}

/**
 * The verbs of the kind: the ones that board it and the ones that leave it.
 */
mapping query_component_actions()
{
  mapping row, out;
  string * verbs;
  int i;

  out = ([ ]);
  row = handler("riding", query_entity())->query_spec_of(query_entity(), spec);
  if (!row)
    return out;

  verbs = row["verbs_board"];
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_board";

  verbs = row["verbs_dismount"];
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_dismount";

  return out;
}

/**
 * The board verb: with nothing after it, the first mount in the place that
 * takes the rider; otherwise the one the words name. A refusal is left for
 * notify_fail so another mount in the place may still answer.
 */
int do_board(string str)
{
  object who, npc;
  string refusal;

  who = this_player();
  npc = query_entity();
  if (!who || !npc || environment(who) != environment(npc))
    return 0;

  if (!handler("riding", npc)->query_addressed(npc, str))
    return 0;

  refusal = handler("riding", npc)->query_board_refusal(who, npc, -1);
  if (refusal)
  {
    notify_fail(refusal);
    return 0;
  }

  return handler("riding", npc)->board(who, npc);
}

/**
 * The dismount verb answers only for the mount the rider is on.
 */
int do_dismount(string str)
{
  object who;

  who = this_player();
  if (!who || !query_entity() || who->query_mount() != query_entity())
    return 0;

  return handler("riding", query_entity())->dismount(who);
}

// Those riding fall when the NPC dies, and when it goes. A dead mount leaves
// its owner's mounts.
void died(object killer)
{
  if (query_entity())
  {
    handler("riding", query_entity())->unseat_riders(query_entity());
    handler("riding", query_entity())->mount_died(query_entity());
  }
}

void dest_me()
{
  object npc;

  npc = query_entity();
  if (npc && environment(npc))
    handler("riding", npc)->unseat_riders(npc);

  // an owned mount is written where it stands; the NPC has already let this
  // component go, so the spec travels with the call
  if (npc)
    handler("riding", npc)->mount_gone(npc, spec);

  component::dest_me();
}

// The NPC has been moved: unless it is following its rider, those on it fall.
void event_moved(object ob, object from, object dest)
{
  if (ob == query_entity())
  {
    handler("riding", ob)->mount_moved(ob, from, dest);
    watch_for_fears();
  }
}

mapping query_hooks()
{
  return ([ "flee": HOOK_PRIORITY_GATE ]);
}

/**
 * With somebody on it, the mount runs when it is hurt under its wimpy or is
 * fighting what it fears (RIDE_WIMPY, RIDE_FEARS).
 */
int hook_flee(mixed * args)
{
  if (query_entity() && handler("riding", query_entity())->query_wants_to_flee(query_entity()))
    return FLEE_FORCE;

  return FLEE_ABSTAIN;
}

/**
 * Looks, a moment later, for something it fears in its place.
 */
void watch_for_fears()
{
  if (fear_handle)
    remove_call_out(fear_handle);
  fear_handle = call_out("check_fears", 1);
}

/**
 * With somebody on it and something it fears in the place, the mount bolts.
 */
void check_fears()
{
  object npc;

  fear_handle = 0;
  npc = query_entity();
  if (!npc || npc->query_dead() || !environment(npc))
    return;

  if (!sizeof(handler("riding", npc)->query_riders(npc)))
    return;

  if (handler("riding", npc)->query_fear_present(npc))
    handler("riding", npc)->run_away(npc);
}

// Somebody has come in: a ridden mount that fears it runs.
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (ob != query_entity() && query_entity() &&
      handler("riding", query_entity())->query_feared(query_entity(), ob))
    watch_for_fears();
}
