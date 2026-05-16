
#include <mud/config.h>

/**
 * Extracts the game name from a mudlib path.
 *
 * Recognises both the source tree (`/games/<name>/...`) and the save
 * tree (`/save/games/<name>/...`). Returns `""` for paths that do not
 * belong to any game (driver, kernel, shared `lib/`, admin / home
 * dirs).
 */
static nomask string game_from_path(string path)
{
  string * words;
  words = explode(path, "/");

  if ((sizeof(words) > 2) && (words[0] == "games"))
    return words[1];
  if ((sizeof(words) > 3) && (words[0] == "save") && (words[1] == "games"))
    return words[2];

  return "";
}

/**
 * Game an object belongs to, or `""` if it does not belong to any.
 *
 * Resolution order:
 *
 *  1. **Players**: defer to their current environment, so the answer
 *     reflects where they actually are.
 *  2. **Runtime entities whose LPC `file_name` does not encode the
 *     game** — locations, sectors, areas: they are all clones of a
 *     generic blueprint under `/lib/` and the game-bound path lives in
 *     their `query_file_name()` (the `.o` save file). Use that.
 *  3. **Everything else**: derive it from the object's own
 *     `file_name(ob)` (works for legacy rooms / NPCs / items that are
 *     `.c` files under `/games/<name>/...`).
 *
 * Without an argument, defaults to `this_object()`.
 */
static nomask string game_name(varargs object ob)
{
  string saved;

  if (!ob)
    ob = this_object();

  // players: ask where they are standing
  if (ob->query_player() && environment(ob))
    return game_name(environment(ob));

  // location / area / sector storage: the LPC file_name is a generic
  // clone (/lib/location#N, /lib/maps/sector#N, /lib/location/area#N);
  // the game-bound path lives in query_file_name().
  saved = ob->query_file_name();
  if (saved && strlen(saved))
    return game_from_path(saved);

  return game_from_path(file_name(ob));
}

/**
 * Source-tree root of the game `ob` belongs to.
 *
 * Returns `/games/<name>/` when the game is known, or `/` as a sane
 * fallback for objects that do not belong to any game. Without an
 * argument, defaults to `this_object()`.
 */
static nomask string game_root(varargs object ob)
{
  string ret;

  if (!ob)
    ob = this_object();

  ret = game_name(ob);

  if (!strlen(ret))
    return "/";

  return "/games/" + ret + "/";
}

/**
 * Save-tree root of the game `ob` belongs to.
 *
 * Returns `/save/games/<name>/` when the game is known, or `/save/` as
 * the fallback. Without an argument, defaults to `this_object()`.
 */
static nomask string game_save_dir(varargs object ob)
{
  string ret;

  if (!ob)
    ob = this_object();

  ret = game_name(ob);

  if (!strlen(ret))
    return "/save/";

  return "/save/games/" + ret + "/";
}

/**
 * Master object of the game `ob` belongs to (loaded from
 * `<game_root>/master.c`).
 *
 * Returns `nil` if the game has no master or it could not be loaded.
 * Useful when callers want to query per-game settings or per-game
 * pretty names.
 */
static nomask object game_master_object(object ob)
{
  object master;
  string path;

  path = game_root(ob);
  master = load_object(path + "master.c");

  if (!master)
    return nil;

  return master;
}

/**
 * Human-readable name of the game `ob` belongs to.
 *
 * Asks the game's master for `query_game_name()` when available; falls
 * back to the mud-wide `MUD_NAME` for objects that do not belong to any
 * game. Without an argument, defaults to `this_object()`.
 */
static nomask string game_pretty_name(varargs object ob)
{
  object master;

  if (!ob)
    ob = this_object();

  master = game_master_object(ob);

  if (master)
    return master->query_game_name();
  else
    return MUD_NAME;
}

/**
 * Quick predicate: is `ob` a source-tree object under `/games/...`?
 *
 * Uses `base_name(ob)` so it answers about the LPC source identity, not
 * about runtime location. A player whose blueprint is `/lib/player.c`
 * returns `0` even when they are standing inside a game; use
 * `game_name(ob)` for the "what game is this object playing right now"
 * question.
 */
static nomask int is_in_game(object ob)
{
  if (explode(base_name(ob), "/")[0] == "games")
    return 1;
  return 0;
}
