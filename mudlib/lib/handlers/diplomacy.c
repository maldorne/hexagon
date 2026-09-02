/*
 * Diplomacy handler.
 *
 * The single oracle for how citizenships relate, and the owner of that graph.
 * A citizenship is a lean social object (/lib/citizenship.c, inherited by
 * /games/<game>/obj/citizenships/*), carried by a living as its city_ob; it
 * stores no relation data. It all lives here as mutable, persisted state.
 *
 * Every game has its own handler: /games/<game>/handlers/diplomacy.c inherits
 * this one and points query_save_file at its own file, so the countries at war
 * in one world have nothing to do with another's. Reach it with
 * handler("diplomacy", ob) -- never by path -- so the object you get is the one
 * belonging to ob's game.
 *
 * Everything is keyed by the citizenship name (its query_name, e.g. the id used
 * in every mapping here). Callers work in names: a caller holding
 * a citizenship object or path resolves it to a name once (load the object, ask
 * query_name / game_name) and passes names in.
 *
 * The graph has two parts, both persisted:
 *
 *   - Citizenship data (the `citizenships` mapping): the facts that belong to
 *     one citizenship alone -- its parent, how many guards it fields (security)
 *     and the file path of the NPC its guards are cloned from (guard). A
 *     citizenship's parent is the one it sits under (a town under its country or
 *     faction); the top-level ones have no parent.
 *
 *   - Relationships (the `relationships` mapping): who is ally or enemy of
 *     whom. A relationship is mutual, so it is stored once, as an unordered pair
 *     of names, instead of on both citizenships. One add or remove edits it.
 *
 * The graph is dynamic (a town can be invaded, change parent, gain enemies), so
 * it is never read from a static table; it is built and edited by hand through
 * admin commands and saved to disk. It starts empty for every game.
 */

#include <mud/config.h>
#include <areas/diplomacy.h>

inherit "/lib/core/object.c";

//   citizenships[name] == ([ "parent"   : parent_name (or ""),
//                            "security" : guard_count,
//                            "guard"    : guard_npc_path (or ""),
//                            "deities"  : ({ deity paths }) ])
mapping citizenships;

// The two relationship lists, each a list of unordered name pairs (every
// relationship stored once):
//   relationships == ([ DIPLOMACY_RELATION_ENEMY : ({ ({a, b}), ... }),
//                       DIPLOMACY_RELATION_ALLY  : ({ ({a, b}), ... }) ])
mapping relationships;

string query_save_file();

void create()
{
  citizenships = ([ ]);
  relationships = ([ ]);
  ::create();
  restore_object(query_save_file(), 1);
}

// Where this graph is kept. A game's own handler overrides it to point at
// /save/games/<game>/diplomacy.o; the shared one answers for anybody without.
string query_save_file()
{
  return DIPLOMACY_SAVE;
}

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance loaded through handler() should
  // exist. The path is not compared, so a game's own subclass passes; any
  // actual clone (file_name suffixed with #N) is destroyed.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
    dest_me();
}

void save_handler()
{
  save_object(query_save_file(), 1);
}

void dest_me()
{
  save_handler();
  ::dest_me();
}

// This game's citizenship map ([ name : citizenship_data ]), empty until one is
// added.
// defined further down, where the mutating side of the graph lives
private mapping query_citizenship(string citizenship_name);

private mapping citizenship_map()
{
  return citizenships;
}

// The relationship store, always with both type lists present.
private mapping relationship_map()
{
  if (!map_sizeof(relationships))
    relationships = ([ DIPLOMACY_RELATION_ENEMY : ({ }),
                       DIPLOMACY_RELATION_ALLY  : ({ }) ]);

  return relationships;
}

// --- Query side: everything is a citizenship name ---------------------------

// The parent of a citizenship (the one it sits under), or "" if it is top-level.
string query_parent(string citizenship_name)
{
  mapping citizenship;
  citizenship = citizenship_map()[citizenship_name];
  return (citizenship && stringp(citizenship["parent"])) ? citizenship["parent"] : "";
}

// Does the `type` list hold the pair {name_a, name_b}? Each pair is one
// stored ally/enemy relationship; the pair is unordered, so the order of the two
// names does not matter. This checks the single stored pair only -- it does not
// follow the parent cascade (see check_citizenships_linked()).
private int check_citizenships_linked_by_type(string type,
                                              string name_a, string name_b)
{
  mixed * pairs;
  int i;

  pairs = relationship_map()[type];
  for (i = 0; i < sizeof(pairs); i++)
    if ((pairs[i][0] == name_a && pairs[i][1] == name_b) ||
        (pairs[i][0] == name_b && pairs[i][1] == name_a))
      return 1;
  return 0;
}

// The citizenships directly `type`-linked to `citizenship_name` (the
// other name in each of its pairs). Direct relationships only -- relationships a
// citizenship only has through its parent are resolved by is_enemy / is_ally,
// not listed here.
private string * query_linked_names(string type,
                                    string citizenship_name)
{
  mixed * pairs;
  string * others;
  int i;

  pairs = relationship_map()[type];
  others = ({ });
  for (i = 0; i < sizeof(pairs); i++)
  {
    if (pairs[i][0] == citizenship_name)
      others += ({ pairs[i][1] });
    else if (pairs[i][1] == citizenship_name)
      others += ({ pairs[i][0] });
  }
  return others;
}


// The top of a citizenship's parent chain: the country a town or a faction
// ultimately belongs to. A top-level citizenship is its own root. The walk is
// bounded so a graph someone edited into a cycle cannot hang the driver.
string query_root_citizenship(string citizenship_name)
{
  string current, parent;
  int steps;

  current = citizenship_name;

  for (steps = 0; steps < DIPLOMACY_MAX_DEPTH; steps++)
  {
    parent = query_parent(current);

    if (!strlen(parent) || parent == current)
      return current;

    current = parent;
  }

  return current;
}

// Are name_a and name_b `type`-linked, directly or through a parent?
// The relationship counts if it is declared between the two citizenships
// themselves, between one and the other's parent, or between their two parents.
// is_enemy / is_ally build on this; the single-pair check without the parent
// cascade is check_citizenships_linked_by_type.
private int check_citizenships_linked(string type,
                                      string name_a, string name_b)
{
  string parent_a, parent_b;

  parent_a = query_parent(name_a);
  parent_b = query_parent(name_b);

  return check_citizenships_linked_by_type(type, name_a, name_b) ||
         (strlen(parent_b) &&
          check_citizenships_linked_by_type(type, name_a, parent_b)) ||
         (strlen(parent_a) &&
          check_citizenships_linked_by_type(type, parent_a, name_b)) ||
         (strlen(parent_a) && strlen(parent_b) &&
          check_citizenships_linked_by_type(type, parent_a, parent_b));
}

// Is `name_b` an enemy of `name_a`?
// Enmity cascades through the parent: a citizenship is at war with whatever its
// parent is at war with.
int is_enemy(string name_a, string name_b)
{
  if (!strlen(name_a) || !strlen(name_b))
    return 0;
  return check_citizenships_linked(DIPLOMACY_RELATION_ENEMY,
                                   name_a, name_b);
}

// Is `name_b` an ally of `name_a` -- the same citizenship, two siblings under
// one parent, a citizenship and its own parent, or an explicit ally? Alliances
// cascade through the parent just as enmity does.
int is_ally(string name_a, string name_b)
{
  string parent_a, parent_b;

  if (!strlen(name_a) || !strlen(name_b))
    return 0;
  if (name_a == name_b)
    return 1;                        // the same citizenship

  // a citizenship allied to its own parent, or two siblings under one parent
  parent_a = query_parent(name_a);
  parent_b = query_parent(name_b);
  if (strlen(parent_a) && (parent_a == name_b || parent_a == parent_b))
    return 1;
  if (strlen(parent_b) && parent_b == name_a)
    return 1;

  return check_citizenships_linked(DIPLOMACY_RELATION_ALLY,
                                   name_a, name_b);
}

// How many guards this citizenship fields at each of its guarded POIs.
int query_security_level(string citizenship_name)
{
  mapping citizenship;
  citizenship = citizenship_map()[citizenship_name];
  return (citizenship && intp(citizenship["security"])) ? citizenship["security"] : 0;
}

// The file path of the NPC this citizenship's guards are cloned from, or "" if
// none. Guards follow the citizenship, not the area: change an area's
// citizenship and its guards respawn from the new citizenship's guard.
string query_guard_path(string citizenship_name)
{
  mapping citizenship;
  citizenship = citizenship_map()[citizenship_name];
  return (citizenship && stringp(citizenship["guard"])) ? citizenship["guard"] : "";
}

// The deities this citizenship accepts, walking up to its parent when it
// declares none of its own: a village keeps the faiths of the realm it belongs
// to until it decides otherwise. An empty answer means it has never been asked
// to choose, which is not the same as accepting nothing -- see accepts_deity.
string * query_deities(string citizenship_name)
{
  string current;
  int steps;

  current = citizenship_name;

  for (steps = 0; steps < DIPLOMACY_MAX_DEPTH; steps++)
  {
    mapping citizenship;
    string parent;

    citizenship = citizenship_map()[current];
    if (citizenship && pointerp(citizenship[DIPLOMACY_DEITIES]) &&
        sizeof(citizenship[DIPLOMACY_DEITIES]))
      return citizenship[DIPLOMACY_DEITIES];

    parent = query_parent(current);
    if (!strlen(parent) || parent == current)
      break;
    current = parent;
  }

  return ({ });
}

// Whether this citizenship will have that deity worshipped on its ground. A
// citizenship that has declared no faiths at all objects to none of them: the
// question only starts to bite once somebody has said what the place believes.
int accepts_deity(string citizenship_name, string deity)
{
  string * accepted;

  if (!deity || !strlen(deity))
    return 1;

  accepted = query_deities(citizenship_name);
  if (!sizeof(accepted))
    return 1;

  return member_array(deity, accepted) != -1;
}

int add_deity(string citizenship_name, string deity)
{
  mapping citizenship;

  citizenship = query_citizenship(citizenship_name);
  if (!citizenship || !deity || !strlen(deity))
    return 0;

  if (!pointerp(citizenship[DIPLOMACY_DEITIES]))
    citizenship[DIPLOMACY_DEITIES] = ({ });
  if (member_array(deity, citizenship[DIPLOMACY_DEITIES]) != -1)
    return 0;

  citizenship[DIPLOMACY_DEITIES] += ({ deity });
  save_handler();
  return 1;
}

int remove_deity(string citizenship_name, string deity)
{
  mapping citizenship;

  citizenship = query_citizenship(citizenship_name);
  if (!citizenship || !pointerp(citizenship[DIPLOMACY_DEITIES]) ||
      member_array(deity, citizenship[DIPLOMACY_DEITIES]) == -1)
    return 0;

  citizenship[DIPLOMACY_DEITIES] -= ({ deity });
  save_handler();
  return 1;
}

// The citizenships this one is directly allied with / at war with (direct relationships
// only; parent-inherited relations are resolved by is_ally / is_enemy).
string * query_allies(string citizenship_name)
{
  return query_linked_names(DIPLOMACY_RELATION_ALLY, citizenship_name);
}

string * query_enemies(string citizenship_name)
{
  return query_linked_names(DIPLOMACY_RELATION_ENEMY, citizenship_name);
}

// Read-only view of the whole graph for inspection commands: one entry per
// citizenship with its data plus its direct allies and enemies pulled from the
// relationships. Built fresh each call so callers cannot mutate the stored graph.
mapping query_relations()
{
  mapping citizenship_map, view;
  string * names;
  int i;

  citizenship_map = citizenship_map();
  view = ([ ]);
  names = map_indices(citizenship_map);
  for (i = 0; i < sizeof(names); i++)
  {
    string name;
    name = names[i];
    view[name] = ([ "parent"   : citizenship_map[name]["parent"],
                    "security" : citizenship_map[name]["security"],
                    "guard"    : citizenship_map[name]["guard"],
                    DIPLOMACY_DEITIES : citizenship_map[name][DIPLOMACY_DEITIES],
                    "allies"   : query_linked_names(DIPLOMACY_RELATION_ALLY, name),
                    "enemies"  : query_linked_names(DIPLOMACY_RELATION_ENEMY, name) ]);
  }
  return view;
}

// Names of every citizenship defined here.
string * query_citizenships()
{
  return map_indices(citizenship_map());
}

// --- Mutating side: the admin command builds the graph by name -------------

// A copy of the `pairs` list with the pair {name_a, name_b} taken out (the pair
// is unordered, so the order of the two names does not matter). Used to drop one
// relationship. Callers reassign the result: list = _filter_pair(list, a, b).
private mixed * _filter_pair(mixed * pairs, string name_a, string name_b)
{
  mixed * kept;
  int i;

  kept = ({ });
  for (i = 0; i < sizeof(pairs); i++)
    if (!((pairs[i][0] == name_a && pairs[i][1] == name_b) ||
          (pairs[i][0] == name_b && pairs[i][1] == name_a)))
      kept += ({ pairs[i] });
  return kept;
}

// Create a citizenship (no-op if it already exists). The game's
// relationship lists are created lazily by add_relationship. The two maps are
// created independently, so neither restore nor either operation can leave one
// present and the other assumed.
int add_citizenship(string citizenship_name)
{
  if (!citizenship_name || !strlen(citizenship_name))
    return 0;

  if (!citizenships[citizenship_name])
    citizenships[citizenship_name] =
      ([ "parent" : "", "security" : 0, "guard" : "" ]);

  save_handler();
  return 1;
}

// Drop a citizenship: remove it and every relationship that names it.
int remove_citizenship(string citizenship_name)
{
  string * types;
  int t;

  if (!citizenships[citizenship_name])
    return 0;

  citizenships[citizenship_name] = nil;

  // drop every pair that names this citizenship, from both type lists (the
  // game may have no relationships yet, so its lists may not exist)
  types = ({ DIPLOMACY_RELATION_ENEMY, DIPLOMACY_RELATION_ALLY });
  for (t = 0; map_sizeof(relationships) && t < sizeof(types); t++)
  {
    mixed * pairs, * kept;
    int i;
    pairs = relationships[types[t]];
    kept = ({ });
    for (i = 0; i < sizeof(pairs); i++)
      if (pairs[i][0] != citizenship_name && pairs[i][1] != citizenship_name)
        kept += ({ pairs[i] });
    relationships[types[t]] = kept;
  }

  save_handler();
  return 1;
}

// The stored data of an existing citizenship, or nil if it is not one. Used by
// the setters below, which only edit a citizenship that already exists (created
// beforehand with add_citizenship).
private mapping query_citizenship(string citizenship_name)
{
  return citizenships[citizenship_name];
}

// Set a citizenship's parent (the one it sits under). "" makes it top-level.
int set_parent(string citizenship_name, string parent_name)
{
  mapping citizenship;
  citizenship = query_citizenship(citizenship_name);
  if (!citizenship)
    return 0;
  citizenship["parent"] = parent_name ? parent_name : "";
  save_handler();
  return 1;
}

int set_security(string citizenship_name, int guard_count)
{
  mapping citizenship;
  citizenship = query_citizenship(citizenship_name);
  if (!citizenship)
    return 0;
  citizenship["security"] = guard_count < 0 ? 0 : guard_count;
  save_handler();
  return 1;
}

int set_guard(string citizenship_name, string guard_npc_path)
{
  mapping citizenship;
  citizenship = query_citizenship(citizenship_name);
  if (!citizenship)
    return 0;
  citizenship["guard"] = guard_npc_path ? guard_npc_path : "";
  save_handler();
  return 1;
}

// Store a mutual `type` relationship (ally or enemy) between two citizenships,
// once. The pair is first dropped from the opposite type (an ally is not an
// enemy, and vice versa) and stored name-sorted so the two orders collapse to
// one entry. Both must be citizenships of the game (and not the same one).
int add_relationship(string type, string name_a, string name_b)
{
  mapping type_lists;
  string opposite_type;

  if (name_a == name_b || !citizenships[name_a] || !citizenships[name_b])
    return 0;

  // the game's relationship lists are born here, with its first relationship
  if (!map_sizeof(relationships))
    relationships = ([ DIPLOMACY_RELATION_ENEMY : ({ }),
                             DIPLOMACY_RELATION_ALLY  : ({ }) ]);

  type_lists = relationships;
  opposite_type = (type == DIPLOMACY_RELATION_ENEMY) ? DIPLOMACY_RELATION_ALLY
                                                     : DIPLOMACY_RELATION_ENEMY;

  type_lists[opposite_type] =
    _filter_pair(type_lists[opposite_type], name_a, name_b);
  if (!check_citizenships_linked_by_type(type, name_a, name_b))
    type_lists[type] += ({ (name_a < name_b) ? ({ name_a, name_b })
                                             : ({ name_b, name_a }) });
  save_handler();
  return 1;
}

// Remove the `type` relationship between two citizenships, if present.
int remove_relationship(string type, string name_a, string name_b)
{
  if (!map_sizeof(relationships))
    return 0;
  relationships[type] = _filter_pair(relationships[type], name_a, name_b);
  save_handler();
  return 1;
}
