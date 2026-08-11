/*
 * Diplomacy handler.
 *
 * The single oracle for how citizenships relate, and the owner of that graph.
 * A citizenship is a lean social object (/lib/citizenship.c, inherited by
 * /games/<game>/obj/citizenships/*), carried by a living as its city_ob; it
 * stores no relation data. All relation data -- each citizenship's parent,
 * allies, enemies, security level and guard NPC source -- lives here as
 * mutable, persisted state.
 *
 * The graph is dynamic: a town can be invaded, lose its parent, change the
 * kingdom it belongs to, gain new enemies. So it is never read from a static
 * table; it is built and edited by hand through admin commands and saved to
 * disk (save_object). It starts empty for every game.
 *
 * References passed to the query side may be a citizenship object, its object
 * path, or a bare name; they resolve to a { name, game } pair. A living with
 * no citizenship (city_ob nil) is neutral -- never an enemy.
 */

#include <mud/config.h>
#include <areas/diplomacy.h>

inherit "/lib/core/object.c";

// game -> ([ name : ([ "parent":.., "allies":({}), "enemies":({}),
//                      "security":n, "guard":npc_path ]) ]
// The whole mapping is the handler's persisted state.
mapping relations;

void create()
{
  relations = ([ ]);
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
// name (naduk.c -> "naduk"); game comes from the path.
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

// A game's citizenship graph (empty when the game has none defined yet).
private mapping _relations(string game)
{
  if (!game || !relations[game])
    return ([ ]);
  return relations[game];
}

// The relation record for a citizenship reference, or nil.
private mapping _record(mixed c)
{
  mixed * r;
  mapping g;

  r = _resolve(c);
  if (!r || !strlen(r[0]))
    return nil;

  g = _relations(r[1]);
  return g[r[0]];
}

// --- Query side -----------------------------------------------------------

// Is `b` an enemy of `a`? a supplies the graph/game; both may be object,
// path or name. Neutral (no citizenship) is never an enemy.
int is_enemy(mixed a, mixed b)
{
  mapping rec;
  mixed * rb;

  rec = _record(a);
  if (!rec || !rec["enemies"])
    return 0;

  rb = _resolve(b);
  if (!rb || !strlen(rb[0]))
    return 0;

  return member_array(rb[0], rec["enemies"]) != -1;
}

// Is `b` an ally of `a` (or the same citizenship)?
int is_ally(mixed a, mixed b)
{
  mapping rec;
  mixed * ra, * rb;

  ra = _resolve(a);
  rb = _resolve(b);
  if (!ra || !rb || !strlen(ra[0]) || !strlen(rb[0]))
    return 0;

  if (ra[0] == rb[0])
    return 1;

  rec = _record(a);
  if (!rec || !rec["allies"])
    return 0;

  return member_array(rb[0], rec["allies"]) != -1;
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

string * query_allies(mixed c)
{
  mapping rec;
  rec = _record(c);
  return (rec && pointerp(rec["allies"])) ? rec["allies"] : ({ });
}

string * query_enemies(mixed c)
{
  mapping rec;
  rec = _record(c);
  return (rec && pointerp(rec["enemies"])) ? rec["enemies"] : ({ });
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

// Read-only view of a game's whole citizenship graph, for inspection commands.
// Returns a shallow copy so callers cannot mutate the state.
mapping query_relations(string game)
{
  return ([ ]) + _relations(game);
}

// Names of every citizenship defined for a game.
string * query_citizenships(string game)
{
  return map_indices(_relations(game));
}

// --- Mutating side (admin commands build the graph by hand) ---------------

// Ensure a game's graph and a citizenship's record both exist, and return the
// record. A fresh record is empty of relations, security 0, no guard.
private mapping _ensure(string game, string name)
{
  mapping g;

  if (!relations[game])
    relations[game] = ([ ]);
  g = relations[game];

  if (!g[name])
    g[name] = ([ "parent" : "", "allies" : ({ }),
                 "enemies" : ({ }), "security" : 0, "guard" : "" ]);
  return g[name];
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

// Drop a citizenship and scrub it from every other record's ally/enemy lists.
int remove_citizenship(string game, string name)
{
  mapping g;
  string * others;
  int i;

  g = relations[game];
  if (!g || !g[name])
    return 0;

  g[name] = nil;

  others = map_indices(g);
  for (i = 0; i < sizeof(others); i++)
  {
    g[others[i]]["allies"]  -= ({ name });
    g[others[i]]["enemies"] -= ({ name });
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

int add_ally(string game, string name, string other)
{
  mapping rec;
  rec = _ensure(game, name);
  if (member_array(other, rec["allies"]) == -1)
    rec["allies"] += ({ other });
  // an ally cannot also be an enemy
  rec["enemies"] -= ({ other });
  _save();
  return 1;
}

int remove_ally(string game, string name, string other)
{
  mapping rec;
  rec = relations[game] ? relations[game][name] : nil;
  if (!rec)
    return 0;
  rec["allies"] -= ({ other });
  _save();
  return 1;
}

int add_enemy(string game, string name, string other)
{
  mapping rec;
  rec = _ensure(game, name);
  if (member_array(other, rec["enemies"]) == -1)
    rec["enemies"] += ({ other });
  // an enemy cannot also be an ally
  rec["allies"] -= ({ other });
  _save();
  return 1;
}

int remove_enemy(string game, string name, string other)
{
  mapping rec;
  rec = relations[game] ? relations[game][name] : nil;
  if (!rec)
    return 0;
  rec["enemies"] -= ({ other });
  _save();
  return 1;
}
