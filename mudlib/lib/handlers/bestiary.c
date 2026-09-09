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
// conversion.

#include <living/persisted.h>
#include <npc/npc.h>

// defined further down; add_template reads it to preserve hand-set fields
mapping query_template(string game, string source);

// Canonical template identity for a source: where the file actually sits in the
// game tree, with the "/games/<game>/" prefix and any ".c" suffix taken off and
// nothing else touched.
//
//   /games/<game>/areas/<area>/npcs/barman.c -> areas/<area>/npcs/barman
//
// Every directory the file lives in stays in the id, so the id names one place
// and the template file is found by adding the language and the extension back
// -- no directory is ever guessed at either end. This id is what the area stores
// as an NPC's identity instead of the original monster path, so nothing in
// day-to-day operation depends on the source .c still existing.
//
// Idempotent: feeding an id back in returns it unchanged (an id has no
// "/games/<game>/" prefix and no ".c" suffix to strip), so a census/roster key
// reads the same whether it was stored as a source path or already as an id.
string template_id(string game, string source)
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

  return rest;
}

// Template file for a source (a source NPC .c or an already-stripped template
// id). The id is the file's own place in the game tree, so the answer is the id
// with the language and the extension on it -- nothing is inferred:
//
//   areas/<area>/npcs/<type>   -> /games/<game>/areas/<area>/npcs/<type>.es.json
//   areas/<area>/items/<thing> -> /games/<game>/areas/<area>/items/<thing>.es.json
//
// A .json cannot be preprocessed, so it carries the language in its name rather
// than through <language.h>, and the language the mudlib was compiled in picks
// between the siblings.
string query_template_file(string game, string source)
{
  // Asked with the source .c itself, the template belongs beside the file it
  // was captured from. This is the path a capture writes to.
  if (strlen(source) > 2 && source[strlen(source) - 2 ..] == ".c" &&
      file_size(source) >= 0)
    return source[0 .. strlen(source) - 3] + "." + mud_language() + ".json";

  return "/games/" + game + "/" + template_id(game, source) + "." +
         mud_language() + ".json";
}

int has_template(string game, string source)
{
  return file_size(query_template_file(game, source)) >= 0;
}

// Field keys whose value can legitimately differ between a male and a female
// spawn of the same source (name, description, plural forms). The rest of the
// data (race, class, alignment) is gender-independent.
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

// One chatter block as the template stores it: the chance the NPC speaks, and
// the flat weight/message list load_chat itself takes. The live object keeps
// that list behind a running total (chat_string is ({ total, ({ w, msg, ... })
// })), so only the second half is worth storing -- load_chat rebuilds the total.
// Returns nil for a silent NPC so the key drops out of the template.
private mapping chatter_block(int chance, mixed live)
{
  mixed lines;

  if (!pointerp(live) || sizeof(live) < 2)
    return nil;

  lines = live[1];
  if (!pointerp(lines) || !sizeof(lines))
    return nil;

  return ([ "chance": chance, "lines": lines ]);
}

// The idle wander pace, stored as ({ after, rand }) on the live object.
private mapping wander_block(mixed pace)
{
  if (!pointerp(pace) || sizeof(pace) < 2 || (!pace[0] && !pace[1]))
    return nil;

  return ([ "after": pace[0], "rand": pace[1] ]);
}

// The gender-independent half.
//
// Level is deliberately NOT captured: an NPC's level comes from its area
// (npc_default_level + the template's level_area_modifier, swung by the area spread;
// see area::decide_level), so a template carries no absolute level by default.
// A template may still be given an explicit "level" by hand to pin a concrete
// level, or a "level_area_modifier" to sit a fixed number of levels above or
// below the area average.
//
// Weight is likewise NOT captured: set_race_ob already sets the body weight from
// the race (living::social set_weight(query_race_weight())), so the race decides
// it when the template's race_ob is applied.
//
// Stats are NOT captured either, and that is deliberate rather than an omission.
// A source rolls them per clone (set_random_stats(low, high)), so a sampled
// clone shows one throw of the dice, never the rule that produced it. Recording
// the throw would freeze every future NPC of the type at one arbitrary set of
// numbers. The range is copied from the source by hand into "random_stats"; see
// set_template_behaviour.
private mapping nongendered_fields(object npc)
{
  mapping social;
  string * keys;
  int i;

  // Every social object the NPC belongs to, as the raw paths the accessors
  // return. Kept in one map rather than as loose fields so a new slot does not
  // mean a new top-level key. CITY_OB is absent on purpose: citizenship is the
  // nationality of the area an NPC is born in, stamped there, not a trait of
  // the type.
  social = ([
    "race":       npc->query_race_ob(),
    "class":      npc->query_class_ob(),
    "guild":      npc->query_guild_ob(),
    "race_group": npc->query_race_group_ob(),
    "group":      npc->query_group_ob(),
    "job":        npc->query_job_ob(),
    "deity":      npc->query_deity_ob(),
  ]);

  // an unset slot reads nil; drop it so the stored template stays readable
  keys = map_indices(social);
  for (i = 0; i < sizeof(keys); i++)
    if (!stringp(social[keys[i]]) || !strlen(social[keys[i]]))
      map_delete(social, keys[i]);

  return ([
    "social_obs":  social,
    "align":       npc->query_real_align(),
    "wimpy":       npc->query_wimpy(),
    "aggressive":  npc->query_aggressive(),
    "chat":        chatter_block((int)npc->query_chat_chance(),
                                 npc->query_chat_string()),
    "a_chat":      chatter_block((int)npc->query_achat_chance(),
                                 npc->query_achat_string()),
    "move_zones":  npc->query_move_zones(),
    "move_after":  wander_block(npc->query_move_after()),
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
  string * keys;
  int * gs;
  int i, j;

  t = ([ ]) + nong;
  gs = map_indices(bygender);

  if (sizeof(gs) == 1)
  {
    t += bygender[gs[0]];
    t["gender"] = gs[0];
    return t;
  }

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
  string * carried;
  mapping t, old;
  int i, slash;

  game = game_from_path(source);
  if (!game)
    return 0;

  t = extract_template(source);
  if (!t)
    return 0;

  // Carry over every hand-set field from an existing template, so a
  // reconversion (which re-extracts from the source) does not wipe the work.
  // extract_template never produces any of these: they are either things the
  // source cannot express (a sentience mark, an equipment kit, a daily
  // timetable) or things a sampled clone cannot reveal (a stat range, which is
  // rolled per clone). All of them are copied from the source by hand once.
  carried = HAND_SET_TEMPLATE_FIELDS;
  old = query_template(game, source);

  if (old)
    for (i = 0; i < sizeof(carried); i++)
      if (!undefinedp(old[carried[i]]))
        t[carried[i]] = old[carried[i]];

  tfile = query_template_file(game, source);
  slash = strsrch(tfile, "/", -1);
  dir = tfile[0 .. slash - 1];
  mkdir(dir);

  // write_file appends; drop any previous version first. Pretty-print so the
  // template stays hand-readable / editable.
  remove_file(tfile);
  return write_file(tfile, json_encode(t, 1));
}

// Set the hand-authored behaviour fields on a source's template, merging the
// given fields into the stored template and rewriting it. These are the fields
// extract_template cannot sample from the source .c -- equipment kit, sentient
// flag, daily timetable, an explicit level -- so a builder stamps them here and
// add_template preserves them across re-extraction. A field whose value is nil
// is cleared. Returns 1 on success, 0 if the source has no template yet.
//
// Timetable note: JSON object keys are strings, so a timetable stored here must
// use string hour keys ("6", "20"); a reader that indexes it by an int hour
// must convert. Keep symbolic gotos ("work"/"home") and messages inside each
// entry, matching the per-gender name maps that already key by "" + gender.
int set_template_behaviour(string game, string source, mapping fields)
{
  mapping t;
  string tfile;
  string * keys;
  int i;

  if (!fields)
    return 0;

  t = query_template(game, source);
  if (!t)
    return 0;

  keys = map_indices(fields);
  for (i = 0; i < sizeof(keys); i++)
  {
    if (fields[keys[i]] == nil)
      map_delete(t, keys[i]);
    else
      t[keys[i]] = fields[keys[i]];
  }

  tfile = query_template_file(game, source);
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
  npc->set_npc_source(source);

  // A fixed template dictates the gender; a multi-gender one rolls one of its
  // listed genders here so apply_template can pick the matching per-gender
  // strings. (An anonymous monster rolls its gender at every materialization --
  // area::spawn_monster; an individual rolls it once, at its first.)
  npc->set_gender(roll_gender(t));

  npc->apply_template(t, 1);
  return npc;
}
