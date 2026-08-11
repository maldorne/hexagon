/*
 * Diplomacy handler.
 *
 * The single oracle for how citizenships relate. A citizenship is a lean
 * social object (/lib/citizenship.c, inherited by /game/obj/citizenships/*),
 * carried by a living as its city_ob; it stores no relation data. The graph --
 * each citizenship's parent, allies, enemies and security level -- lives here,
 * loaded per game from that game's diplomacy table
 * (/games/<game>/tables/diplomacy.c, which returns the mapping).
 *
 * References passed in may be a citizenship object, its object path, or a bare
 * name; they resolve to a { name, game } pair. A living with no citizenship
 * (city_ob nil) is neutral -- never an enemy.
 */

#include <mud/config.h>
#include <areas/diplomacy.h>

inherit "/lib/core/object.c";

// game -> ([ name : ([ "parent":.., "allies":({}), "enemies":({}), "security":n ]) ])
mapping relations;

void create()
{
  relations = ([ ]);
  ::create();
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

// A game's citizenship graph, loaded once from its diplomacy table (empty when
// the game defines none -- everyone neutral).
private mapping _relations(string game)
{
  mapping g;
  string tpath;
  object t;

  if (!game)
    return ([ ]);
  if (relations[game])
    return relations[game];

  g = ([ ]);
  tpath = "/games/" + game + "/" + DIPLOMACY_TABLE;
  if (file_size(tpath + ".c") >= 0)
  {
    catch(t = load_object(tpath));
    if (t)
      g = t->query_diplomacy();
  }
  if (!mappingp(g))
    g = ([ ]);

  relations[game] = g;
  return g;
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

// Read-only view of a game's whole citizenship graph, for inspection commands.
// Returns a shallow copy so callers cannot mutate the cache.
mapping query_relations(string game)
{
  return ([ ]) + _relations(game);
}

// Drop the cache so a table edit is picked up without a reboot.
void reload()
{
  relations = ([ ]);
}
