// bestiary.c (BESTIARY_HANDLER) — NPC data-template store.
//
// A hand-authored NPC .c is read once into a data template (a plain mapping of
// field -> value, stored as JSON at /save/games/<game>/npcs/templates/...);
// every spawn then clones the generic NPC (GENERIC_NPC = /lib/npc.c) and
// applies the template. The original .c is a one-time seed, never reloaded at
// spawn — mirroring room -> location.
//
// The template captures an explicit set of fields (see extract_template). Grow
// that set as more of a source's data needs to survive conversion. Behaviour
// that a data template cannot represent (custom code, per-spawn random()
// variety, movement started in setup) is identified by manual review during
// conversion — see dev/area-npc-system.md.

#include <living/persisted.h>

// Template file for a source NPC .c, under the game's template tree. A source
// inside /games/<game>/ is mirrored game-relative; anything else by its
// absolute path. The ".c" suffix becomes ".json".
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

  // the store already lives under <game>/npcs/templates/, so drop the source's
  // own "npcs" directory segment (the game's per-area npc dir) as redundant
  {
    string * parts, * out;
    int i, drop;

    parts = explode(rest, "/");
    drop = sizeof(parts) - 2;   // the file's parent directory
    if (drop >= 0 && parts[drop] == "npcs")
    {
      out = ({ });
      for (i = 0; i < sizeof(parts); i++)
        if (i != drop)
          out += ({ parts[i] });
      rest = implode(out, "/");
    }
  }

  return "/save/games/" + game + "/npcs/templates/" + rest + ".json";
}

int has_template(string game, string source)
{
  return file_size(query_template_file(game, source)) >= 0;
}

// Explicit data extraction from a live source clone. This is the one place to
// extend when more of a monster's data needs to be carried into templates.
private mapping extract_template(object npc)
{
  return ([
    "name":        npc->query_name(),
    "short":       npc->query_short(),
    "long":        npc->query_long(),
    "main_plural": npc->query_main_plural(),
    "aliases":     npc->query_alias(),
    "plurals":     npc->query_plurals(),
    "race_ob":     npc->query_race_ob(),
    "class_ob":    npc->query_class_ob(),
    "level":       npc->query_level(),
    "gender":      npc->query_gender(),
    "align":       npc->query_real_align(),
    "weight":      npc->query_weight(),
  ]);
}

// Read (or refresh) a source NPC .c into its data template. Returns 1 on
// success. Clones the source once, extracts its data, destructs it.
int add_template(string source)
{
  object npc;
  string game, tfile, dir;
  mapping t;
  int slash;

  game = game_from_path(source);
  if (!game)
    return 0;

  npc = clone_object(source);
  if (!npc)
    return 0;

  t = extract_template(npc);
  npc->dest_me();

  tfile = query_template_file(game, source);
  slash = strsrch(tfile, "/", -1);
  dir = tfile[0 .. slash - 1];
  mkdir(dir);

  // write_file appends; drop any previous version first. Pretty-print so the
  // template stays hand-readable / editable.
  remove_file(tfile);
  return write_file(tfile, json_encode(t, 1));
}

// The template mapping for a source, or nil if none.
mapping query_template(string game, string source)
{
  string tfile, data;

  tfile = query_template_file(game, source);
  if (file_size(tfile) < 0)
    return nil;

  data = read_file(tfile);
  if (!data)
    return nil;

  return json_decode(data);
}

// Spawn a generic NPC and apply the source's template to it. Returns the new
// NPC, or nil if there is no template. The caller stamps the persisted
// identity (uuid / game / area) and moves it.
object spawn_from_template(string game, string source)
{
  mapping t;
  object npc;

  t = query_template(game, source);
  if (!t)
    return nil;

  npc = clone_object(GENERIC_NPC);
  if (!npc)
    return nil;

  // give it its game before applying the template, so game-specific race /
  // class paths pass set_race_ob / set_class_ob validation (game_root)
  npc->set_npc_game(game);
  npc->apply_template(t);
  return npc;
}
