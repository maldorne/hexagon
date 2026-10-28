// Traits: the lasting conditions a living carries (a wound, a poison, a
// curse). The behaviour lives in one trait object per kind under
// /lib/obj/traits/ (or a game's own obj/traits/); the living keeps only the
// data of each one it carries, which is saved with it. See
// /lib/obj/traits/traits.md.

#include <living/traits.h>
#include <living/stats.h>

// id -> data mapping; saved with the living
mapping traits;

// what the traits add up to, rebuilt on every change: ([ "stats" : ([ name :
// delta ]), "flags" : ([ flag : how many traits raise it ]) ])
static mapping trait_summary;

void create()
{
  traits = ([ ]);
  trait_summary = ([ "stats" : ([ ]), "flags" : ([ ]) ]);
}

/**
 * The file a new trait id resolves to: the game's obj/traits/ first, then the
 * shared directory. "" when the id is not a plain name or no file is there.
 */
private string trait_path(string id)
{
  string path, root;

  if (!stringp(id) || !strlen(id) || strsrch(id, "/") != -1 ||
      strsrch(id, "..") != -1)
    return "";

  root = game_root(this_object());
  path = root + TRAIT_DIR + id + ".c";

  if (root != "/" && file_exists(path))
    return path;

  path = TRAIT_SHARED_DIR + id + ".c";

  return file_exists(path) ? path : "";
}

/**
 * The trait object an id stands for, or nil when there is none.
 *
 * A carried trait names its file in its data (set when it was added), so it
 * resolves wherever the living stands; a new one is looked for with
 * trait_path.
 *
 * @param  id    the trait id, which is also its file name without ".c"
 * @param  data  the data of a trait already carried, nil for a new one
 */
private object trait_object(string id, varargs mapping data)
{
  object ob;
  string path;

  ob = nil;

  if (data && stringp(data["file"]))
    path = data["file"];
  else
    path = trait_path(id);

  if (strlen(path))
    catch(ob = load_object(path));

  return ob;
}

/**
 * Adds up what every trait carried contributes and hands the stat deltas to
 * stats.c. Called after every change, so the queries never walk the traits.
 */
private void rebuild_traits()
{
  mapping sum_stats, sum_flags, fx, positions;
  string * ids, * names;
  mixed * flags;
  object ob;
  int * deltas;
  int i, j, pos;

  sum_stats = ([ ]);
  sum_flags = ([ ]);
  ids = map_indices(traits);

  for (i = 0; i < sizeof(ids); i++)
  {
    ob = trait_object(ids[i], traits[ids[i]]);
    if (!ob)
      continue;

    fx = nil;
    catch(fx = (mapping)ob->trait_effects(this_object(), traits[ids[i]]));
    if (!fx)
      continue;

    if (mappingp(fx[TRAIT_EFFECT_STATS]))
    {
      names = map_indices(fx[TRAIT_EFFECT_STATS]);

      for (j = 0; j < sizeof(names); j++)
      {
        if (undefinedp(sum_stats[names[j]]))
          sum_stats[names[j]] = fx[TRAIT_EFFECT_STATS][names[j]];
        else
          sum_stats[names[j]] += fx[TRAIT_EFFECT_STATS][names[j]];
      }
    }

    if (arrayp(fx[TRAIT_EFFECT_FLAGS]))
    {
      flags = fx[TRAIT_EFFECT_FLAGS];

      for (j = 0; j < sizeof(flags); j++)
      {
        if (undefinedp(sum_flags[flags[j]]))
          sum_flags[flags[j]] = 1;
        else
          sum_flags[flags[j]] += 1;
      }
    }
  }

  trait_summary = ([ "stats" : sum_stats, "flags" : sum_flags ]);

  // the stat deltas, by position, to the place that sums the stats
  positions = TRAIT_STATS;
  deltas = allocate_int(NUM_STATS);
  names = map_indices(sum_stats);

  for (i = 0; i < sizeof(names); i++)
  {
    if (undefinedp(positions[names[i]]))
      continue;

    pos = (int)positions[names[i]];
    deltas[pos] = (int)sum_stats[names[i]];
  }

  this_object()->set_trait_stats(deltas);
}

/**
 * Adds a trait.
 *
 * The data is copied and stored under the id; keep to values a savefile can
 * hold (numbers, strings, arrays and mappings of them). A source given as an
 * object is stored by its object name, since objects do not save. "expires"
 * (an absolute time()) is optional. A living that already carries the id asks
 * the trait which data to keep (trait_merge).
 *
 * @param  id    the trait id, which is also its file name without ".c"
 * @param  data  what the trait needs to run; nil for none
 * @return 1 if the living now carries this data, 0 if the id resolves to no
 *         trait or the one carried was kept
 */
int add_trait(string id, varargs mapping data)
{
  object ob;
  mapping old, kept;
  string err;

  ob = trait_object(id);
  if (!ob)
    return 0;

  data = data ? ([ ]) + data : ([ ]);

  if (objectp(data[TRAIT_DATA_SOURCE]))
    data[TRAIT_DATA_SOURCE] = object_name(data[TRAIT_DATA_SOURCE]);

  // the file it resolved to, so it resolves again wherever the living stands
  data["file"] = trait_path(id);

  old = traits[id];

  if (old)
  {
    kept = old;
    err = catch(kept = (mapping)ob->trait_merge(this_object(), old, data));
    if (err)
      debug("traits", "trait_merge of " + id + ": " + err + "\n");

    // the one carried stays
    if (!kept || kept == old)
      return 0;

    data = kept;
    err = catch(ob->trait_removed(this_object(), old, TRAIT_END_REPLACED));
    if (err)
      debug("traits", "trait_removed of " + id + ": " + err + "\n");
  }

  traits[id] = data;
  rebuild_traits();

  err = catch(ob->trait_added(this_object(), data));
  if (err)
    debug("traits", "trait_added of " + id + ": " + err + "\n");

  // a resting npc has its heart_beat off, and nothing would tick
  if (this_object() && !query_heart_beat())
    set_heart_beat(1);

  return 1;
}

/**
 * Removes a trait, telling it why (TRAIT_END_*).
 *
 * The trait leaves the living before trait_removed runs, so the hook may add
 * it again.
 *
 * @param  id      the trait id
 * @param  reason  why it ends; "removed" when not given
 * @return 1 if the living carried it
 */
int remove_trait(string id, varargs string reason)
{
  object ob;
  mapping data;
  string err;

  data = traits[id];
  if (!data)
    return 0;

  if (!reason)
    reason = TRAIT_END_REMOVED;

  traits[id] = nil;
  rebuild_traits();

  ob = trait_object(id, data);
  if (!ob)
    return 1;

  err = catch(ob->trait_removed(this_object(), data, reason));
  if (err)
    debug("traits", "trait_removed of " + id + ": " + err + "\n");

  return 1;
}

/** Whether the living carries the trait. */
int has_trait(string id)
{
  return traits[id] != nil;
}

/** A copy of the data of a trait the living carries, nil when it does not. */
mapping query_trait(string id)
{
  if (!traits[id])
    return nil;

  return ([ ]) + traits[id];
}

/**
 * Every trait carried.
 *
 * @return id -> data, a copy
 */
mapping query_traits()
{
  return ([ ]) + traits;
}

/**
 * Removes every trait, telling each why.
 *
 * @param  reason  TRAIT_END_*; "removed" when not given
 */
void clear_traits(varargs string reason)
{
  string * ids;
  int i;

  ids = map_indices(traits);

  for (i = 0; i < sizeof(ids); i++)
    remove_trait(ids[i], reason);
}

/**
 * Brings the traits back into order after the living was restored from its
 * savefile: a missing mapping is made, the summary is rebuilt, and a living
 * that carries traits gets its heart_beat.
 */
void restore_traits()
{
  if (!mappingp(traits))
    traits = ([ ]);

  rebuild_traits();

  if (map_sizeof(traits) && !query_heart_beat())
    set_heart_beat(1);
}

/**
 * How many carried traits raise a state flag (TRAIT_EFFECT_FLAGS); 0 for none.
 */
int query_trait_flag(string flag)
{
  mapping flags;

  flags = trait_summary[TRAIT_EFFECT_FLAGS];
  return undefinedp(flags[flag]) ? 0 : (int)flags[flag];
}

/**
 * What the traits add to a stat ("str", "dex"...), as summed on the last
 * change.
 */
int query_trait_stat(string name)
{
  mapping sum;

  sum = trait_summary[TRAIT_EFFECT_STATS];
  return undefinedp(sum[name]) ? 0 : (int)sum[name];
}

/**
 * The lines the carried traits add to how the living looks (trait_look), each
 * ending in a newline; "" when there are none.
 */
string query_trait_look()
{
  string * ids;
  string ret, line;
  object ob;
  int i;

  ret = "";
  ids = map_indices(traits);

  for (i = 0; i < sizeof(ids); i++)
  {
    ob = trait_object(ids[i], traits[ids[i]]);
    if (!ob)
      continue;

    line = nil;
    catch(line = (string)ob->trait_look(this_object(), traits[ids[i]]));

    if (!stringp(line) || !strlen(line))
      continue;

    ret += line;
    if (line[strlen(line) - 1] != '\n')
      ret += "\n";
  }

  return ret;
}

/**
 * One heart_beat of every carried trait: an expired one ends, the others get
 * trait_tick, and one whose tick returns 1 ends too. The living's heart_beat
 * calls it; with no traits it returns at once.
 */
void tick_traits()
{
  string * ids;
  mapping data;
  object ob;
  mixed result;
  string err;
  int i;

  if (!map_sizeof(traits))
    return;

  ids = map_indices(traits);

  for (i = 0; i < sizeof(ids); i++)
  {
    // an earlier tick may have ended this one
    data = traits[ids[i]];
    if (!data)
      continue;

    if (data[TRAIT_DATA_EXPIRES] && (int)data[TRAIT_DATA_EXPIRES] <= time())
    {
      remove_trait(ids[i], TRAIT_END_EXPIRED);
      continue;
    }

    ob = trait_object(ids[i], data);
    if (!ob)
    {
      remove_trait(ids[i], TRAIT_END_ERROR);
      continue;
    }

    result = nil;
    err = catch(result = ob->trait_tick(this_object(), data));

    // the tick may have destructed this living
    if (!this_object())
      return;

    // the tick ended or replaced its own trait, and that call decided
    if (traits[ids[i]] != data)
      continue;

    if (err)
    {
      debug("traits", "trait_tick of " + ids[i] + " in " +
        object_name(this_object()) + ": " + err + "\n");
      remove_trait(ids[i], TRAIT_END_ERROR);
    }
    else if (result)
      remove_trait(ids[i], TRAIT_END_EXPIRED);

    if (!this_object())
      return;
  }
}

mixed * stats()
{
  mixed * ret;
  string * ids;
  int i;

  ret = ({ });
  ids = map_indices(traits);

  for (i = 0; i < sizeof(ids); i++)
    ret += ({ ({ "Trait " + ids[i], json_encode(traits[ids[i]]), }) });

  return ret;
}
