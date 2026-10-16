/*
 * A part of a body: a skin, a head, a heart, taken from a corpse with a skill
 * (see /lib/obj/death/body-parts.md). neverbot 10/2026
 *
 * It keeps who it came from (race, name, level, alignment, whether a player)
 * and when it was taken, across saves. A perishable part goes off with the
 * time it spends in the world, as food does: nothing ticks, the time is
 * charged whenever its state is read, and the hours nobody played do not
 * count. Its value grows with the level of the dead and falls as it rots.
 */

#include <living/body-parts.h>
#include <basic/gender.h>
#include <translations/body-parts.h>
#include <language.h>

inherit "/lib/item.c";

// the part id, and the names a spec gave it (empty: the translations name it)
private string part_id;
private mapping part_names;
private int base_value, perishable;
// seconds of freshness left, and when they were last charged
private int fresh_left;
static int charged_at;

// who it came from, as the corpse knew them
private string source_name, source_given_name, source_file, source_npc;
private string source_race_ob, source_race_id;
private string source_base_race_ob, source_base_race_id;
private int source_level, source_align, source_was_player;
private int taken_at;

// prototypes
void describe();

void create()
{
  part_id = "";
  part_names = ([ ]);
  base_value = 0;
  perishable = 0;
  fresh_left = BODY_PART_FRESH_TIME;
  charged_at = time();

  source_name = "";
  source_given_name = "";
  source_file = "";
  source_npc = "";
  source_race_ob = "";
  source_race_id = "";
  source_base_race_ob = "";
  source_base_race_id = "";
  source_level = 0;
  source_align = 0;
  source_was_player = 0;
  taken_at = time();

  ::create();
}

int query_body_part() { return 1; }

// --- Names -------------------------------------------------------------------

/**
 * The name of a part: the spec's own, or the translated one of its id.
 *
 * @param id the part id
 * @param spec its spec, or nil
 */
string part_name(string id, mapping spec)
{
  mapping names;

  if (spec && stringp(spec[BODY_PART_NAME]))
    return spec[BODY_PART_NAME];

  names = _LANG_BODY_PART_NAMES;
  return names[id] ? names[id] : id;
}

/** Every word a part answers to: its name, plural and aliases. */
string * part_words(string id, mapping spec)
{
  mapping plurals, aliases;
  string * words;

  words = ({ part_name(id, spec) });

  if (spec && stringp(spec[BODY_PART_PLURAL]))
    words += ({ spec[BODY_PART_PLURAL] });
  else
  {
    plurals = _LANG_BODY_PART_PLURALS;
    if (plurals[id])
      words += ({ plurals[id] });
  }

  if (spec && arrayp(spec[BODY_PART_ALIASES]))
    words += spec[BODY_PART_ALIASES];
  else
  {
    aliases = _LANG_BODY_PART_ALIASES;
    if (aliases[id])
      words += aliases[id];
  }

  return words;
}

// The name overrides a spec carries, kept so a save names the part the same.
private mapping name_overrides(mapping spec)
{
  mapping ret;
  string * keys;
  int i;

  ret = ([ ]);
  keys = ({ BODY_PART_NAME, BODY_PART_PLURAL, BODY_PART_ALIASES,
            BODY_PART_FEMININE, BODY_PART_PLURAL_NOUN });

  for (i = 0; i < sizeof(keys); i++)
    if (spec && !undefinedp(spec[keys[i]]))
      ret[keys[i]] = spec[keys[i]];

  return ret;
}

string query_part_id() { return part_id; }
string query_part_name() { return part_name(part_id, part_names); }

string query_part_plural()
{
  mapping plurals;

  if (stringp(part_names[BODY_PART_PLURAL]))
    return part_names[BODY_PART_PLURAL];

  plurals = _LANG_BODY_PART_PLURALS;
  return plurals[part_id] ? plurals[part_id] : pluralize(query_part_name());
}

// --- Setting it up -----------------------------------------------------------

/**
 * Makes this the part of that id, with what its spec says.
 *
 * @param id the part id
 * @param spec the race's spec of it (BODY_PART_* keys)
 */
void set_part(string id, mapping spec)
{
  part_id = id ? id : "";
  part_names = name_overrides(spec);
  base_value = (spec && intp(spec[BODY_PART_VALUE])) ? spec[BODY_PART_VALUE] : 0;
  perishable = (spec && spec[BODY_PART_PERISHABLE]) ? 1 : 0;

  if (spec && intp(spec[BODY_PART_WEIGHT]) && spec[BODY_PART_WEIGHT] > 0)
    set_weight(spec[BODY_PART_WEIGHT]);

  describe();
}

/** Copies who the part came from out of the corpse it is taken from. */
void set_source(object corpse)
{
  if (!corpse)
    return;

  source_name = corpse->query_owner() ? corpse->query_owner() : "";
  source_given_name = corpse->query_given_name() ? corpse->query_given_name() : "";
  source_file = corpse->query_file_name() ? corpse->query_file_name() : "";
  source_npc = corpse->query_npc_source() ? corpse->query_npc_source() : "";
  source_race_ob = corpse->query_race_ob() ? corpse->query_race_ob() : "";
  source_race_id = corpse->query_race_id() ? corpse->query_race_id() : "";
  source_base_race_ob = corpse->query_base_race_ob() ? corpse->query_base_race_ob() : "";
  source_base_race_id = corpse->query_base_race_id() ? corpse->query_base_race_id() : "";
  source_level = corpse->query_dead_level();
  source_align = corpse->query_dead_align();
  source_was_player = corpse->query_was_player();
  taken_at = time();

  describe();
}

/** How fresh it starts (0-100): a part off a rotting corpse is half gone. */
void set_freshness(int percent)
{
  if (percent < 0)
    percent = 0;
  if (percent > 100)
    percent = 100;

  fresh_left = BODY_PART_FRESH_TIME * percent / 100;
  charged_at = time();
}

string query_source_name() { return source_name; }
string query_source_given_name() { return source_given_name; }
string query_source_file() { return source_file; }
// the source the dead NPC was spawned from, "" when it was not one
string query_source_npc() { return source_npc; }
string query_source_race_ob() { return source_race_ob; }
string query_source_race_id() { return source_race_id; }
string query_source_base_race_ob() { return source_base_race_ob; }
string query_source_base_race_id() { return source_base_race_id; }
int query_source_level() { return source_level; }
int query_source_align() { return source_align; }
int query_source_was_player() { return source_was_player; }
int query_taken_at() { return taken_at; }
int query_base_value() { return base_value; }
int query_perishable() { return perishable; }

// --- Freshness ---------------------------------------------------------------

// Charges the time spent in the world since the last reading.
private void charge_freshness()
{
  int elapsed;

  elapsed = time() - charged_at;
  charged_at = time();

  if (!perishable || elapsed <= 0)
    return;

  fresh_left = (fresh_left > elapsed) ? fresh_left - elapsed : 0;
}

/** How fresh it is, 0-100; a part that does not rot is always 100. */
int query_freshness()
{
  if (!perishable)
    return 100;

  charge_freshness();
  return fresh_left * 100 / BODY_PART_FRESH_TIME;
}

int query_rotten() { return query_freshness() <= 0; }

/**
 * Its base value grown with the level of the dead and cut by how much it has
 * rotted; never below 1, so a vault keeps it.
 */
int query_value()
{
  int value, fresh;

  value = BODY_LEVEL_WORTH(base_value, source_level);

  fresh = query_freshness();
  if (fresh < BODY_PART_ROTTEN_WORTH)
    fresh = BODY_PART_ROTTEN_WORTH;
  value = value * fresh / 100;

  return value > 0 ? value : 1;
}

// --- Description -------------------------------------------------------------

/** Names and describes the part from its id and who it came from. */
void describe()
{
  string name, plural, source;
  string * words;

  name = query_part_name();
  plural = query_part_plural();
  source = strlen(source_name) ? source_name : _LANG_BODY_PART_UNKNOWN_SOURCE;

  if (!undefinedp(part_names[BODY_PART_FEMININE]))
    set_gender(part_names[BODY_PART_FEMININE] ? GENDER_FEMALE : GENDER_MALE);
  else
    set_gender(member_array(part_id, _LANG_BODY_PART_FEMININE) != -1 ?
               GENDER_FEMALE : GENDER_MALE);

  if (!undefinedp(part_names[BODY_PART_PLURAL_NOUN]))
    set_plural_noun(part_names[BODY_PART_PLURAL_NOUN]);
  else
    set_plural_noun(member_array(part_id, _LANG_BODY_PART_PLURAL_NOUNS) != -1);

  set_name(name);
  set_short(_LANG_BODY_PART_SHORT);
  set_main_plural(_LANG_BODY_PART_PLURAL_SHORT);

  words = part_words(part_id, part_names);
  set_aliases(words + ({ lower_case(_LANG_BODY_PART_SHORT) }));
  set_plurals(({ plural, lower_case(_LANG_BODY_PART_PLURAL_SHORT) }));

  set_long(_LANG_BODY_PART_LONG);
}

string long(varargs string str, int dark)
{
  string ret;
  int fresh, feminine, plural;

  ret = ::long(str, dark);

  if (!perishable)
    return ret;

  fresh = query_freshness();
  feminine = (query_gender() == GENDER_FEMALE);
  plural = query_plural_noun();

  if (fresh <= 0)
    return ret + _LANG_BODY_PART_STATE_ROTTEN;
  if (fresh < BODY_PART_FRESH_MARK)
    return ret + _LANG_BODY_PART_STATE_STALE;
  return ret + _LANG_BODY_PART_STATE_FRESH;
}

// --- Saving ------------------------------------------------------------------

mapping query_auto_load_attributes()
{
  charge_freshness();

  return ::query_auto_load_attributes() +
    ([ "part_id" : part_id,
       "part_names" : part_names,
       "part_value" : base_value,
       "part_perishable" : perishable,
       "part_fresh_left" : fresh_left,
       "part_weight" : query_weight(),
       "part_taken_at" : taken_at,
       "source_name" : source_name,
       "source_given_name" : source_given_name,
       "source_file" : source_file,
       "source_npc" : source_npc,
       "source_race_ob" : source_race_ob,
       "source_race_id" : source_race_id,
       "source_base_race_ob" : source_base_race_ob,
       "source_base_race_id" : source_base_race_id,
       "source_level" : source_level,
       "source_align" : source_align,
       "source_was_player" : source_was_player,
    ]);
}

void init_auto_load_attributes(mapping args)
{
  if (stringp(args["part_id"]))
    part_id = args["part_id"];
  if (mappingp(args["part_names"]))
    part_names = args["part_names"];
  if (intp(args["part_value"]))
    base_value = args["part_value"];
  if (intp(args["part_perishable"]))
    perishable = args["part_perishable"];
  if (intp(args["part_fresh_left"]))
    fresh_left = args["part_fresh_left"];
  if (intp(args["part_weight"]) && args["part_weight"] > 0)
    set_weight(args["part_weight"]);
  if (intp(args["part_taken_at"]) && args["part_taken_at"])
    taken_at = args["part_taken_at"];

  if (stringp(args["source_name"]))
    source_name = args["source_name"];
  if (stringp(args["source_given_name"]))
    source_given_name = args["source_given_name"];
  if (stringp(args["source_file"]))
    source_file = args["source_file"];
  if (stringp(args["source_npc"]))
    source_npc = args["source_npc"];
  if (stringp(args["source_race_ob"]))
    source_race_ob = args["source_race_ob"];
  if (stringp(args["source_race_id"]))
    source_race_id = args["source_race_id"];
  if (stringp(args["source_base_race_ob"]))
    source_base_race_ob = args["source_base_race_ob"];
  if (stringp(args["source_base_race_id"]))
    source_base_race_id = args["source_base_race_id"];
  if (intp(args["source_level"]))
    source_level = args["source_level"];
  if (intp(args["source_align"]))
    source_align = args["source_align"];
  if (intp(args["source_was_player"]))
    source_was_player = args["source_was_player"];

  ::init_auto_load_attributes(args);

  // back in the world: the time its owner spent logged out does not count
  charged_at = time();
  describe();
}

mixed * stats()
{
  return ::stats() + ({
    ({ "Part", part_id }),
    ({ "Base Value", base_value }),
    ({ "Perishable", perishable }),
    ({ "Freshness", query_freshness() }),
    ({ "Source", source_name }),
    ({ "Source Race", source_race_ob }),
    ({ "Source Base Race", source_base_race_ob }),
    ({ "Source Level", source_level }),
    ({ "Source Align", source_align }),
    ({ "Source Was Player", source_was_player }),
    ({ "Taken At", ctime(taken_at) }),
  });
}
