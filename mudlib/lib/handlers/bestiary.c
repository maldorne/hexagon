// bestiary.c (BESTIARY_HANDLER) — NPC data-template store.
//
// A hand-authored NPC .c is snapshotted ONCE into a data template
// (/save/games/<game>/npcs/templates/<source-path>.o); every spawn then clones
// a generic mob (GENERIC_MOB) and restores the template into it. The original
// .c is a one-time seed, never reloaded at spawn — mirroring room -> location.
//
// The template captures DATA only (whatever the source setup() sets on the
// monster/living/social vars, plus the shadowed display/id fields). Behaviour
// living in the source's own program (custom functions, own variables,
// per-spawn random() variety, movement started in setup) is NOT carried; such
// sources are identified by manual review during conversion for now (reliable
// reflection on a source's own program is not available from LPC — when an NPC
// component system exists, an automated check would live here).

#include <living/persisted.h>

// Template .o path for a source NPC .c, under the game's template tree. A
// source inside /games/<game>/ is mirrored game-relative; anything else by its
// absolute path. The ".c" suffix becomes ".o".
string query_template_file(string game, string source)
{
  string rest, prefix;

  prefix = "/games/" + game + "/";
  if (strlen(source) >= strlen(prefix) &&
      source[0 .. strlen(prefix) - 1] == prefix)
    rest = source[strlen(prefix) ..];
  else if (strlen(source) && source[0] == '/')
    rest = source[1 ..];
  else
    rest = source;

  if (strlen(rest) > 2 && rest[strlen(rest) - 2 ..] == ".c")
    rest = rest[0 .. strlen(rest) - 3];

  return "/save/games/" + game + "/npcs/templates/" + rest + ".o";
}

// True if a template already exists for this source.
int query_has_template(string game, string source)
{
  return file_size(query_template_file(game, source)) >= 0;
}

// Snapshot a source NPC .c into its data template (adding it to the store).
// Returns 1 on success. Clones the source once, saves its data, destructs it.
int add_template(string source)
{
  object mob;
  string game, tfile, dir;
  int ok, slash;

  game = game_from_path(source);
  if (!game)
    return 0;

  mob = clone_object(source);
  if (!mob)
    return 0;

  tfile = query_template_file(game, source);
  slash = strsrch(tfile, "/", -1);
  dir = tfile[0 .. slash - 1];
  mkdir(dir);

  ok = mob->save_template(tfile);
  mob->dest_me();

  return ok;
}

// Spawn a generic mob and restore the source's template into it. Returns the
// new mob, or nil if there is no template. The caller stamps the persisted
// identity (uuid / game / area) and moves it.
object spawn_from_template(string game, string source)
{
  object mob;
  string tfile;

  tfile = query_template_file(game, source);
  if (file_size(tfile) < 0)
    return nil;

  mob = clone_object(GENERIC_MOB);
  if (!mob)
    return nil;

  mob->restore_template(tfile);
  return mob;
}
