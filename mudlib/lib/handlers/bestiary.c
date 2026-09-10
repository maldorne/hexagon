// bestiary.c (BESTIARY_HANDLER) — NPC data-template store.
//
// A kind of NPC is described by a data template: a plain mapping of
// field -> value, authored by hand as JSON in the game tree. Every spawn
// clones the generic NPC (GENERIC_NPC = /lib/npc.c) and applies one, the way a
// location is a clone with data rather than a compiled room.
//
// This handler only finds and reads those files -- nothing in the mud writes
// one. A template is content, versioned with the game it describes, so a change
// to it is a programmer editing the JSON. What a template may contain, and
// which of its fields belong to the citizenship, the area or the post instead,
// is the author's business, not this handler's.

#include <living/persisted.h>

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
  // Asked with the source .c itself, the template is its sibling: a type that
  // still has a legacy .c keeps its files next to it.
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
