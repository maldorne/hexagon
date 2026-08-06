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

// Field keys whose value can legitimately differ between a male and a female
// spawn of the same source (name, description, plural forms). The rest of the
// data (race, class, level, alignment, weight) is gender-independent.
private string * gendered_keys()
{
  return ({ "name", "short", "long", "main_plural", "aliases", "plurals" });
}

// The gendered half of a live clone's data.
private mapping gendered_fields(object npc)
{
  return ([
    "name":        npc->query_name(),
    "short":       npc->query_short(),
    "long":        npc->query_long(),
    "main_plural": npc->query_main_plural(),
    "aliases":     npc->query_alias(),
    "plurals":     npc->query_plurals(),
  ]);
}

// The gender-independent half.
private mapping nongendered_fields(object npc)
{
  return ([
    "race_ob":  npc->query_race_ob(),
    "class_ob": npc->query_class_ob(),
    "level":    npc->query_level(),
    "align":    npc->query_real_align(),
    "weight":   npc->query_weight(),
  ]);
}

// Value equality for JSON-able data (arrays compare by content, not identity).
private int same_value(mixed a, mixed b)
{
  return json_encode(a) == json_encode(b);
}

// Assemble a template from the fields seen per gender. A source whose setup()
// only ever produces one gender yields a fixed template: "gender" is recorded
// and every field is a single value. A source that varies gender yields a
// multi-gender template: "genders" lists the ones it can roll (spawn picks one)
// and each gendered field that actually differs becomes a mapping keyed by
// gender ([ gender: value ]); fields that match across genders stay single so
// templates stay small and readable. Gender ids are the driver's own
// (0 neuter, 1 male, 2 female) so this generalises to any language, including
// ones with a neuter gender for living beings (German, Russian, ...).
private mapping assemble_template(mapping bygender, mapping nong)
{
  mapping t;
  int * gs;

  t = nong + ([ ]);
  gs = map_indices(bygender);

  if (sizeof(gs) == 1)
  {
    t += bygender[gs[0]];
    t["gender"] = gs[0];
    return t;
  }

  {
    string * keys;
    int i, j;

    keys = gendered_keys();
    for (i = 0; i < sizeof(keys); i++)
    {
      string k;
      mixed ref;
      int varies;

      k = keys[i];
      ref = bygender[gs[0]][k];
      varies = 0;
      for (j = 1; j < sizeof(gs); j++)
        if (!same_value(ref, bygender[gs[j]][k]))
        {
          varies = 1;
          break;
        }

      if (!varies)
        t[k] = ref;
      else
      {
        mapping perg;
        perg = ([ ]);
        // string keys: JSON object keys are strings, and apply_template reads
        // them back as "" + query_gender()
        for (j = 0; j < sizeof(gs); j++)
          perg["" + gs[j]] = bygender[gs[j]][k];
        t[k] = perg;
      }
    }

    t["genders"] = gs;
  }

  return t;
}

// Explicit data extraction from a source .c. Because a source's setup() may
// pick a gender (and matching name / description) at random per clone, the
// source is sampled several times: the first clone seen of each gender supplies
// that gender's fields, and sampling stops as soon as both are seen. This is
// the one place to extend when more of a monster's data needs to survive.
private mapping extract_template(string source)
{
  mapping bygender, nong;
  int i;

  bygender = ([ ]);
  nong = nil;

  for (i = 0; i < 12; i++)
  {
    object npc;
    int g;

    npc = clone_object(source);
    if (!npc)
      continue;

    g = npc->query_gender();
    if (!bygender[g])
    {
      bygender[g] = gendered_fields(npc);
      if (!nong)
        nong = nongendered_fields(npc);
    }
    npc->dest_me();

    if (map_sizeof(bygender) >= 2)
      break;
  }

  if (!map_sizeof(bygender))
    return nil;

  return assemble_template(bygender, nong);
}

// Read (or refresh) a source NPC .c into its data template. Returns 1 on
// success. Samples the source (see extract_template) to capture both genders.
int add_template(string source)
{
  string game, tfile, dir;
  mapping t;
  int slash;

  game = game_from_path(source);
  if (!game)
    return 0;

  t = extract_template(source);
  if (!t)
    return 0;

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

// The gender to give a fresh NPC of this template. A fixed template ("gender")
// dictates it; a multi-gender one ("genders") rolls one of its listed genders;
// with neither known, fall back to a male/female coin flip. Gender ids are the
// driver's own (0 neuter, 1 male, 2 female).
int roll_gender(mapping t)
{
  mixed gl;

  if (!t)
    return random(2) + 1;
  if (t["gender"])
    return t["gender"];

  gl = t["genders"];
  if (pointerp(gl) && sizeof(gl))
    return gl[random(sizeof(gl))];

  return random(2) + 1;
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

  // A fixed template dictates the gender; a multi-gender one rolls one of its
  // listed genders here so apply_template can pick the matching per-gender
  // strings. (The census path decides gender at assign time -- area::assign_npc.)
  npc->set_gender(roll_gender(t));

  npc->apply_template(t);
  return npc;
}
