/* ************************************************************
 *
 *   Quest system for CcMud, neverbot 03/09
 *
 *   Rewritten for Hexagon, neverbot 09/2026: the flat interleaved array of
 *   ({ path, counter, ... }) inside player.o became a mapping keyed by quest id
 *   in a savefile of its own.
 *
 * *********************************************************** */

// Player quest component. Records, per game, the quests being done with their
// progress per objective, and the quests handed in with how many times and when.
// The state lives in its own savefile (/save/players/<l>/<name>/quests.o) via a
// helper object, and is held here in a static (unsaved) in-memory mapping so it
// never bloats player.o. Loaded lazily on first access, written on each change.
//
//   quest_data = ([ game : ([ QUESTS_ACTIVE : ([ id : ([ QUEST_PROGRESS : ({ int per objective }),
//                                                        QUEST_SOURCE   : "<who gave it>",
//                                                        QUEST_STARTED  : time ]) ]),
//                             QUESTS_DONE   : ([ id : ([ QUEST_TIMES : int,
//                                                        QUEST_LAST  : time ]) ]) ]) ])
//
// This component only stores and answers; every rule about what may be taken,
// advanced or handed in belongs to handler("quests"). Quests are keyed by their
// own stable id, never by the path of the file that defines them, so moving a
// quest file leaves saved progress alone.

#include <user/quests.h>
#include <user/player.h>
#include <language.h>

static mapping quest_data;
static int quests_loaded;

void create()
{
  quest_data = ([ ]);
  quests_loaded = 0;
}

// Lazily read this player's quest savefile, once per session.
private void load_quests()
{
  object o;

  if (quests_loaded)
    return;
  quests_loaded = 1;

  o = clone_object(QUESTS_OB);
  if (o)
  {
    if (o->restore_data(this_object()->query_name()))
      quest_data = o->query_data();
    destruct_object(o);
  }

  if (!mappingp(quest_data))
    quest_data = ([ ]);
}

private void save_quests()
{
  object o;

  // a guest leaves nothing behind
  if (this_object()->query_property(GUEST_PROP))
    return;

  o = clone_object(QUESTS_OB);
  if (!o)
    return;

  o->set_data(quest_data);
  o->save_data(this_object()->query_name());
  destruct_object(o);
}

// Ensure the per-game sub-mapping exists and return it.
private mapping game_data(string game)
{
  load_quests();

  if (!mappingp(quest_data[game]))
    quest_data[game] = ([ QUESTS_ACTIVE : ([ ]), QUESTS_DONE : ([ ]) ]);

  return quest_data[game];
}

mapping query_active_quests(string game)
{
  return game_data(game)[QUESTS_ACTIVE];
}

mapping query_done_quests(string game)
{
  return game_data(game)[QUESTS_DONE];
}

mapping query_quest(string game, string id)
{
  return game_data(game)[QUESTS_ACTIVE][id];
}

int is_doing_quest(string game, string id)
{
  return mappingp(game_data(game)[QUESTS_ACTIVE][id]);
}

int has_done_quest(string game, string id)
{
  return mappingp(game_data(game)[QUESTS_DONE][id]);
}

int query_times_done(string game, string id)
{
  mapping done;

  done = game_data(game)[QUESTS_DONE][id];

  return done ? done[QUEST_TIMES] : 0;
}

// Write down a quest as started, with one counter per objective. `source` is
// whoever handed it over, kept so the log can say where it came from.
void start_quest(string game, string id, int objectives, string source)
{
  game_data(game)[QUESTS_ACTIVE][id] =
    ([ QUEST_PROGRESS : allocate_int(objectives),
       QUEST_SOURCE   : source,
       QUEST_STARTED  : time() ]);

  save_quests();
}

// Move a quest from active to handed in, counting the times it has been.
void finish_quest(string game, string id)
{
  mapping active, done;

  active = game_data(game)[QUESTS_ACTIVE];
  done = game_data(game)[QUESTS_DONE];

  if (!mappingp(active[id]))
    return;

  active = map_delete(active, id);
  quest_data[game][QUESTS_ACTIVE] = active;

  if (mappingp(done[id]))
  {
    done[id][QUEST_TIMES] = done[id][QUEST_TIMES] + 1;
    done[id][QUEST_LAST] = time();
  }
  else
    done[id] = ([ QUEST_TIMES : 1, QUEST_LAST : time() ]);

  save_quests();
}

void abandon_quest(string game, string id)
{
  quest_data[game][QUESTS_ACTIVE] =
    map_delete(game_data(game)[QUESTS_ACTIVE], id);

  save_quests();
}

// Wipe a quest from both lists, as if it had never been taken.
void forget_quest(string game, string id)
{
  game_data(game);

  quest_data[game][QUESTS_ACTIVE] = map_delete(quest_data[game][QUESTS_ACTIVE], id);
  quest_data[game][QUESTS_DONE] = map_delete(quest_data[game][QUESTS_DONE], id);

  save_quests();
}

int * query_progress(string game, string id)
{
  mapping quest;

  quest = game_data(game)[QUESTS_ACTIVE][id];

  return quest ? quest[QUEST_PROGRESS] : nil;
}

// Add to one objective's counter, never above what it asks for, and return the
// counter afterwards. A negative amount is how an objective that counts what is
// carried goes back down when the player parts with it.
int adjust_progress(string game, string id, int which, int amount, int needed)
{
  mapping quest;
  int * progress;

  quest = game_data(game)[QUESTS_ACTIVE][id];

  if (!quest)
    return 0;

  progress = quest[QUEST_PROGRESS];

  if (which < 0 || which >= sizeof(progress))
    return 0;

  progress[which] += amount;

  if (progress[which] > needed)
    progress[which] = needed;
  if (progress[which] < 0)
    progress[which] = 0;

  save_quests();

  return progress[which];
}

mixed * stats()
{
  load_quests();

  return ({ ({ "Quests", quest_data }) });
}
