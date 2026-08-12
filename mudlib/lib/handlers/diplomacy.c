/*
 * Diplomacy handler.
 *
 * The single oracle for how citizenships relate, and the owner of that graph.
 * A citizenship is a lean social object (/lib/citizenship.c, inherited by
 * /games/<game>/obj/citizenships/*), carried by a living as its city_ob; it
 * stores no relation data. It all lives here as mutable, persisted state.
 *
 * The graph has two parts, both persisted:
 *
 *   - Nodes (`relations`): per-citizenship data that is one-sided by nature --
 *     its parent kingdom, its guard count (security) and its guard NPC source.
 *
 *   - Links (`links`): ally / enemy relations between two citizenships. A
 *     relation is mutual, so it is stored once, as an unordered pair, rather
 *     than duplicated on both records. Adding or removing it is a single edit.
 *
 * The graph is dynamic (a town can be invaded, change kingdom, gain enemies),
 * so it is never read from a static table; it is built and edited by hand
 * through admin commands and saved to disk. It starts empty for every game.
 *
 * References passed to the query side may be a citizenship object, its object
 * path, or a bare name; they resolve to a { name, game } pair. A living with
 * no citizenship (city_ob nil) is neutral -- never an enemy.
 */

#include <mud/config.h>
#include <areas/diplomacy.h>

inherit "/lib/core/object.c";

// game -> ([ name : ([ "parent":.., "security":n, "guard":npc_path ]) ]
mapping relations;
// game -> ([ "enemy" : ({ ({a,b}), ... }), "ally" : ({ ({a,b}), ... }) ]
// Each relation is one unordered pair, stored once.
mapping links;

void create()
{
  relations = ([ ]);
  links = ([ ]);
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

private void _save()
{
  save_object(DIPLOMACY_SAVE, 1);
}

void dest_me()
{
  _save();
  ::dest_me();
}

// Resolve a citizenship reference to ({ name, game }). Accepts a citizenship
// object, its path, or a bare name (game then nil). Name is the file's base
// name (foo.c -> "foo"); game comes from the path.
private mixed * _resolve(mixed c)
{
  string path, name, game;
  string * parts;

  if (!c)
    return nil;

  if (objectp(c))
    path = base_name(c);
  else if (stringp(c))
  {
    if (strlen(c) && c[0] == '/')
      path = c;
    else
      return ({ c, nil });   // a bare name, no game context
  }
  else
    return nil;

  game = game_from_path(path);

  parts = explode(path, "/");
  name = parts[sizeof(parts) - 1];
  if (strlen(name) > 2 && name[strlen(name) - 2 ..] == ".c")
    name = name[0 .. strlen(name) - 3];

  return ({ name, game });
}

// A game's node map (empty when the game has none defined yet).
private mapping _relations(string game)
{
  if (!game || !relations[game])
    return ([ ]);
  return relations[game];
}

// A game's link store, always with both lists present.
private mapping _links(string game)
{
  mapping l;
  l = game ? links[game] : nil;
  if (!l)
    return ([ "enemy" : ({ }), "ally" : ({ }) ]);
  return l;
}

// The node record for a citizenship reference, or nil.
private mapping _record(mixed c)
{
  mixed * r;
  r = _resolve(c);
  if (!r || !strlen(r[0]))
    return nil;
  return _relations(r[1])[r[0]];
}

// --- Query side -----------------------------------------------------------

// The parent citizenship (kingdom) of a name in a game's graph, or "".
private string _parent_name(string game, string name)
{
  mapping rec;
  rec = _relations(game)[name];
  return (rec && stringp(rec["parent"])) ? rec["parent"] : "";
}

// A name together with its parent kingdom, so a relation declared at either
// level is seen. The empty parent is dropped.
private string * _self_and_parent(string game, string name)
{
  string p;
  p = _parent_name(game, name);
  return strlen(p) ? ({ name, p }) : ({ name });
}

// Is the unordered pair {a,b} present in a game's `kind` links?
private int _linked(string game, string kind, string a, string b)
{
  mixed * pairs;
  int i;

  pairs = _links(game)[kind];
  for (i = 0; i < sizeof(pairs); i++)
    if ((pairs[i][0] == a && pairs[i][1] == b) ||
        (pairs[i][0] == b && pairs[i][1] == a))
      return 1;
  return 0;
}

// Is `b` an enemy of `a`? Both may be object, path or name. Relations cascade
// through the parent: a town is at war with whatever its kingdom is at war
// with, so a and b are enemies if any level of a (itself or its kingdom) is
// enemy-linked to any level of b. Neutral (no citizenship) is never an enemy.
int is_enemy(mixed a, mixed b)
{
  mixed * ra, * rb;
  string * aset, * bset;
  string game;
  int i, j;

  ra = _resolve(a);
  rb = _resolve(b);
  if (!ra || !rb || !strlen(ra[0]) || !strlen(rb[0]))
    return 0;

  game = ra[1];                 // a supplies the graph/game
  aset = _self_and_parent(game, ra[0]);
  bset = _self_and_parent(game, rb[0]);

  for (i = 0; i < sizeof(aset); i++)
    for (j = 0; j < sizeof(bset); j++)
      if (_linked(game, "enemy", aset[i], bset[j]))
        return 1;
  return 0;
}

// Is `b` an ally of `a` (or the same citizenship, or a sibling town of the same
// kingdom)? Allies cascade through the parent like enemies do.
int is_ally(mixed a, mixed b)
{
  mixed * ra, * rb;
  string * aset, * bset;
  string game, pa, pb;
  int i, j;

  ra = _resolve(a);
  rb = _resolve(b);
  if (!ra || !rb || !strlen(ra[0]) || !strlen(rb[0]))
    return 0;

  if (ra[0] == rb[0])
    return 1;                   // same citizenship

  game = ra[1];

  // a citizenship and its own kingdom, and two towns of one kingdom, are allies
  pa = _parent_name(game, ra[0]);
  pb = _parent_name(game, rb[0]);
  if (strlen(pa) && (pa == rb[0] || pa == pb))
    return 1;
  if (strlen(pb) && pb == ra[0])
    return 1;

  aset = _self_and_parent(game, ra[0]);
  bset = _self_and_parent(game, rb[0]);
  for (i = 0; i < sizeof(aset); i++)
    for (j = 0; j < sizeof(bset); j++)
      if (_linked(game, "ally", aset[i], bset[j]))
        return 1;
  return 0;
}

int query_security_level(mixed c)
{
  mapping rec;
  rec = _record(c);
  return (rec && intp(rec["security"])) ? rec["security"] : 0;
}

string query_parent(mixed c)
{
  mapping rec;
  rec = _record(c);
  return (rec && stringp(rec["parent"])) ? rec["parent"] : "";
}

// The citizenships directly `kind`-linked to `name` in `game` (the other side
// of each pair). Direct links only -- inherited (parent) relations are not
// listed, they are resolved by is_enemy / is_ally.
private string * _linked_names(string game, string kind, string name)
{
  mixed * pairs;
  string * out;
  int i;

  pairs = _links(game)[kind];
  out = ({ });
  for (i = 0; i < sizeof(pairs); i++)
  {
    if (pairs[i][0] == name)
      out += ({ pairs[i][1] });
    else if (pairs[i][1] == name)
      out += ({ pairs[i][0] });
  }
  return out;
}

string * query_allies(mixed c)
{
  mixed * r;
  r = _resolve(c);
  return (r && strlen(r[0])) ? _linked_names(r[1], "ally", r[0]) : ({ });
}

string * query_enemies(mixed c)
{
  mixed * r;
  r = _resolve(c);
  return (r && strlen(r[0])) ? _linked_names(r[1], "enemy", r[0]) : ({ });
}

// The NPC source a citizenship fields as its guard, or "" if none set. Guards
// follow the citizenship, not the area: change an area's citizenship and its
// guards respawn from the new citizenship's guard source.
string query_guard(mixed c)
{
  mapping rec;
  rec = _record(c);
  return (rec && stringp(rec["guard"])) ? rec["guard"] : "";
}

// Read-only view of a game's whole graph for inspection commands, one entry
// per citizenship with its node data and its direct allies / enemies filled in
// from the links. Synthesised fresh so callers cannot mutate the state.
mapping query_relations(string game)
{
  mapping g, out;
  string * names;
  int i;

  g = _relations(game);
  out = ([ ]);
  names = map_indices(g);
  for (i = 0; i < sizeof(names); i++)
  {
    string n;
    n = names[i];
    out[n] = ([ "parent"   : g[n]["parent"],
                "security" : g[n]["security"],
                "guard"    : g[n]["guard"],
                "allies"   : _linked_names(game, "ally", n),
                "enemies"  : _linked_names(game, "enemy", n) ]);
  }
  return out;
}

// Names of every citizenship defined for a game.
string * query_citizenships(string game)
{
  return map_indices(_relations(game));
}

// --- Mutating side (admin commands build the graph by hand) ---------------

// Ensure a game's node map and a citizenship's node both exist, and return the
// node. A fresh node has no parent, security 0, no guard.
private mapping _ensure(string game, string name)
{
  mapping g;

  if (!relations[game])
    relations[game] = ([ ]);
  g = relations[game];

  if (!g[name])
    g[name] = ([ "parent" : "", "security" : 0, "guard" : "" ]);
  return g[name];
}

// Ensure a game's link store exists and return it.
private mapping _ensure_links(string game)
{
  if (!links[game])
    links[game] = ([ "enemy" : ({ }), "ally" : ({ }) ]);
  return links[game];
}

// The `kind` pair list of a game without the unordered pair {a,b}.
private mixed * _without_pair(mixed * pairs, string a, string b)
{
  mixed * out;
  int i;

  out = ({ });
  for (i = 0; i < sizeof(pairs); i++)
    if (!((pairs[i][0] == a && pairs[i][1] == b) ||
          (pairs[i][0] == b && pairs[i][1] == a)))
      out += ({ pairs[i] });
  return out;
}

// Create a citizenship in a game's graph (no-op if it already exists).
int add_citizenship(string game, string name)
{
  if (!game || !strlen(game) || !name || !strlen(name))
    return 0;
  _ensure(game, name);
  _save();
  return 1;
}

// Drop a citizenship: remove its node and every link that mentions it.
int remove_citizenship(string game, string name)
{
  mapping g, l;

  g = relations[game];
  if (!g || !g[name])
    return 0;

  g[name] = nil;

  // scrub any link that mentions name, from both lists
  l = _ensure_links(game);
  {
    string * kinds;
    int k, i;
    mixed * pairs, * out;

    kinds = ({ "enemy", "ally" });
    for (k = 0; k < sizeof(kinds); k++)
    {
      pairs = l[kinds[k]];
      out = ({ });
      for (i = 0; i < sizeof(pairs); i++)
        if (pairs[i][0] != name && pairs[i][1] != name)
          out += ({ pairs[i] });
      l[kinds[k]] = out;
    }
  }

  _save();
  return 1;
}

int set_parent(string game, string name, string parent)
{
  mapping rec;
  rec = _ensure(game, name);
  rec["parent"] = parent ? parent : "";
  _save();
  return 1;
}

int set_security(string game, string name, int n)
{
  mapping rec;
  rec = _ensure(game, name);
  rec["security"] = n < 0 ? 0 : n;
  _save();
  return 1;
}

int set_guard(string game, string name, string npc_path)
{
  mapping rec;
  rec = _ensure(game, name);
  rec["guard"] = npc_path ? npc_path : "";
  _save();
  return 1;
}

// Add a mutual `kind` link between two citizenships, stored once. The relation
// is dropped from the opposite kind first (an ally is not an enemy, and vice
// versa). Both citizenships are created if needed.
private void _add_link(string game, string kind, string a, string b)
{
  mapping l;
  string opp;

  _ensure(game, a);
  _ensure(game, b);
  l = _ensure_links(game);
  opp = (kind == "enemy") ? "ally" : "enemy";

  l[opp] = _without_pair(l[opp], a, b);
  if (!_linked(game, kind, a, b))
    l[kind] += ({ (a < b) ? ({ a, b }) : ({ b, a }) });
}

int add_ally(string game, string name, string other)
{
  if (!strlen(name) || !strlen(other) || name == other)
    return 0;
  _add_link(game, "ally", name, other);
  _save();
  return 1;
}

int remove_ally(string game, string name, string other)
{
  mapping l;
  l = _ensure_links(game);
  l["ally"] = _without_pair(l["ally"], name, other);
  _save();
  return 1;
}

int add_enemy(string game, string name, string other)
{
  if (!strlen(name) || !strlen(other) || name == other)
    return 0;
  _add_link(game, "enemy", name, other);
  _save();
  return 1;
}

int remove_enemy(string game, string name, string other)
{
  mapping l;
  l = _ensure_links(game);
  l["enemy"] = _without_pair(l["enemy"], name, other);
  _save();
  return 1;
}
