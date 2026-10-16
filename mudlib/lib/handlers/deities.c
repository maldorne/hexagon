/*
 * Deities handler.
 *
 * Keeps the accounts of every deity's followers: when each one took the deity,
 * how many offerings they made and what they were worth, how many prayers they
 * raised, how many were heard and how many times they were cursed. Only the
 * accounts: nothing here decides anything yet.
 *
 * Every game with deities has its own handler: /games/<game>/handlers/deities.c
 * inherits this one and points query_save_file at /save/games/<game>/deities.o.
 * Reach it with handler("deities", ob), never by path, so the accounts are the
 * ones of ob's game. This shared copy names no file and keeps nothing.
 *
 * Deities are keyed by their full path with ".c", players by name.
 */

#include <deities.h>

inherit "/lib/core/object.c";

// deity path -> ([ player name : ([ DEITY_REC_* : value ]) ])
mapping followers;

string query_save_file();

void create()
{
  followers = ([ ]);
  ::create();

  if (strlen(query_save_file()))
    restore_object(query_save_file(), 1);
  if (!mappingp(followers))
    followers = ([ ]);
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

// The account of a follower, made when missing.
private mapping account(string name, string deity)
{
  if (!followers[deity])
    followers[deity] = ([ ]);

  if (!followers[deity][name])
    followers[deity][name] = ([
      DEITY_REC_WORSHIPPED : 0,
      DEITY_REC_OFFERINGS  : 0,
      DEITY_REC_OFFERED    : 0,
      DEITY_REC_PRAYERS    : 0,
      DEITY_REC_ANSWERED   : 0,
      DEITY_REC_CURSED     : 0,
    ]);

  return followers[deity][name];
}

/** A player has just taken a deity. */
void record_worship(object player, string deity)
{
  mapping rec;

  if (!player || !deity || !strlen(deity))
    return;

  rec = account(player->query_name(), deity);
  rec[DEITY_REC_WORSHIPPED] = time();
  save_handler();
}

/** A follower has made an offering worth that much. */
void record_offering(object player, string deity, int value)
{
  mapping rec;

  if (!player || !deity || !strlen(deity))
    return;

  rec = account(player->query_name(), deity);
  rec[DEITY_REC_OFFERINGS] += 1;
  rec[DEITY_REC_OFFERED] += value;
  save_handler();
}

/** A follower has prayed; heard says whether the prayer was. */
void record_prayer(object player, string deity, int heard)
{
  mapping rec;

  if (!player || !deity || !strlen(deity))
    return;

  rec = account(player->query_name(), deity);
  rec[DEITY_REC_PRAYERS] += 1;
  if (heard)
    rec[DEITY_REC_ANSWERED] += 1;
  save_handler();
}

/** A follower has been cursed by the deity. */
void record_curse(object player, string deity)
{
  mapping rec;

  if (!player || !deity || !strlen(deity))
    return;

  rec = account(player->query_name(), deity);
  rec[DEITY_REC_CURSED] += 1;
  save_handler();
}

/** The deities that have any follower on the books. */
string * query_deities() { return map_indices(followers); }

/**
 * A deity's followers: ([ name : account ]), a copy.
 */
mapping query_followers(string deity)
{
  mapping ret;
  string * names;
  int i;

  ret = ([ ]);
  if (!deity || !followers[deity])
    return ret;

  names = map_indices(followers[deity]);
  for (i = 0; i < sizeof(names); i++)
    ret[names[i]] = ([ ]) + followers[deity][names[i]];

  return ret;
}

/**
 * The account of one follower, a copy; nil when they have none.
 */
mapping query_follower(string name, string deity)
{
  if (!deity || !followers[deity] || !followers[deity][name])
    return nil;
  return ([ ]) + followers[deity][name];
}

mixed * stats()
{
  return ({
    ({ "Save File", strlen(query_save_file()) ? query_save_file() : "none" }),
    ({ "Deities", map_sizeof(followers) }),
  });
}
