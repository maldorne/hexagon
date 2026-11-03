
// Monsters: the anonymous half of an area's population.
//
// Ordinary creatures -- fauna, vermin, wildlife -- are counted, not tracked.
// The community knows how many of each kind belong to each of its locations and
// nothing else about them; the counts are kept with its root area, like the
// roster and the census: no uuid, no savefile, no state carried between
// loads. They are cloned fresh from their template every time their location
// loads and simply vanish when it unloads. Anything with an individual life
// the world refers to (a citizen, a POI vacancy, a posted guard) is not here;
// it lives in the area's npc_census, with a uuid.

#include <living/persisted.h>

//   ([ location_file : ([ template_id : count ]) ])
mapping monster_census;


private int _live_monster_count(object loc, string source);
private mapping _books();
int query_monster_live_count(string source);
int query_area_monster_count(string source);
private object spawn_monster(string source, object loc);

void create()
{
  monster_census = ([ ]);
}

/**
 * The buckets of this area's community, kept by its root area. An area that
 * still holds buckets of its own hands them up the first time they are asked
 * for: each location file belongs to one area, so nothing collides.
 *
 * @return the community's ([ location_file : ([ template_id : count ]) ])
 */
private mapping _books()
{
  object root;
  mapping books;
  string * files;
  int i;

  root = (object)this_object()->query_root_area();
  if (!root || root == this_object())
    return monster_census;

  books = (mapping)root->query_monster_census();
  if (map_sizeof(monster_census))
  {
    files = map_indices(monster_census);
    for (i = 0; i < sizeof(files); i++)
      books[files[i]] = monster_census[files[i]];
    monster_census = ([ ]);
    this_object()->mark_dirty();
  }

  return books;
}

// Add one monster of `source` to `location_file`'s bucket. Monsters are
// anonymous: the area only tracks how many of each kind belong to each
// location, with no per-animal identity, uuid or savefile. They are cloned
// from the template when the location loads and simply vanish when it unloads.
// Returns 1 when it was recorded.
// Whether a bucket may hold this source: on the roster, and not sentient.
// Somebody the world knows one by one belongs in the census, not in a count.
private int _is_monster_source(string source)
{
  mapping t;

  if (!((mapping)this_object()->query_npc_caps())[source])
    return 0;

  t = BESTIARY_HANDLER->query_template(
        game_from_path((string)this_object()->query_area_path()), source);

  return !(t && t["sentient"]);
}

// Whether the type of `source` lives in water.
private int _swims(string source)
{
  mapping t;

  t = BESTIARY_HANDLER->query_template(
        game_from_path((string)this_object()->query_area_path()), source);

  return t && t["swims"];
}

int assign_monster(string source, string location_file)
{
  mapping books, bucket;

  if (!source || !strlen(source) || !location_file || !strlen(location_file))
    return 0;
  if (!_is_monster_source(source))
    return 0;

  books = _books();
  bucket = books[location_file];
  if (!bucket)
  {
    bucket = ([ ]);
    books[location_file] = bucket;
  }

  if (!bucket[source])
    bucket[source] = 1;
  else
    bucket[source] = bucket[source] + 1;
  this_object()->mark_dirty();

  return 1;
}

// One monster of `source` died: take it off the bucket it was materialized
// from (the NPC carries that location, so an animal that wandered still
// decrements where it was counted). Persisted at once -- a death is never
// left to a later sweep to notice.
void monster_died(string source, string location_file)
{
  mapping books, bucket;

  books = _books();
  bucket = books[location_file];
  if (!bucket || !bucket[source])
    return;

  if (bucket[source] > 1)
    bucket[source] = bucket[source] - 1;
  else
  {
    map_delete(bucket, source);
    if (!map_sizeof(bucket))
      map_delete(books, location_file);
  }
  this_object()->mark_dirty();
}

// How many monsters of `source` the bucket of `file` holds.
private int _bucket_count(mapping books, string file, string source)
{
  mixed count;

  count = books[file][source];
  return count ? count : 0;
}

// The location files of the community's buckets that belong to this area.
private string * _own_files(mapping books)
{
  mapping mine;
  string * files, * out;
  int i;

  mine = (mapping)this_object()->query_locations();
  if (!mine)
    return ({ });

  files = map_indices(books);
  out = ({ });
  for (i = 0; i < sizeof(files); i++)
    if (mine[files[i]])
      out += ({ files[i] });

  return out;
}

// Bring the monster counts back within what this area's locations ask for,
// after a conversion or a roster limit changed it: a kind no room of the area
// clones any more goes, and a kind over its share loses the excess, first from
// locations that do not declare it and then from the fullest ones. The share is
// what the population sweep tops up to (cut to the roster limit, as there), so
// lowering either lowers the population instead of leaving the old one in place.
// The shares are trimmed in this area's own locations; a roster limit, held by
// the whole community, in any of them.
void trim_monsters_to_caps()
{
  string * sources, * files, * own;
  mapping caps, books;
  int i, j, changed;

  caps = (mapping)this_object()->query_npc_caps();
  books = _books();
  own = _own_files(books);

  sources = ({ });
  for (i = 0; i < sizeof(own); i++)
    sources |= map_indices(books[own[i]]);

  for (i = 0; i < sizeof(sources); i++)
  {
    mapping declared;
    int share, excess;

    declared = (mapping)this_object()->query_original_clone_counts(sources[i]);
    share = 0;
    files = map_indices(declared);
    for (j = 0; j < sizeof(files); j++)
      share += declared[files[j]];
    if (caps[sources[i]] && caps[sources[i]]["limit"] &&
        share > caps[sources[i]]["limit"])
      share = caps[sources[i]]["limit"];

    excess = query_area_monster_count(sources[i]) - share;
    if (excess <= 0)
      continue;

    // locations that never declared this kind are emptied of it first
    for (j = 0; j < sizeof(own) && excess > 0; j++)
    {
      int here;

      here = _bucket_count(books, own[j], sources[i]);
      if (!here || declared[own[j]])
        continue;

      here = (here > excess) ? excess : here;
      books[own[j]][sources[i]] -= here;
      excess -= here;
      changed = 1;
    }

    // then one at a time from wherever most of them are
    while (excess > 0)
    {
      string fullest;
      int most;

      fullest = nil;
      most = 0;
      for (j = 0; j < sizeof(own); j++)
        if (_bucket_count(books, own[j], sources[i]) > most)
        {
          most = _bucket_count(books, own[j], sources[i]);
          fullest = own[j];
        }

      if (!fullest)
        break;

      books[fullest][sources[i]] -= 1;
      excess--;
      changed = 1;
    }
  }

  // a roster limit holds for the whole community: past it, one at a time from
  // the fullest bucket of any of its areas
  files = map_indices(books);
  sources = map_indices(caps);
  for (i = 0; i < sizeof(sources); i++)
  {
    int excess;

    if (!caps[sources[i]]["limit"])
      continue;

    excess = query_monster_live_count(sources[i]) - caps[sources[i]]["limit"];
    while (excess > 0)
    {
      string fullest;
      int most;

      fullest = nil;
      most = 0;
      for (j = 0; j < sizeof(files); j++)
        if (_bucket_count(books, files[j], sources[i]) > most)
        {
          most = _bucket_count(books, files[j], sources[i]);
          fullest = files[j];
        }

      if (!fullest)
        break;

      books[fullest][sources[i]] -= 1;
      excess--;
      changed = 1;
    }
  }

  if (!changed)
    return;

  // drop the counts and the buckets left at zero
  for (i = 0; i < sizeof(files); i++)
  {
    sources = map_indices(books[files[i]]);
    for (j = 0; j < sizeof(sources); j++)
      if (books[files[i]][sources[j]] <= 0)
        map_delete(books[files[i]], sources[j]);
    if (!map_sizeof(books[files[i]]))
      map_delete(books, files[i]);
  }

  this_object()->save_me();
}

// How many monsters of `source` the community holds, summed across all its
// locations. This is what a roster limit is checked against.
int query_monster_live_count(string source)
{
  mapping books;
  string * locs;
  int i, n;

  books = _books();
  locs = map_indices(books);
  for (i = 0; i < sizeof(locs); i++)
    if (books[locs[i]][source])
      n += books[locs[i]][source];

  return n;
}

// How many monsters of `source` this area's own locations hold: what the
// population sweep tops up against what the area's rooms declared.
int query_area_monster_count(string source)
{
  mapping books;
  string * own;
  int i, n;

  books = _books();
  own = _own_files(books);
  for (i = 0; i < sizeof(own); i++)
    if (books[own[i]][source])
      n += books[own[i]][source];

  return n;
}

mapping query_monster_census() { return _books(); }

// This area's own buckets, without asking its root: for moving books when the
// area joins or leaves a community.
mapping query_own_monster_census() { return monster_census; }
void set_own_monster_census(mapping m) { monster_census = m ? m : ([ ]); }

// The sources the population sweep may top up as fauna: the caps roster minus
// every sentient kind. Citizens are staffed by their settlement, one named
// individual at a time -- they are never scattered statistically, so they are
// not counted here even when an old conversion left them on the roster. The
// people nothing else places are swept as persons instead
// (query_unplaced_people_sources).
string * query_monster_sources()
{
  mapping caps;
  string * sources, * out;
  string game;
  int i;

  game = game_from_path((string)this_object()->query_area_path());
  caps = (mapping)this_object()->query_npc_caps();
  sources = map_indices(caps);
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
    if (inv[i] && inv[i]->query_npc() && !inv[i]->query_uuid() &&
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
  mapping t;
  string * kit;
  int i;

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
  t = (mapping)this_object()->query_area_template(game, source);
  npc->apply_template(t, 1);
  npc->set_level((int)this_object()->decide_level(game, source));

  // the type's kit, rolled anew each time like the rest of an anonymous monster
  kit = ({ });
  if (t && pointerp(t["equipment"]) && sizeof(t["equipment"]))
    kit = (string *)this_object()->resolve_equipment(t["equipment"]);
  for (i = 0; i < sizeof(kit); i++)
    npc->add_clone(kit[i], 1);

  npc->move(loc);
  if (sizeof(kit))
    npc->do_equip("");
  // its components may need to see the world it now stands in: a guard
  // registers on the exit it watches
  npc->components_placed();

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

  bucket = _books()[file];
  if (!bucket)
    return;

  sources = map_indices(bucket);
  for (i = 0; i < sizeof(sources); i++)
  {
    int want, have, j;

    // What a bucket may hold changes under it -- opening a vacancy takes a
    // type off the roster. Drop what no longer belongs instead of spawning it.
    if (!_is_monster_source(sources[i]))
    {
      map_delete(bucket, sources[i]);
      this_object()->save_me();
      continue;
    }

    // nothing that cannot swim is put in water, whatever a room declared
    if (loc->query_water_environment() && !loc->query_bridge() &&
        !_swims(sources[i]))
      continue;

    want = bucket[sources[i]];
    have = _live_monster_count(loc, sources[i]);
    for (j = have; j < want; j++)
      spawn_monster(sources[i], loc);
  }
}
