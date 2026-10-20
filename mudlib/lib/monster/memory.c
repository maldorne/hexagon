// memory.c — the grudges a monster holds: the players who fought it, kept by
// name, so it attacks them on sight when they come back, even after they log
// out or the mud restarts. A grudge is forgotten GRUDGE_EXPIRY_SECONDS after
// the last blow between the two.
//
// Grew from the player memory of the original monster ("I remember you you
// bastard, you attacked me then logged off", Taniwha 1995), which kept the
// names for good.

#include <npc/npc.h>

mapping grudges;   // player name -> time of the last blow between us

void create()
{
  grudges = ([ ]);
}

/**
 * Forgets every grudge with no blow for GRUDGE_EXPIRY_SECONDS. The time is
 * absolute, so a grudge also runs out while the mud is down.
 */
void expire_grudges()
{
  string * names;
  int i, now;

  if (!mappingp(grudges))
  {
    grudges = ([ ]);
    return;
  }

  now = time();
  names = map_indices(grudges);
  for (i = 0; i < sizeof(names); i++)
    if (now - grudges[names[i]] > GRUDGE_EXPIRY_SECONDS)
      grudges[names[i]] = nil;
}

/**
 * Starts or renews the grudge against a player who fights the monster. Only a
 * monster of GRUDGE_MIN_MONSTER_LEVEL or more remembers, and only players of
 * GRUDGE_MIN_ATTACKER_LEVEL or more are remembered.
 *
 * @param  player  the player in the fight
 */
void add_grudge(object player)
{
  if (!player || !interactive(player))
    return;
  if (this_object()->query_level() < GRUDGE_MIN_MONSTER_LEVEL)
    return;
  if (player->query_level() < GRUDGE_MIN_ATTACKER_LEVEL)
    return;

  expire_grudges();
  grudges[player->query_name()] = time();
}

/**
 * @param  player  a player
 * @return 1 when the monster still holds a grudge against them
 */
int has_grudge(object player)
{
  if (!player || !interactive(player))
    return 0;

  expire_grudges();
  return grudges[player->query_name()] != nil;
}

/**
 * Forgets the grudge against a player at once: the fight between them is over.
 *
 * @param  player  a player
 */
void remove_grudge(object player)
{
  if (!player || !mappingp(grudges))
    return;

  grudges[player->query_name()] = nil;
}

/**
 * @return player name -> time of the last blow, a copy
 */
mapping query_grudges()
{
  expire_grudges();
  return ([ ]) + grudges;
}

mixed * stats()
{
  return ({
    ({ "Grudges", query_grudges() }),
  });
}
