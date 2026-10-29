// boss.c -- a boss, as an NPC component.
//
// Marks the NPC as a boss: it shows a mark beside its name in a room, and the
// bosses handler (/lib/handlers/bosses.c) keeps the record of its fights: how
// long each lasted, who took part, who struck the killing blow, and the
// players it has killed. Declared in the npc template with no attributes:
//
//   "boss" : ([ ])

inherit component "/lib/npc/component.c";

// seconds without a blow after which the next one is taken for a new fight
#define FIGHT_GAP 120

// when the current fight began and when its last blow landed, 0 for none
private int fight_start, last_blow;

void create()
{
  component::create();
  fight_start = 0;
  last_blow = 0;
}

string query_info()
{
  return "boss";
}

// Known to the bosses handler as soon as it stands in the world.
void placed()
{
  object npc;

  npc = query_entity();
  if (npc)
    handler("bosses", npc)->register_boss(npc);
}

// A blow in the room: the first one that involves the boss starts the clock.
void event_fight_in_progress(object ob, object * fighters)
{
  object npc;

  npc = query_entity();
  if (!npc || !pointerp(fighters) || member_array(npc, fighters) == -1)
    return;

  if (!fight_start || time() - last_blow > FIGHT_GAP)
    fight_start = time();
  last_blow = time();
}

// It dies, still standing: whoever fights it is the group that beat it.
void died(object killer)
{
  object npc;

  npc = query_entity();
  if (!npc)
    return;

  handler("bosses", npc)->record_death(npc, killer, npc->query_attacker_list(),
    fight_start ? time() - fight_start : 0);
  fight_start = 0;
  last_blow = 0;
}

// The boss has dealt a killing blow.
void event_kill(object victim)
{
  object npc;

  npc = query_entity();
  if (npc)
    handler("bosses", npc)->record_kill(npc, victim);
}
