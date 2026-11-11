// rideable.c -- an NPC that can be ridden, as a component.
//
// The attributes are the spec of the steed (the keys of <living/riding.h>: the
// kind, the seats, the speed, what it may cross...), written over the row of its
// kind in the steeds table. The component only holds that data and gives the
// NPC the verbs of its kind; boarding, moving, falling and every text belong
// to the riding handler (/lib/handlers/riding.c).
//
// Template: "rideable": { "kind": "horse" }; any other key of the spec may be
// added to differ from the kind.

#include <living/riding.h>

inherit component "/lib/npc/component.c";

private mapping spec;

void create()
{
  component::create();
  spec = ([ ]);
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
 * The board verb: with nothing after it, the first steed in the place that
 * takes the rider; otherwise the one the words name. A refusal is left for
 * notify_fail so another steed in the place may still answer.
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
 * The dismount verb answers only for the steed the rider is on.
 */
int do_dismount(string str)
{
  object who;

  who = this_player();
  if (!who || !query_entity() || who->query_mount() != query_entity())
    return 0;

  return handler("riding", query_entity())->dismount(who);
}

// Those riding fall when the NPC dies, and when it goes. A dead steed is no
// longer in its owner's registry.
void died(object killer)
{
  if (query_entity())
  {
    handler("riding", query_entity())->unseat_riders(query_entity());
    handler("riding", query_entity())->steed_died(query_entity());
  }
}

void dest_me()
{
  object npc;

  npc = query_entity();
  if (npc && environment(npc))
    handler("riding", npc)->unseat_riders(npc);

  // an owned steed stays in its owner's registry where it stood; the NPC has
  // already let this component go, so the spec travels with the call
  if (npc)
    handler("riding", npc)->steed_gone(npc, spec);

  component::dest_me();
}

// The NPC has been moved: unless it is following its rider, those on it fall.
void event_moved(object ob, object from, object dest)
{
  if (ob == query_entity())
    handler("riding", ob)->steed_moved(ob, from, dest);
}
