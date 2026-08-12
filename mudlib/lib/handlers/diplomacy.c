/*
 * Diplomacy handler.
 *
 * The single oracle for how citizenships relate, and the owner of that graph.
 * A citizenship is a lean social object (/lib/citizenship.c, inherited by
 * /games/<game>/obj/citizenships/*), carried by a living as its city_ob; it
 * stores no relation data. It all lives here as mutable, persisted state.
 *
 * Everything is keyed by the citizenship name (its query_name, e.g. the id used
 * in every mapping here) within a game. Callers work in names: a caller holding
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

// A mapping of mappings, indexed by game name. Each game's value is itself a
// mapping indexed by citizenship name, whose value is that citizenship's data:
//   citizenships[game][name] == ([ "parent"   : parent_name (or ""),
//                                  "security" : guard_count,
//                                  "guard"    : guard_npc_path (or "") ])
mapping citizenships;

// Indexed by game name. Each game's value holds the two relationship lists,
// each a list of unordered name pairs (every relationship stored once):
//   relationships[game] == ([ DIPLOMACY_RELATION_ENEMY : ({ ({a, b}), ... }),
//                              DIPLOMACY_RELATION_ALLY  : ({ ({a, b}), ... }) ])
mapping relationships;

void create()
{
  citizenships = ([ ]);
  relationships = ([ ]);
  ::create();
  restore_object(DIPLOMACY_SAVE, 1);
}

void setup()
{
  // anticloning, like the other handlers
  if (file_name(this_object()) != DIPLOMACY_HANDLER)
  {
    dest_me();
    return;
  }
}

void save_handler()
{
  save_object(DIPLOMACY_SAVE, 1);
}

void dest_me()
{
  save_handler();
  ::dest_me();
}

// A game's citizenship map ([ name : citizenship_data ]), empty if the game has
// no citizenships yet.
private mapping game_citizenships(string game)
{
  if (!game || !citizenships[game])
    return ([ ]);
  return citizenships[game];
}

// A game's relationship store, always with both type lists present.
private mapping game_relationships(string game)
{
  mapping store;
  store = game ? relationships[game] : nil;
  if (!store)
    return ([ DIPLOMACY_RELATION_ENEMY : ({ }), DIPLOMACY_RELATION_ALLY : ({ }) ]);
  return store;
}

// --- Query side: everything is a citizenship name within a game ------------

// The parent of a citizenship (the one it sits under), or "" if it is top-level.
string query_parent(string game, string citizenship_name)
{
  mapping citizenship;
  citizenship = game_citizenships(game)[citizenship_name];
  return (citizenship && stringp(citizenship["parent"])) ? citizenship["parent"] : "";
}

// Does the game's `type` list hold the pair {name_a, name_b}? Each pair is one
// stored ally/enemy relationship; the pair is unordered, so the order of the two
// names does not matter. This checks the single stored pair only -- it does not
// follow the parent cascade (see check_citizenships_linked()).
private int check_citizenships_linked_by_type(string game, string type,
                                              string name_a, string name_b)
{
  mixed * pairs;
  int i;

  pairs = game_relationships(game)[type];
  for (i = 0; i < sizeof(pairs); i++)
    if ((pairs[i][0] == name_a && pairs[i][1] == name_b) ||
        (pairs[i][0] == name_b && pairs[i][1] == name_a))
      return 1;
  return 0;
}

// The citizenships directly `type`-linked to `citizenship_name` in a game (the
// other name in each of its pairs). Direct relationships only -- relationships a
// citizenship only has through its parent are resolved by is_enemy / is_ally,
// not listed here.
private string * query_linked_names(string game, string type,
                                    string citizenship_name)
{
  mixed * pairs;
  string * others;
  int i;

  pairs = game_relationships(game)[type];
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

// Are name_a and name_b `type`-linked in `game`, directly or through a parent?
// The relationship counts if it is declared between the two citizenships
// themselves, between one and the other's parent, or between their two parents.
// is_enemy / is_ally build on this; the single-pair check without the parent
// cascade is check_citizenships_linked_by_type.
private int check_citizenships_linked(string game, string type,
                                      string name_a, string name_b)
{
  string parent_a, parent_b;

  parent_a = query_parent(game, name_a);
  parent_b = query_parent(game, name_b);

  return check_citizenships_linked_by_type(game, type, name_a, name_b) ||
         (strlen(parent_b) &&
          check_citizenships_linked_by_type(game, type, name_a, parent_b)) ||
         (strlen(parent_a) &&
          check_citizenships_linked_by_type(game, type, parent_a, name_b)) ||
         (strlen(parent_a) && strlen(parent_b) &&
          check_citizenships_linked_by_type(game, type, parent_a, parent_b));
}

// Is `name_b` an enemy of `name_a` (both citizenship names in `game`)?
// Enmity cascades through the parent: a citizenship is at war with whatever its
// parent is at war with.
int is_enemy(string game, string name_a, string name_b)
{
  if (!strlen(name_a) || !strlen(name_b))
    return 0;
  return check_citizenships_linked(game, DIPLOMACY_RELATION_ENEMY,
                                   name_a, name_b);
}

// Is `name_b` an ally of `name_a` -- the same citizenship, two siblings under
// one parent, a citizenship and its own parent, or an explicit ally? Alliances
// cascade through the parent just as enmity does.
int is_ally(string game, string name_a, string name_b)
{
  string parent_a, parent_b;

  if (!strlen(name_a) || !strlen(name_b))
    return 0;
  if (name_a == name_b)
    return 1;                        // the same citizenship

  // a citizenship allied to its own parent, or two siblings under one parent
  parent_a = query_parent(game, name_a);
  parent_b = query_parent(game, name_b);
  if (strlen(parent_a) && (parent_a == name_b || parent_a == parent_b))
    return 1;
  if (strlen(parent_b) && parent_b == name_a)
    return 1;

  return check_citizenships_linked(game, DIPLOMACY_RELATION_ALLY,
                                   name_a, name_b);
}

// How many guards this citizenship fields at each of its guarded POIs.
int query_security_level(string game, string citizenship_name)
{
  mapping citizenship;
  citizenship = game_citizenships(game)[citizenship_name];
  return (citizenship && intp(citizenship["security"])) ? citizenship["security"] : 0;
}

// The file path of the NPC this citizenship's guards are cloned from, or "" if
// none. Guards follow the citizenship, not the area: change an area's
// citizenship and its guards respawn from the new citizenship's guard.
string query_guard_path(string game, string citizenship_name)
{
  mapping citizenship;
  citizenship = game_citizenships(game)[citizenship_name];
  return (citizenship && stringp(citizenship["guard"])) ? citizenship["guard"] : "";
}

// The citizenships this one is directly allied with / at war with (direct relationships
// only; parent-inherited relations are resolved by is_ally / is_enemy).
string * query_allies(string game, string citizenship_name)
{
  return query_linked_names(game, DIPLOMACY_RELATION_ALLY, citizenship_name);
}

string * query_enemies(string game, string citizenship_name)
{
  return query_linked_names(game, DIPLOMACY_RELATION_ENEMY, citizenship_name);
}

// Read-only view of a game's whole graph for inspection commands: one entry per
// citizenship with its data plus its direct allies and enemies pulled from the
// relationships. Built fresh each call so callers cannot mutate the stored graph.
mapping query_relations(string game)
{
  mapping citizenship_map, view;
  string * names;
  int i;

  citizenship_map = game_citizenships(game);
  view = ([ ]);
  names = map_indices(citizenship_map);
  for (i = 0; i < sizeof(names); i++)
  {
    string name;
    name = names[i];
    view[name] = ([ "parent"   : citizenship_map[name]["parent"],
                    "security" : citizenship_map[name]["security"],
                    "guard"    : citizenship_map[name]["guard"],
                    "allies"   : query_linked_names(game, DIPLOMACY_RELATION_ALLY, name),
                    "enemies"  : query_linked_names(game, DIPLOMACY_RELATION_ENEMY, name) ]);
  }
  return view;
}

// Names of every citizenship defined for a game.
string * query_citizenships(string game)
{
  return map_indices(game_citizenships(game));
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

// Create a citizenship in a game's graph (no-op if it already exists). The
// first citizenship of a game creates that game's citizenship map; the game's
// relationship lists are created lazily by add_relationship. The two maps are
// created independently, so neither restore nor either operation can leave one
// present and the other assumed.
int add_citizenship(string game, string citizenship_name)
{
  if (!game || !strlen(game) || !citizenship_name || !strlen(citizenship_name))
    return 0;

  if (!citizenships[game])
    citizenships[game] = ([ ]);

  if (!citizenships[game][citizenship_name])
    citizenships[game][citizenship_name] =
      ([ "parent" : "", "security" : 0, "guard" : "" ]);

  save_handler();
  return 1;
}

// Drop a citizenship: remove it and every relationship that names it.
int remove_citizenship(string game, string citizenship_name)
{
  string * types;
  int t;

  if (!citizenships[game] || !citizenships[game][citizenship_name])
    return 0;

  citizenships[game][citizenship_name] = nil;

  // drop every pair that names this citizenship, from both type lists (the
  // game may have no relationships yet, so its lists may not exist)
  types = ({ DIPLOMACY_RELATION_ENEMY, DIPLOMACY_RELATION_ALLY });
  for (t = 0; relationships[game] && t < sizeof(types); t++)
  {
    mixed * pairs, * kept;
    int i;
    pairs = relationships[game][types[t]];
    kept = ({ });
    for (i = 0; i < sizeof(pairs); i++)
      if (pairs[i][0] != citizenship_name && pairs[i][1] != citizenship_name)
        kept += ({ pairs[i] });
    relationships[game][types[t]] = kept;
  }

  save_handler();
  return 1;
}

// The stored data of an existing citizenship, or nil if it is not one. Used by
// the setters below, which only edit a citizenship that already exists (created
// beforehand with add_citizenship).
private mapping query_citizenship(string game, string citizenship_name)
{
  return citizenships[game] ? citizenships[game][citizenship_name] : nil;
}

// Set a citizenship's parent (the one it sits under). "" makes it top-level.
int set_parent(string game, string citizenship_name, string parent_name)
{
  mapping citizenship;
  citizenship = query_citizenship(game, citizenship_name);
  if (!citizenship)
    return 0;
  citizenship["parent"] = parent_name ? parent_name : "";
  save_handler();
  return 1;
}

int set_security(string game, string citizenship_name, int guard_count)
{
  mapping citizenship;
  citizenship = query_citizenship(game, citizenship_name);
  if (!citizenship)
    return 0;
  citizenship["security"] = guard_count < 0 ? 0 : guard_count;
  save_handler();
  return 1;
}

int set_guard(string game, string citizenship_name, string guard_npc_path)
{
  mapping citizenship;
  citizenship = query_citizenship(game, citizenship_name);
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
int add_relationship(string game, string type, string name_a, string name_b)
{
  mapping type_lists;
  string opposite_type;

  if (name_a == name_b || !citizenships[game] ||
      !citizenships[game][name_a] || !citizenships[game][name_b])
    return 0;

  // the game's relationship lists are born here, with its first relationship
  if (!relationships[game])
    relationships[game] = ([ DIPLOMACY_RELATION_ENEMY : ({ }),
                             DIPLOMACY_RELATION_ALLY  : ({ }) ]);

  type_lists = relationships[game];
  opposite_type = (type == DIPLOMACY_RELATION_ENEMY) ? DIPLOMACY_RELATION_ALLY
                                                     : DIPLOMACY_RELATION_ENEMY;

  type_lists[opposite_type] =
    _filter_pair(type_lists[opposite_type], name_a, name_b);
  if (!check_citizenships_linked_by_type(game, type, name_a, name_b))
    type_lists[type] += ({ (name_a < name_b) ? ({ name_a, name_b })
                                             : ({ name_b, name_a }) });
  save_handler();
  return 1;
}

// Remove the `type` relationship between two citizenships, if present.
int remove_relationship(string game, string type, string name_a, string name_b)
{
  if (!relationships[game])
    return 0;
  relationships[game][type] =
    _filter_pair(relationships[game][type], name_a, name_b);
  save_handler();
  return 1;
}
