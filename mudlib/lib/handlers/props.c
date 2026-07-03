
/*
 * Props registry and plan oracle.
 *
 * Single-instance handler that owns the catalogue of available prop
 * types and is the ONLY API any other code in the mudlib uses to
 * interact with props.
 *
 * Two roles:
 *
 *  1. CATALOGUE — load and cache type definitions from
 *     /lib/tables/props_table.c (base catalogue) plus per-game overrides
 *     at /games/<game>/tables/props_table.c. Per-game tables merge over
 *     the base on type id collision. Also scan /lib/location/props/*.c
 *     and per-game /games/<game>/location/props/*.c for custom
 *     blueprints (the ones inheriting /lib/location/prop.c).
 *
 *  2. PLAN ORACLE — the dispatch model described in §5 of the Nottario
 *     doc dev/props.md. query_action_plan(type, verb) returns a plan:
 *     either ({ PROP_PLAN_GENERIC, action_spec }) for fully declarative
 *     actions, or ({ PROP_PLAN_CUSTOM, custom_blueprint }) for actions
 *     that delegate to an LPC handler. The location component executes
 *     the plan; this handler never has side effects.
 *
 * Stateless beyond the in-memory cache. No persistence. Catalogue
 * loaded lazily on first reference; reload() admin entry drops the
 * cache for live development.
 */

#include <mud/config.h>
#include <room/prop.h>

inherit "/lib/core/object.c";

#define HOME             "/room/weather_room.c"
#define PROPS_TABLE      "props_table"
#define BASE_PROPS_DIR   "/lib/location/props/"
#define PROP_BASE_FILE   "/lib/location/prop.c"

// types[type_id] = ([
//   "spec":   <table_data_mapping_for_this_type>,   // possibly ([ ])
//   "custom": <custom_blueprint_or_nil>,
// ])
private mapping types;
private int loaded;

// prototypes
private void _ensure_loaded();
private void _load_table();
private void _scan_customs();
private void _scan_dir(string dir);

void create()
{
  types = ([ ]);
  loaded = 0;

  ::create();
}

void setup()
{
  // anti-clone guard mirrors ventures.c
  if (file_name(this_object()) != "/lib/handlers/props")
  {
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }

  reset_get();
  set_name("props handler");
  set_short("Props handler");
  set_long("This is the props registry handler.\n");
  add_alias("handler");
  add_plural("handlers");

  // move masked, always move to HOME
  move("bing");
}

int move(mixed dest, varargs mixed messin, mixed messout)
{
  ::move(HOME, messin, messout);
}

// ************************************************************
//  Catalogue loading
// ************************************************************

private void _ensure_loaded()
{
  if (loaded) return;
  loaded = 1;     // set before the work so a recursive call during
                  // load does not loop
  _load_table();
  _scan_customs();
}

private void _load_table()
{
  object base_table;
  object * games;
  mapping table_data;
  string * type_ids;
  int i;

  table_data = ([ ]);

  catch
  {
    base_table = load_object("/lib/tables/" + PROPS_TABLE);
    if (base_table)
      table_data = base_table->query_types();
  }

  // per-game tables override base on collision (mapping `+` keeps
  // right-hand side on duplicate keys, which is what we want).
  games = handler("games")->query_game_objects();
  for (i = 0; i < sizeof(games); i++)
  {
    object game_table;
    string game_path;

    if (!games[i]) continue;
    game_path = game_root(games[i]) + "tables/" + PROPS_TABLE;

    catch
    {
      game_table = load_object(game_path);
      if (game_table)
        table_data = table_data + game_table->query_types();
    }
  }

  type_ids = map_indices(table_data);
  for (i = 0; i < sizeof(type_ids); i++)
    types[type_ids[i]] = ([
      "spec":   table_data[type_ids[i]],
      "custom": nil,
    ]);
}

private void _scan_customs()
{
  object * games;
  int i;

  _scan_dir(BASE_PROPS_DIR);

  games = handler("games")->query_game_objects();
  for (i = 0; i < sizeof(games); i++)
  {
    if (!games[i]) continue;
    _scan_dir(game_root(games[i]) + "location/props/");
  }
}

private void _scan_dir(string dir)
{
  string * files;
  string full;
  object blueprint;
  string type_id;
  int i;

  if (file_size(dir) != -2) return;   // not a directory

  // The mudlib's masked get_dir (lib/core/efuns/file.c) returns a flat
  // string array of filenames, not the raw `({ names, sizes, mtimes })`
  // kfun tuple. Use the result directly.
  files = get_dir(dir + "*.c");
  if (!files || !sizeof(files)) return;

  for (i = 0; i < sizeof(files); i++)
  {
    if (files[i] == "prop.c") continue;   // never our own base
    full = dir + files[i];

    catch
    {
      blueprint = load_object(full);
      if (!blueprint) continue;
      if (!blueprint->query_location_prop()) continue;

      type_id = blueprint->query_type_id();
      if (!type_id || !strlen(type_id)) continue;

      // No table entry yet → create one with empty spec so the
      // dispatcher can still ask for its custom blueprint.
      if (!types[type_id])
        types[type_id] = ([ "spec": ([ ]), "custom": nil ]);

      types[type_id]["custom"] = blueprint;
    }
  }
}

// ************************************************************
//  Plan oracle API
// ************************************************************

/*
 * Returns the action plan for (type, verb), or nil if no such action.
 *
 *   ({ PROP_PLAN_GENERIC, action_spec_mapping })
 *   ({ PROP_PLAN_CUSTOM,  custom_blueprint })
 *
 * The component is expected to interpret the plan and run side effects;
 * this handler never mutates anything itself.
 */
mixed * query_action_plan(string type, string verb)
{
  mapping entry;
  mapping spec;
  mapping actions;
  mapping action_spec;
  string kind;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return nil;

  spec = entry["spec"];
  actions = spec ? spec[PROP_TYPE_ACTIONS] : nil;
  action_spec = actions ? actions[verb] : nil;

  if (action_spec)
  {
    kind = action_spec[PROP_SPEC_KIND];

    if (kind == PROP_PLAN_CUSTOM)
    {
      if (!entry["custom"]) return nil;
      return ({ PROP_PLAN_CUSTOM, entry["custom"] });
    }

    return ({ PROP_PLAN_GENERIC, action_spec });
  }

  // No table-driven spec for this verb. If a custom blueprint is
  // attached it owns the catalogue entirely; route the verb to its
  // handle() and let it decide. This covers pure-custom types like
  // /lib/location/props/test_fountain.c that never appear in the
  // table.
  if (entry["custom"])
    return ({ PROP_PLAN_CUSTOM, entry["custom"] });

  return nil;
}

/*
 * Single-line entry composed by the props component when rendering
 * the section. Honours overrides.short first, then the custom
 * blueprint's query_render_line (if it returns non-nil), then the
 * type's short_key. State-driven suffixes from spec[state_suffixes]
 * are appended for each truthy state field; suffix strings that
 * contain "%s" are sprintf'd with the state value (used for occupant
 * names and similar).
 */
string query_render_line(string type, mapping overrides, mapping state)
{
  mapping entry;
  mapping spec;
  mapping suffixes;
  string base;
  string * suffix_keys;
  int i;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return nil;
  spec = entry["spec"];

  // base short
  base = nil;
  if (overrides && overrides[PROP_OVERRIDE_SHORT])
    base = overrides[PROP_OVERRIDE_SHORT];
  if (!base && entry["custom"])
    base = entry["custom"]->query_render_line(state, overrides);
  if (!base && spec)
    base = spec[PROP_TYPE_SHORT_KEY];
  if (!base) base = "";

  // state suffixes
  if (state && spec)
    suffixes = spec[PROP_TYPE_STATE_SUFFIXES];
  if (suffixes)
  {
    suffix_keys = map_indices(suffixes);
    for (i = 0; i < sizeof(suffix_keys); i++)
    {
      mixed value;
      string suffix;

      value = state[suffix_keys[i]];
      if (!value) continue;

      suffix = suffixes[suffix_keys[i]];
      if (!suffix || !strlen(suffix)) continue;

      // Always sprintf — when the suffix has no format directive the
      // extra arg is ignored and the suffix is returned verbatim.
      // Author-controlled language strings keep this safe (no stray %
      // characters). The `"" +` idiom coerces int/string scalars to
      // their natural string form (DGD: when one operand of `+` is a
      // string, the other is rendered without quotes).
      base = base + sprintf(suffix, "" + value);
    }
  }

  return base;
}

/*
 * Full long shown on `look <prop>`. overrides.description wins
 * unconditionally; otherwise the custom blueprint may provide a
 * state-aware long; otherwise the table's long_key plus any
 * matching PROP_TYPE_LONG_SUFFIXES appended for truthy state fields
 * (parallel to the state_suffixes that drive the section line).
 */
string query_type_long(string type, mapping overrides, mapping state,
                       string instance_id)
{
  mapping entry;
  mapping spec;
  mapping suffixes;
  string * keys;
  string ret;
  int i;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return nil;

  if (overrides && overrides[PROP_OVERRIDE_DESCRIPTION])
    return overrides[PROP_OVERRIDE_DESCRIPTION];

  if (entry["custom"])
  {
    ret = entry["custom"]->query_long(state, overrides, instance_id);
    if (ret) return ret;
  }

  spec = entry["spec"];
  if (!spec) return nil;

  ret = spec[PROP_TYPE_LONG_KEY];
  if (!ret) return nil;

  // Append truthy-state suffixes. sprintf returns the suffix
  // verbatim when it has no format directive; the "" + value idiom
  // coerces ints to strings and leaves strings untouched (no
  // debug-style quoting).
  suffixes = spec[PROP_TYPE_LONG_SUFFIXES];
  if (suffixes && state)
  {
    keys = map_indices(suffixes);
    for (i = 0; i < sizeof(keys); i++)
    {
      mixed value;
      value = state[keys[i]];
      if (!value) continue;
      ret += sprintf(suffixes[keys[i]], "" + value);
    }
  }

  // Append falsy/unset-state suffixes — the inverse of the above. Lets
  // a prop describe its default state in the long body (e.g.
  // "ashes are cold" when fireplace.lit is 0) without baking it into
  // PROP_TYPE_LONG_KEY, which should describe only structure so a
  // mutated state can replace the right slice of the description.
  suffixes = spec[PROP_TYPE_LONG_SUFFIXES_UNSET];
  if (suffixes)
  {
    keys = map_indices(suffixes);
    for (i = 0; i < sizeof(keys); i++)
    {
      mixed value;
      value = state ? state[keys[i]] : nil;
      if (value) continue;
      ret += suffixes[keys[i]];
    }
  }

  return ret;
}

/*
 * Just the state-driven suffix tail — the string composed from
 * LONG_SUFFIXES + LONG_SUFFIXES_UNSET for a given state. Reused by
 * the props component when it needs to append the state flavour to
 * a noun-phrase sentence in the room prose.
 */
string query_state_suffixes(string type, mapping state)
{
  mapping entry;
  mapping spec;
  mapping suffixes;
  string * keys;
  string ret;
  int i;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return "";
  spec = entry["spec"];
  if (!spec) return "";

  ret = "";

  suffixes = spec[PROP_TYPE_LONG_SUFFIXES];
  if (suffixes && state)
  {
    keys = map_indices(suffixes);
    for (i = 0; i < sizeof(keys); i++)
    {
      mixed value;
      value = state[keys[i]];
      if (!value) continue;
      ret += sprintf(suffixes[keys[i]], "" + value);
    }
  }

  suffixes = spec[PROP_TYPE_LONG_SUFFIXES_UNSET];
  if (suffixes)
  {
    keys = map_indices(suffixes);
    for (i = 0; i < sizeof(keys); i++)
    {
      mixed value;
      value = state ? state[keys[i]] : nil;
      if (value) continue;
      ret += suffixes[keys[i]];
    }
  }

  return ret;
}

/*
 * Short form (for nominal references in messages, "you sit on the
 * chair" etc). overrides.short wins; otherwise custom; otherwise the
 * table.
 */
string query_type_short(string type, mapping overrides, mapping state)
{
  mapping entry;
  mapping spec;
  string ret;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return nil;

  if (overrides && overrides[PROP_OVERRIDE_SHORT])
    return overrides[PROP_OVERRIDE_SHORT];

  if (entry["custom"])
  {
    ret = entry["custom"]->query_short(state, overrides);
    if (ret) return ret;
  }

  spec = entry["spec"];
  return spec ? spec[PROP_TYPE_SHORT_KEY] : nil;
}

/*
 * Parser id list. Custom blueprint can return its own; otherwise the
 * table's. Caller (the component) is responsible for appending the
 * per-instance override id when matching.
 */
string * query_id_list(string type)
{
  mapping entry;
  mapping spec;
  string * ret;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return ({ });

  if (entry["custom"])
  {
    ret = entry["custom"]->query_id_list();
    if (ret) return ret;
  }

  spec = entry["spec"];
  if (spec && spec[PROP_TYPE_ID_LIST])
    return spec[PROP_TYPE_ID_LIST];

  return ({ });
}

/*
 * The verbs this type supports given the instance overrides. By
 * default returns the union of every action's PROP_SPEC_VERBS array
 * (every synonym across every action) — what init() needs to feed
 * `add_action` so the dispatcher recognises any verb the player may
 * type.
 *
 * Pass `primary_only=1` to get one verb per action instead (the
 * first element of each PROP_SPEC_VERBS array). Player-facing hints
 * and coder listings use this to avoid noise from synonyms.
 *
 * Actions whose spec lacks PROP_SPEC_VERBS fall back to the canonical
 * key as the only verb (backwards compat).
 *
 * Custom blueprint may take over by returning its own list from
 * query_supported_verbs(overrides) — the primary_only mode is ignored
 * in that path; the blueprint is authoritative.
 */
string * query_supported_verbs(string type, mapping overrides,
                               varargs int primary_only)
{
  mapping entry;
  mapping spec;
  mapping actions;
  mapping override_actions;
  string * action_keys;
  string * ret;
  int i, j;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return ({ });

  if (entry["custom"])
  {
    ret = entry["custom"]->query_supported_verbs(overrides);
    if (ret) return ret;
  }

  spec = entry["spec"];
  if (!spec) return ({ });

  actions = spec[PROP_TYPE_ACTIONS];
  if (!actions) return ({ });

  if (overrides)
    override_actions = overrides[PROP_OVERRIDE_ACTIONS];

  action_keys = map_indices(actions);
  ret = ({ });

  for (i = 0; i < sizeof(action_keys); i++)
  {
    string canonical;
    mapping action_spec;
    string * verbs;

    canonical = action_keys[i];

    // a verb explicitly marked PROP_VALUE_REMOVED in overrides is
    // dropped from the supported set; any other override value
    // (including a remapped action mapping) stays available.
    if (override_actions && override_actions[canonical] == PROP_VALUE_REMOVED)
      continue;

    action_spec = actions[canonical];
    verbs = action_spec ? action_spec[PROP_SPEC_VERBS] : nil;

    // Backwards-compat: if the action doesn't declare an explicit
    // verb array, the canonical key is the only verb.
    if (!verbs || !sizeof(verbs))
      verbs = ({ canonical });

    if (primary_only)
    {
      ret += ({ verbs[0] });
      continue;
    }

    for (j = 0; j < sizeof(verbs); j++)
      if (member_array(verbs[j], ret) == -1)
        ret += ({ verbs[j] });
  }

  return ret;
}

/*
 * Resolve a typed verb to the canonical action id for a type. Scans
 * every action's PROP_SPEC_VERBS array; returns the action key whose
 * verb list contains `verb`, or nil if no action accepts it. The
 * dispatcher uses this to translate "apagar" → "extinguish" before
 * checking overrides.actions[canonical] and dispatching the plan.
 */
string query_canonical_action(string type, string verb)
{
  mapping entry;
  mapping spec;
  mapping actions;
  string * action_keys;
  int i;

  _ensure_loaded();

  if (!verb || !strlen(verb)) return nil;

  entry = types[type];
  if (!entry) return nil;

  spec = entry["spec"];
  if (!spec) return nil;

  actions = spec[PROP_TYPE_ACTIONS];
  if (!actions) return nil;

  action_keys = map_indices(actions);
  for (i = 0; i < sizeof(action_keys); i++)
  {
    mapping action_spec;
    string * verbs;

    action_spec = actions[action_keys[i]];
    verbs = action_spec ? action_spec[PROP_SPEC_VERBS] : nil;
    if (!verbs || !sizeof(verbs))
      verbs = ({ action_keys[i] });  // backwards-compat: canonical key

    if (member_array(verb, verbs) != -1)
      return action_keys[i];
  }

  return nil;
}

/*
 * Alphabetical type catalogue for the `props types` builder cmd.
 */
string * query_known_types()
{
  string * ret;

  _ensure_loaded();

  ret = map_indices(types);
  return sort_array(ret);
}

/*
 * Compact summary for `props list` and stats: short, materials,
 * supported_verbs, and whether the type has a custom blueprint.
 */
/*
 * Full type spec — the raw catalogue mapping for a type. Callers
 * that need noun / noun_plural / gender / default_material go
 * through here rather than expanding the summary. Returns nil for
 * unknown types.
 */
mapping query_type_spec(string type)
{
  mapping entry;

  _ensure_loaded();

  entry = types[type];
  return entry ? entry["spec"] : nil;
}

mapping query_type_summary(string type)
{
  mapping entry;
  mapping spec;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return nil;
  spec = entry["spec"];

  return ([
    "short":           spec ? spec[PROP_TYPE_SHORT_KEY] : nil,
    "materials":       spec ? spec[PROP_TYPE_MATERIALS] : nil,
    "supported_verbs": query_supported_verbs(type, nil),
    "custom":          entry["custom"] ? 1 : 0,
  ]);
}

/*
 * Initial state seeded into a new instance by the component's
 * add_prop_instance. Returns a fresh map_copy so the caller does
 * not share storage with the table.
 */
mapping query_default_state(string type)
{
  mapping entry;
  mapping spec;
  mapping default_state;

  _ensure_loaded();

  entry = types[type];
  if (!entry) return ([ ]);
  spec = entry["spec"];
  if (!spec) return ([ ]);

  default_state = spec[PROP_TYPE_DEFAULT_STATE];
  return default_state ? map_copy(default_state) : ([ ]);
}

/*
 * Admin entry. Drops the in-memory cache; the next API call will
 * re-load the table and re-scan custom blueprints. Useful during
 * live development when edits to the table or a custom blueprint
 * should take effect without restarting the mud.
 */
void reload()
{
  types = ([ ]);
  loaded = 0;
}
