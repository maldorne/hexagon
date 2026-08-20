
// Monsters: the anonymous half of an area's population.
//
// Ordinary creatures -- fauna, vermin, wildlife -- are counted, not tracked.
// The area knows how many of each kind belong to each of its locations and
// nothing else about them: no uuid, no savefile, no state carried between
// loads. They are cloned fresh from their template every time their location
// loads and simply vanish when it unloads. Anything with an individual life
// the world refers to (a citizen, a POI vacancy, a posted guard) is not here;
// it lives in the area's npc_census, with a uuid.
//
// Inherited by /lib/location/area.c. Variables cannot be shared upward between
// inherited files, so everything this file needs from the area is reached
// through the accessors prototyped below and resolved by the inheriting object.

#include <living/persisted.h>

//   ([ location_file : ([ template_id : count ]) ])
mapping monster_census;

// Calls into the rest of the area go through this_object(): an area is a single
// object carrying the whole inheritance tree, so the call resolves at run time
// against the complete program. That avoids declaring prototypes here for
// functions that live in a sibling file. Only public functions are reachable
// this way, and the result comes back as mixed, hence the casts.

private int _live_monster_count(object loc, string source);
private object spawn_monster(string source, object loc);

void create()
{
  monster_census = ([ ]);
}

// Add one monster of `source` to `location_file`'s bucket. Monsters are
// anonymous: the area only tracks how many of each kind belong to each
// location, with no per-animal identity, uuid or savefile. They are cloned
// from the template when the location loads and simply vanish when it unloads.
// Returns 1 when it was recorded.
int assign_monster(string source, string location_file)
{
  mapping bucket;

  if (!source || !strlen(source) || !location_file || !strlen(location_file))
    return 0;
  if (!((mapping)this_object()->query_npc_intended())[source])
    return 0;

  bucket = monster_census[location_file];
  if (!bucket)
  {
    bucket = ([ ]);
    monster_census[location_file] = bucket;
  }

  if (!bucket[source])
    bucket[source] = 1;
  else
    bucket[source] = bucket[source] + 1;
  this_object()->save_me();

  return 1;
}

// One monster of `source` died: take it off the bucket it was materialized
// from (the NPC carries that location, so an animal that wandered still
// decrements where it was counted). Persisted at once -- a death is never
// left to a later sweep to notice.
void monster_died(string source, string location_file)
{
  mapping bucket;

  bucket = monster_census[location_file];
  if (!bucket || !bucket[source])
    return;

  if (bucket[source] > 1)
    bucket[source] = bucket[source] - 1;
  else
  {
    map_delete(bucket, source);
    if (!map_sizeof(bucket))
      map_delete(monster_census, location_file);
  }
  this_object()->save_me();
}

// How many monsters of `source` the area holds, summed across its locations.
// This is what the population sweep checks against the cap.
int query_monster_live_count(string source)
{
  string * locs;
  int i, n;

  locs = map_indices(monster_census);
  for (i = 0; i < sizeof(locs); i++)
    if (monster_census[locs[i]][source])
      n += monster_census[locs[i]][source];

  return n;
}

mapping query_monster_census() { return monster_census; }

// The sources the population sweep may top up: the intended roster minus every
// sentient kind. Citizens are staffed by their settlement, one named individual
// at a time -- they are never scattered statistically, so they are not the
// sweep's business even when an old conversion left them on the roster.
string * query_monster_sources()
{
  mapping intended;
  string * sources, * out;
  string game;
  int i;

  game = game_from_path((string)this_object()->query_area_path());
  intended = (mapping)this_object()->query_npc_intended();
  sources = map_indices(intended);
  out = ({ });

  for (i = 0; i < sizeof(sources); i++)
  {
    mapping t;
    t = BESTIARY_HANDLER->query_template(game, sources[i]);
    if (!(t && t["sentient"]))
      out += ({ sources[i] });
  }

  return out;
}

// How many anonymous monsters of `source` are already standing in `loc`. A
// monster is told apart from an individual by having no uuid.
private int _live_monster_count(object loc, string source)
{
  object * inv;
  int i, n;

  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && inv[i]->query_npc() && !inv[i]->query_npc_uuid() &&
        inv[i]->query_npc_source() == source)
      n++;

  return n;
}

// Clone one anonymous monster of `source` into `loc`. Nothing is restored and
// nothing will be saved: it has no uuid, so query_persisted() is false and the
// whole savefile path is inert for it. Gender and level are rolled here, fresh
// each time the location loads. It carries the location whose bucket counts it
// so it can decrement that bucket on death.
private object spawn_monster(string source, object loc)
{
  object npc;
  string game, area_path;

  area_path = (string)this_object()->query_area_path();
  game = game_from_path(area_path);

  npc = clone_object(GENERIC_NPC);
  if (!npc)
    return nil;

  npc->set_npc_game(game);
  npc->set_npc_area_path(area_path);
  npc->set_npc_source(source);
  npc->set_monster_location(loc->query_file_name());

  // gender before the template, so apply_template picks the matching
  // per-gender name / short / long
  npc->set_gender((int)this_object()->decide_gender(game, source));
  npc->apply_template(BESTIARY_HANDLER->query_template(game, source));
  npc->set_level((int)this_object()->decide_level(game, source));

  npc->move(loc);

  return npc;
}

// Bring this location's monsters in: clone whatever its bucket says is missing.
// Idempotent -- it tops up to the bucket count rather than adding blindly, so a
// second call on an already-populated location is a no-op.
void restore_location_monsters(object loc, string file)
{
  mapping bucket;
  string * sources;
  int i;

  bucket = monster_census[file];
  if (!bucket)
    return;

  sources = map_indices(bucket);
  for (i = 0; i < sizeof(sources); i++)
  {
    int want, have, j;

    want = bucket[sources[i]];
    have = _live_monster_count(loc, sources[i]);
    for (j = have; j < want; j++)
      spawn_monster(sources[i], loc);
  }
}
