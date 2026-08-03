// Player exploration component. Records, per game, the areas the player has
// explored, the travel anchors they have discovered, and the encyclopedia
// articles they have collected. The state lives in its own savefile
// (/save/players/<l>/<name>/exploration.o) via a helper object, and is held
// here in a static (unsaved) in-memory mapping so it never bloats player.o.
// Loaded lazily on first access, written on each change.
//
//   exploration_data = ([ game : ([ EXP_AREAS    : ({ area names }),
//                                    EXP_ANCHORS  : ([ name : destination ]),
//                                    EXP_ARTICLES : ({ article ids }) ]) ])
//
// Redesigned from the CcMud explorers handler (Folken 2009): Hexagon uses the
// area path as the stable id, so no global name->id registry is needed.

#include <user/exploration.h>

static mapping exploration_data;
static int exploration_loaded;

void create()
{
  exploration_data = ([ ]);
  exploration_loaded = 0;
}

// Lazily read this player's exploration savefile, once per session.
private void load_exploration()
{
  object o;

  if (exploration_loaded)
    return;
  exploration_loaded = 1;

  o = clone_object(EXPLORATION_OB);
  if (o)
  {
    if (o->restore_data(this_object()->query_name()))
      exploration_data = o->query_data();
    destruct_object(o);
  }

  if (!mappingp(exploration_data))
    exploration_data = ([ ]);
}

private void save_exploration()
{
  object o;

  o = clone_object(EXPLORATION_OB);
  if (!o)
    return;

  o->set_data(exploration_data);
  o->save_data(this_object()->query_name());
  destruct_object(o);
}

// Ensure the per-game sub-mapping exists and return it.
private mapping game_data(string game)
{
  load_exploration();

  if (!exploration_data[game])
    exploration_data[game] = ([
      EXP_AREAS    : ({ }),
      EXP_ANCHORS  : ([ ]),
      EXP_ARTICLES : ({ }) ]);

  return exploration_data[game];
}

// --- Explored areas -------------------------------------------------------

// Record that the player has explored an area (by display name) in a game.
// Returns 1 only when newly recorded, so callers can gate a message / XP.
int add_exploration(string game, string area_name)
{
  mapping g;

  if (!game || !area_name || area_name == "")
    return 0;

  g = game_data(game);

  if (member_array(area_name, g[EXP_AREAS]) != -1)
    return 0;

  g[EXP_AREAS] += ({ area_name });
  save_exploration();
  return 1;
}

string * query_explorations(string game)
{
  load_exploration();

  if (!exploration_data[game])
    return ({ });

  return exploration_data[game][EXP_AREAS] + ({ });
}

// --- Travel anchors -------------------------------------------------------

// Record a discovered travel anchor (name -> destination location) in a game.
// Returns 1 only when newly recorded.
int add_travel_point(string game, string name, string destination)
{
  mapping g;

  if (!game || !name || name == "")
    return 0;

  g = game_data(game);

  if (g[EXP_ANCHORS][name])
    return 0;

  g[EXP_ANCHORS][name] = destination;
  save_exploration();
  return 1;
}

string * query_travel_points(string game)
{
  load_exploration();

  if (!exploration_data[game])
    return ({ });

  return map_indices(exploration_data[game][EXP_ANCHORS]);
}

string query_travel_destination(string game, string name)
{
  load_exploration();

  if (!exploration_data[game] || !exploration_data[game][EXP_ANCHORS])
    return nil;

  return exploration_data[game][EXP_ANCHORS][name];
}

// --- Encyclopedia articles ------------------------------------------------

// Record that the player has collected an encyclopedia article (by id) in a
// game. Returns 1 only when newly recorded.
int add_wiki_article(string game, int id)
{
  mapping g;

  if (!game || id <= 0)
    return 0;

  g = game_data(game);

  if (member_array(id, g[EXP_ARTICLES]) != -1)
    return 0;

  g[EXP_ARTICLES] += ({ id });
  save_exploration();
  return 1;
}

int * query_wiki_articles(string game)
{
  load_exploration();

  if (!exploration_data[game])
    return ({ });

  return exploration_data[game][EXP_ARTICLES] + ({ });
}

// Debug accessor.
mapping query_exploration_data()
{
  load_exploration();
  return exploration_data;
}
