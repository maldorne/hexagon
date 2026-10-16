/*
 * Deities handler.
 *
 * Keeps, per deity, a devotion score for each player who prays or makes
 * offerings to it. Prayers and offerings raise it; it falls with real time, so
 * a player who stops is forgotten: an entry that reaches zero is deleted and
 * the book never holds more than the active devotees. It is brought up to date
 * lazily from a stored timestamp, whenever it is read or written. The lifetime
 * totals (prayers, offerings, curses) are kept by the player itself
 * (add_deity_total), saved with it. Nothing here decides anything yet.
 *
 * Every game with deities has its own handler: /games/<game>/handlers/deities.c
 * inherits this one, points query_save_file at /save/games/<game>/deities.o and
 * may mask the query_devotion_* functions. Reach it with handler("deities", ob),
 * never by path, so the book is the one of ob's game. This shared copy names no
 * file and keeps nothing.
 *
 * Deities are keyed by their full path with ".c", players by name.
 */

#include <deities.h>

inherit "/lib/core/object.c";

// deity path -> ([ player name : ({ devotion points, time of the last update }) ])
mapping devotion;

string query_save_file();

void create()
{
  devotion = ([ ]);
  ::create();

  if (strlen(query_save_file()))
    restore_object(query_save_file(), 1);
  // a book saved with the old per-follower accounts has no devotion: starts empty
  if (!mappingp(devotion))
    devotion = ([ ]);
}

/** Where the accounts are kept; a game's own handler names its file. */
string query_save_file() { return ""; }

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance loaded through handler() may exist.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
    dest_me();
}

void save_handler()
{
  if (strlen(query_save_file()))
    save_object(query_save_file(), 1);
}

void dest_me()
{
  save_handler();
  ::dest_me();
}

/** Points a prayer gives. */
int query_devotion_prayer() { return DEITY_DEVOTION_PRAYER; }

/** Worth of an offering for each point it gives. */
int query_devotion_worth() { return DEITY_DEVOTION_WORTH; }

/** The most points a player can have in a deity. */
int query_devotion_max() { return DEITY_DEVOTION_MAX; }

/** Points lost every decay period. */
int query_devotion_decay() { return DEITY_DEVOTION_DECAY; }

/** Length of the decay period, in seconds. */
int query_devotion_period() { return DEITY_DEVOTION_PERIOD; }

// The devotion of a player in a deity, decayed up to now. The entry is
// deleted, with the deity's map when it empties, if nothing is left.
private int refresh(string name, string deity)
{
  mixed * entry;
  int periods, period, points;

  if (!devotion[deity] || !devotion[deity][name])
    return 0;

  entry = devotion[deity][name];
  points = entry[0];
  period = query_devotion_period();

  if (period > 0)
  {
    periods = (time() - entry[1]) / period;
    if (periods > 0)
    {
      points -= periods * query_devotion_decay();
      // the stamp moves by whole periods, so the remainder keeps running
      entry[1] += periods * period;
    }
  }

  if (points <= 0)
  {
    devotion[deity][name] = nil;
    if (!map_sizeof(devotion[deity]))
      devotion[deity] = nil;
    return 0;
  }

  entry[0] = points;
  return points;
}

// Adds points to a player's devotion in a deity, capped.
private void raise(string name, string deity, int points)
{
  int current, max;

  if (points < 1)
    return;

  current = refresh(name, deity);
  if (!devotion[deity])
    devotion[deity] = ([ ]);

  max = query_devotion_max();
  current += points;
  if (max > 0 && current > max)
    current = max;

  // a new entry starts its clock now; an old one keeps its stamp
  if (!devotion[deity][name])
    devotion[deity][name] = ({ current, time() });
  else
    devotion[deity][name][0] = current;
}

/** A player has just taken a deity. */
void record_worship(object player, string deity)
{
  if (!player || !deity || !strlen(deity))
    return;

  player->set_deity_total(DEITY_TOT_WORSHIPPED, time());
}

/** A follower has made an offering worth that much. */
void record_offering(object player, string deity, int value)
{
  int per;

  if (!player || !deity || !strlen(deity))
    return;

  player->add_deity_total(DEITY_TOT_OFFERINGS, 1);
  player->add_deity_total(DEITY_TOT_OFFERED, value);

  per = query_devotion_worth();
  raise(player->query_name(), deity, per > 0 ? (value / per > 0 ? value / per : 1) : 1);
  save_handler();
}

/** A follower has prayed; heard says whether the prayer was. */
void record_prayer(object player, string deity, int heard)
{
  if (!player || !deity || !strlen(deity))
    return;

  player->add_deity_total(DEITY_TOT_PRAYERS, 1);
  if (heard)
    player->add_deity_total(DEITY_TOT_ANSWERED, 1);

  raise(player->query_name(), deity, query_devotion_prayer());
  save_handler();
}

/** A follower has been cursed by the deity. */
void record_curse(object player, string deity)
{
  if (!player || !deity || !strlen(deity))
    return;

  player->add_deity_total(DEITY_TOT_CURSED, 1);
}

/** The deities that have any active devotee. */
string * query_deities()
{
  string * deities, * names;
  int i, j;

  deities = map_indices(devotion);
  for (i = 0; i < sizeof(deities); i++)
  {
    // refreshing drops the entries that decayed
    names = map_indices(devotion[deities[i]]);
    for (j = 0; j < sizeof(names); j++)
      refresh(names[j], deities[i]);
  }

  return map_indices(devotion);
}

/** The devotion of a player in a deity, decay applied; 0 when none. */
int query_devotion(string name, string deity)
{
  return refresh(name, deity);
}

/**
 * A deity's active devotees, the most devoted first: ({ ({ name, devotion }), ... }).
 */
mixed * query_devotees(string deity)
{
  mixed * ret;
  string * names;
  int i, j, points;

  ret = ({ });
  if (!deity || !devotion[deity])
    return ret;

  names = map_indices(devotion[deity]);
  for (i = 0; i < sizeof(names); i++)
  {
    points = refresh(names[i], deity);
    if (points < 1)
      continue;

    // insertion in order: more points first, then by name
    for (j = 0; j < sizeof(ret); j++)
      if (points > ret[j][1] || (points == ret[j][1] && names[i] < ret[j][0]))
        break;
    if (j == 0)
      ret = ({ ({ names[i], points }) }) + ret;
    else if (j >= sizeof(ret))
      ret += ({ ({ names[i], points }) });
    else
      ret = ret[.. j - 1] + ({ ({ names[i], points }) }) + ret[j ..];
  }

  return ret;
}

mixed * stats()
{
  return ({
    ({ "Save File", strlen(query_save_file()) ? query_save_file() : "none" }),
    ({ "Deities", map_sizeof(devotion) }),
  });
}
