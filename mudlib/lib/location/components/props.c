
/*
 * Props component.
 *
 * Owns the per-location list of prop instances and their runtime
 * state. The handler at /lib/handlers/props.c is the catalogue / plan
 * oracle; this component is the executor that holds the state and
 * runs the verbs.
 *
 * See the Nottario doc dev/props.md for the full design.
 *
 * Instance shape (one mapping per attached prop):
 *
 *   ([
 *     PROP_FIELD_TYPE:      "chair",
 *     PROP_FIELD_ID:        "throne",   // optional
 *     PROP_FIELD_OVERRIDES: ([ ... ]),  // design-time, set by builder
 *     PROP_FIELD_STATE:     ([ ... ]),  // runtime, mutated by handlers
 *   ])
 *
 * Persistence: rides the symmetric pull-on-save flow established by
 * commit f89f73de. The full instance array (including each `state`)
 * goes through the autoload channel under the namespaced key
 * `props_instances`.
 */

#include <room/location.h>
#include <room/prop.h>
#include <translations/props.h>

inherit component "/lib/location/component.c";

private mapping * props_instances;

// prototypes
private mapping  _find_unique_match(string str);
private mapping * _find_matching_instances(string str);
private mapping  _find_by_handle(string handle);
private int      _str_matches_instance(string str, mapping inst);
private int      _execute_generic(mapping spec, mapping inst, string args);

// ************************************************************
//  Lifecycle
// ************************************************************

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_PROPS);
  props_instances = ({ });
}

void dest_me()
{
  props_instances = ({ });
}

void initialize(object loc)
{
  component::initialize(loc);
}

// ************************************************************
//  Persistence
// ************************************************************

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "props_instances": props_instances, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["props_instances"]))
    props_instances = args["props_instances"];
}

// ************************************************************
//  Builder API — called by the `props` cmd and never during
//  the conversion path.
// ************************************************************

/*
 * Append a new instance. `id` may be nil (the builder cmd auto-generates
 * one when needed). State is seeded from the type's default_state.
 */
void add_prop_instance(string type, string id, mapping overrides)
{
  mapping default_state;
  mapping inst;

  if (!type || !strlen(type)) return;

  default_state = handler("props")->query_default_state(type);
  if (!default_state) default_state = ([ ]);

  inst = ([
    PROP_FIELD_TYPE:      type,
    PROP_FIELD_ID:        id,
    PROP_FIELD_OVERRIDES: overrides ? overrides : ([ ]),
    PROP_FIELD_STATE:     map_copy(default_state),
  ]);

  props_instances += ({ inst });
}

/*
 * Drop the first instance whose id or (failing that) type matches
 * `handle`. Returns 1 on success, 0 if no match.
 */
int remove_prop_instance(string handle)
{
  int i;

  if (!handle || !strlen(handle)) return 0;

  for (i = 0; i < sizeof(props_instances); i++)
    if (props_instances[i][PROP_FIELD_ID] == handle ||
        props_instances[i][PROP_FIELD_TYPE] == handle)
    {
      props_instances = delete(props_instances, i, 1);
      return 1;
    }

  return 0;
}

/*
 * Write a dotted-path entry inside an instance's overrides mapping.
 * Supports depth up to 2 (`material`, `props.nailed`, `actions.tip`).
 * Returns 1 on success.
 */
int set_override(string handle, string dotted_path, mixed value)
{
  mapping inst;
  mapping ov;
  string * parts;

  inst = _find_by_handle(handle);
  if (!inst) return 0;

  parts = explode(dotted_path, ".");
  if (!sizeof(parts)) return 0;

  ov = inst[PROP_FIELD_OVERRIDES];
  if (!ov)
  {
    ov = ([ ]);
    inst[PROP_FIELD_OVERRIDES] = ov;
  }

  if (sizeof(parts) == 1)
  {
    ov[parts[0]] = value;
    return 1;
  }

  // depth 2: ov[parts[0]] is itself a mapping we write into
  if (!ov[parts[0]])
    ov[parts[0]] = ([ ]);
  ov[parts[0]][parts[1]] = value;
  return 1;
}

/*
 * Write a single state field. Use reset_state() instead to wipe and
 * re-seed from the type's default_state. Returns 1 on success.
 */
int set_state(string handle, string field, mixed value)
{
  mapping inst;
  mapping st;

  inst = _find_by_handle(handle);
  if (!inst) return 0;

  st = inst[PROP_FIELD_STATE];
  if (!st)
  {
    st = ([ ]);
    inst[PROP_FIELD_STATE] = st;
  }

  st[field] = value;
  return 1;
}

/*
 * Re-seed the instance state from the type's default_state, as if it
 * had just been added. For `props state <id> reset`. Returns 1 on
 * success.
 */
int reset_state(string handle)
{
  mapping inst;
  mapping default_state;

  inst = _find_by_handle(handle);
  if (!inst) return 0;

  default_state = handler("props")->query_default_state(inst[PROP_FIELD_TYPE]);
  if (!default_state) default_state = ([ ]);

  inst[PROP_FIELD_STATE] = map_copy(default_state);
  return 1;
}

mapping * query_prop_instances() { return props_instances; }

mapping query_instance_by_id(string id)
{
  int i;

  if (!id) return nil;
  for (i = 0; i < sizeof(props_instances); i++)
    if (props_instances[i][PROP_FIELD_ID] == id)
      return props_instances[i];

  return nil;
}

mapping * query_instances_by_type(string type)
{
  mapping * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(props_instances); i++)
    if (props_instances[i][PROP_FIELD_TYPE] == type)
      ret += ({ props_instances[i] });

  return ret;
}

// ************************************************************
//  Render
// ************************************************************

/*
 * Single composed line for the props section in look output, joining
 * the per-instance render_line strings with localized separators.
 * Returns "" when there is nothing to show.
 */
string query_props_section_string()
{
  string * pieces;
  int i;
  int n;
  string line;

  if (!sizeof(props_instances)) return "";

  pieces = ({ });
  for (i = 0; i < sizeof(props_instances); i++)
  {
    string p;
    p = handler("props")->query_render_line(
          props_instances[i][PROP_FIELD_TYPE],
          props_instances[i][PROP_FIELD_OVERRIDES],
          props_instances[i][PROP_FIELD_STATE]);
    if (p && strlen(p))
      pieces += ({ p });
  }

  n = sizeof(pieces);
  if (!n) return "";

  if (n == 1)
    line = pieces[0];
  else if (n == 2)
    line = pieces[0] + _LANG_PROPS_LIST_AND + pieces[1];
  else
    line = implode(pieces[0..n-2], _LANG_PROPS_LIST_SEPARATOR) +
           _LANG_PROPS_LIST_AND + pieces[n-1];

  return _LANG_PROPS_SECTION_HEADER + " " + line + _LANG_PROPS_LIST_TERMINATOR;
}

// ************************************************************
//  Hooks
// ************************************************************

mapping query_hooks()
{
  return ([ "long": HOOK_PRIORITY_DYNAMIC ]);
}

/*
 * Reduce contract for long: receive ({ str, dark }).
 *
 * - No str → no contribution. The props section is rendered separately
 *   by location.c via query_props_section_string(), inserted between
 *   the exit list and the inventory. We do not duplicate it inside
 *   the long body.
 * - With a str matching an instance → return the prop's long with
 *   HOOK_EXCLUSIVE so the room body is replaced.
 */
mixed hook_long(mixed * args)
{
  string str;
  mapping inst;
  string desc;

  if (!args || sizeof(args) < 1) return "";
  str = args[0];

  if (!str || !strlen(str)) return "";

  inst = _find_unique_match(str);
  if (!inst) return "";

  desc = handler("props")->query_long(
           inst[PROP_FIELD_TYPE],
           inst[PROP_FIELD_OVERRIDES],
           inst[PROP_FIELD_STATE],
           inst[PROP_FIELD_ID]);

  if (!desc || !strlen(desc)) return "";

  return ({ HOOK_EXCLUSIVE, desc });
}

/*
 * Delegation point used by the location's id() to know whether a str
 * matches any attached prop instance. Wired in task #6 (render
 * integration in location.c).
 */
int id_matches_prop(string str)
{
  return _find_unique_match(str) != nil ||
         sizeof(_find_matching_instances(str)) > 0;
}

// ************************************************************
//  Verb registration
// ************************************************************

/*
 * Compute the union of verbs every attached instance supports and
 * register one add_action per unique verb. Idempotent across multiple
 * players entering (add_action is per-player, called from init()).
 */
void init()
{
  string * seen;
  int i, j;

  if (!sizeof(props_instances)) return;

  seen = ({ });
  for (i = 0; i < sizeof(props_instances); i++)
  {
    string * verbs;

    verbs = handler("props")->query_supported_verbs(
              props_instances[i][PROP_FIELD_TYPE],
              props_instances[i][PROP_FIELD_OVERRIDES]);
    if (!verbs) continue;

    for (j = 0; j < sizeof(verbs); j++)
      if (member_array(verbs[j], seen) == -1)
      {
        add_action("do_prop_action", verbs[j]);
        seen += ({ verbs[j] });
      }
  }
}

// ************************************************************
//  Action dispatcher
// ************************************************************

/*
 * Single entry point registered for every supported verb. Resolves
 * the target, asks the handler for the action plan, and either runs
 * the generic executor or delegates to a custom blueprint.
 *
 * Returns 0 whenever the action cannot be satisfied, so any global
 * cmd registered for the same verb still has its chance.
 */
int do_prop_action(string str)
{
  string verb;
  mapping * matches;
  mapping inst;
  mapping ov;
  mapping ov_actions;
  mixed * plan;
  string kind;
  mixed payload;

  verb = query_verb();

  matches = _find_matching_instances(str);

  if (!sizeof(matches))
  {
    notify_fail(_LANG_PROPS_NO_TARGET);
    return 0;
  }

  if (sizeof(matches) > 1)
  {
    notify_fail(_LANG_PROPS_AMBIGUOUS_TARGET);
    return 0;
  }

  inst = matches[0];

  // honour overrides.actions[verb] = nil (action explicitly removed)
  ov = inst[PROP_FIELD_OVERRIDES];
  if (ov)
  {
    ov_actions = ov[PROP_OVERRIDE_ACTIONS];
    if (ov_actions &&
        member_array(verb, map_indices(ov_actions)) != -1 &&
        ov_actions[verb] == nil)
    {
      notify_fail(_LANG_PROPS_REMOVED_ACTION);
      return 0;
    }
  }

  plan = handler("props")->query_action_plan(inst[PROP_FIELD_TYPE], verb);
  if (!plan || sizeof(plan) < 2)
  {
    notify_fail(_LANG_PROPS_NO_HANDLER);
    return 0;
  }

  kind = plan[0];
  payload = plan[1];

  if (kind == PROP_PLAN_GENERIC)
    return _execute_generic(payload, inst, str);

  // PROP_PLAN_CUSTOM: payload is the blueprint
  return payload->handle(verb, str,
                         inst[PROP_FIELD_STATE],
                         inst[PROP_FIELD_OVERRIDES],
                         this_player(),
                         query_my_location());
}

// ************************************************************
//  The generic executor
//
//  Implements the six declarative primitives from the data table:
//
//    1. blocked_by             — state[flag] OR overrides.props[flag] aborts
//    2. requires_state         — state[flag] must be set
//    3. requires_state_unset   — state[flag] must be unset (or equal to me)
//    4. requires_state_match   — state[flag] must equal expected value
//    5. set_state              — assign state[flag] = value
//    6. clear_state            — state[flag] = nil
//
//  Plus message output to player and room.
//
//  PROP_VALUE_PLAYER_NAME is the sentinel that resolves to
//  this_player()->query_name() at apply time, allowing table data to
//  refer to "the player who triggered the action".
// ************************************************************

private int _execute_generic(mapping spec, mapping inst, string args)
{
  mapping st;
  mapping ov;
  mapping ov_props;
  string * flags;
  int i;

  // Ensure state mapping exists and the instance points at it (so
  // mutations below are visible to the next save).
  st = inst[PROP_FIELD_STATE];
  if (!st)
  {
    st = ([ ]);
    inst[PROP_FIELD_STATE] = st;
  }

  ov = inst[PROP_FIELD_OVERRIDES];
  ov_props = ov ? ov[PROP_OVERRIDE_PROPS] : nil;

  // 1. blocked_by — state OR static override.props
  flags = spec[PROP_SPEC_BLOCKED_BY];
  if (flags)
    for (i = 0; i < sizeof(flags); i++)
      if (st[flags[i]] || (ov_props && ov_props[flags[i]]))
      {
        if (spec[PROP_SPEC_BLOCKED_MSG])
          write(spec[PROP_SPEC_BLOCKED_MSG] + "\n");
        return 1;
      }

  // 2. requires_state — flag must be set
  flags = spec[PROP_SPEC_REQUIRES_STATE];
  if (flags)
    for (i = 0; i < sizeof(flags); i++)
      if (!st[flags[i]])
      {
        if (spec[PROP_SPEC_MISSING_MSG])
          write(spec[PROP_SPEC_MISSING_MSG] + "\n");
        return 1;
      }

  // 3. requires_state_unset — flag must be unset OR equal to current
  // player's name (idempotent re-trigger)
  flags = spec[PROP_SPEC_REQUIRES_STATE_UNSET];
  if (flags)
    for (i = 0; i < sizeof(flags); i++)
      if (st[flags[i]] &&
          st[flags[i]] != this_player()->query_name())
      {
        if (spec[PROP_SPEC_ALREADY_SET_MSG])
          write(sprintf(spec[PROP_SPEC_ALREADY_SET_MSG],
                        to_string(st[flags[i]])) + "\n");
        return 1;
      }

  // 4. requires_state_match — every key must match expected value
  if (spec[PROP_SPEC_REQUIRES_STATE_MATCH])
  {
    mapping match;
    string * mkeys;

    match = spec[PROP_SPEC_REQUIRES_STATE_MATCH];
    mkeys = map_indices(match);
    for (i = 0; i < sizeof(mkeys); i++)
    {
      mixed expected;

      expected = match[mkeys[i]];
      if (expected == PROP_VALUE_PLAYER_NAME)
        expected = this_player()->query_name();
      if (st[mkeys[i]] != expected)
      {
        if (spec[PROP_SPEC_MISSING_MATCH_MSG])
          write(spec[PROP_SPEC_MISSING_MATCH_MSG] + "\n");
        return 1;
      }
    }
  }

  // 5. set_state — apply mutations
  if (spec[PROP_SPEC_SET_STATE])
  {
    mapping setmap;
    string * skeys;

    setmap = spec[PROP_SPEC_SET_STATE];
    skeys = map_indices(setmap);
    for (i = 0; i < sizeof(skeys); i++)
    {
      mixed v;

      v = setmap[skeys[i]];
      if (v == PROP_VALUE_PLAYER_NAME)
        v = this_player()->query_name();
      st[skeys[i]] = v;
    }
  }

  // 6. clear_state — reset flags
  flags = spec[PROP_SPEC_CLEAR_STATE];
  if (flags)
    for (i = 0; i < sizeof(flags); i++)
      st[flags[i]] = nil;

  // messages
  if (spec[PROP_SPEC_MSG_ME])
    write(spec[PROP_SPEC_MSG_ME] + "\n");
  if (spec[PROP_SPEC_MSG_OTHERS])
  {
    object loc;
    loc = query_my_location();
    if (loc)
      tell_room(loc,
                sprintf(spec[PROP_SPEC_MSG_OTHERS],
                        this_player()->query_cap_name()),
                ({ this_player() }));
  }

  return 1;
}

// ************************************************************
//  Matching helpers
// ************************************************************

/*
 * Returns 1 if `str` matches the instance — either against the type's
 * id_list from the registry, or the instance's per-location id.
 */
private int _str_matches_instance(string str, mapping inst)
{
  string * ids;
  string inst_id;

  if (!str || !strlen(str)) return 0;

  inst_id = inst[PROP_FIELD_ID];
  if (inst_id && inst_id == str) return 1;

  ids = handler("props")->query_id_list(inst[PROP_FIELD_TYPE]);
  if (ids && member_array(str, ids) != -1) return 1;

  return 0;
}

/*
 * Returns every attached instance whose ids include `str`. Caller
 * decides what to do with cardinality > 1.
 */
private mapping * _find_matching_instances(string str)
{
  mapping * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(props_instances); i++)
    if (_str_matches_instance(str, props_instances[i]))
      ret += ({ props_instances[i] });

  return ret;
}

/*
 * Used by the look hook where ambiguity does not warrant an error —
 * returns the FIRST match (or nil if there are zero matches).
 */
private mapping _find_unique_match(string str)
{
  int i;

  for (i = 0; i < sizeof(props_instances); i++)
    if (_str_matches_instance(str, props_instances[i]))
      return props_instances[i];

  return nil;
}

/*
 * For builder-cmd lookups: find an instance by its per-location id
 * first, falling back to the first instance of that type. Returns
 * nil if neither matches.
 */
private mapping _find_by_handle(string handle)
{
  int i;

  if (!handle || !strlen(handle)) return nil;

  for (i = 0; i < sizeof(props_instances); i++)
    if (props_instances[i][PROP_FIELD_ID] == handle)
      return props_instances[i];

  for (i = 0; i < sizeof(props_instances); i++)
    if (props_instances[i][PROP_FIELD_TYPE] == handle)
      return props_instances[i];

  return nil;
}

// ************************************************************
//  Stats
// ************************************************************

mixed * stats()
{
  return component::stats() +
         ({ ({ "Prop instances", props_instances, }) });
}
