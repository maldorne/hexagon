
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
private string   _render_msg(string template, mixed arg);
private string   _instance_material(mapping inst);
private mapping  _group_instances_by_type_material();
private string   _group_noun_phrase(mapping * insts);
private string   _compose_group_sentence(mapping * insts);
private string   _compose_prose();
string           query_actions_hint(mapping inst);

// ************************************************************
//  Lifecycle
// ************************************************************

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_PROPS);
  props_instances = ({ });

  // Hidden-inventory-item discipline (same pattern as
  // /lib/room/items/door.c): empty name so pretty_short returns ""
  // and the contents listing skips us, plus reset_get/reset_drop as
  // a safety net.
  set_name("");
  reset_get();
  reset_drop();
}

void dest_me()
{
  // Chain to the inherited dest_me so the clone actually destructs.
  // Without this, `update here` (which dest_me's the location's
  // inventory) leaves orphan props component clones lingering — each
  // call accumulates another stale dispatcher in the same room.
  props_instances = ({ });
  ::dest_me();
}

// The component lives inside the location's _inventory so DGD's
// canonical move/init pass registers our verbs for every living that
// enters. add_action(func, verb) only routes when this_object() is in
// the player's environment; staying in components[] alone leaves the
// actions unreachable. This places us in BOTH lists — components[]
// for hook dispatch and queries, _inventory for action registration.
void initialize(object loc)
{
  component::initialize(loc);
  if (loc && environment(this_object()) != loc)
    move(loc);
}

// Enumerate every string a player can type to refer to an attached
// prop — the type's id_list (from the registry) and its material-
// composed variants assembled via _LANG_PROPS_ID_WITH_MATERIAL. Per-
// instance handles (chair_1, silla_hierro, ...) are DELIBERATELY
// omitted; those exist for the coder `props` cmd only and should
// never surface to players as parser input.
string * query_alias()
{
  string * ret;
  int i, j;

  ret = ({ });
  for (i = 0; i < sizeof(props_instances); i++)
  {
    mapping inst;
    mapping spec;
    string * ids;
    string material_phrase;
    string mat_id;

    inst = props_instances[i];
    spec = (mapping)handler("props")->query_type_spec(inst[PROP_FIELD_TYPE]);
    if (!spec) continue;

    ids = (string *)handler("props")->query_id_list(inst[PROP_FIELD_TYPE]);
    if (!ids) continue;

    material_phrase = "";
    mat_id = _instance_material(inst);
    if (mat_id && strlen(mat_id))
      material_phrase = (string)table("materials")->query_material_phrase(mat_id);

    for (j = 0; j < sizeof(ids); j++)
    {
      string id;
      id = ids[j];
      if (member_array(id, ret) == -1)
        ret += ({ id });

      if (strlen(material_phrase))
      {
        string composed;
        composed = _LANG_PROPS_ID_WITH_MATERIAL;
        if (member_array(composed, ret) == -1)
          ret += ({ composed });
      }
    }
  }

  return ret;
}

// Count how many attached instances match `str` — used by the
// location's find_inv_match to inflate our reference in the parser's
// candidate list. That inflation is what lets the mudlib's standard
// numeric-disambiguation idiom (`mirar silla 2`, `sentarse silla 3`)
// work against a composite that carries N instances behind a single
// LPC object. Never trims a numeric suffix itself — the location
// strips before asking.
int count_matching_props(string str)
{
  int i, n;
  n = 0;
  for (i = 0; i < sizeof(props_instances); i++)
    if (_str_matches_instance(str, props_instances[i]))
      n++;
  return n;
}

// Same shape as query_alias but sourced from PROP_TYPE_NOUN_PLURAL —
// lets `find_match` recognise "sillas", "wooden chairs" etc.
string * query_plurals()
{
  string * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(props_instances); i++)
  {
    mapping inst;
    mapping spec;
    string id;
    string material_phrase;
    string mat_id;

    inst = props_instances[i];
    spec = (mapping)handler("props")->query_type_spec(inst[PROP_FIELD_TYPE]);
    if (!spec) continue;

    id = spec[PROP_TYPE_NOUN_PLURAL];
    if (!id || !strlen(id)) continue;

    if (member_array(id, ret) == -1)
      ret += ({ id });

    material_phrase = "";
    mat_id = _instance_material(inst);
    if (mat_id && strlen(mat_id))
      material_phrase = (string)table("materials")->query_material_phrase(mat_id);

    if (strlen(material_phrase))
    {
      string composed;
      composed = _LANG_PROPS_ID_WITH_MATERIAL;
      if (member_array(composed, ret) == -1)
        ret += ({ composed });
    }
  }

  return ret;
}

// present() and other id-driven parser paths call this. The default
// from /lib/core/basic/id.c reads private `alias` / `plurals` arrays
// we don't populate; delegate to our dynamic accessors instead.
int id(string str)
{
  if (!str || !strlen(str)) return 0;
  if (member_array(str, query_alias()) != -1) return 1;
  if (member_array(str, query_plurals()) != -1) return 1;
  return 0;
}

int id_plural(string str)
{
  if (!str || !strlen(str)) return 0;
  return member_array(str, query_plurals()) != -1;
}

// Serving find_match's per-target long lookup: when a player types
// `mirar silla` and find_match matches the component (through
// query_alias / query_plurals / id), look and examine call this to
// render the description. Delegates to the same prop resolution
// logic hook_long uses on the str-provided branch.
string long(varargs string str, int dark)
{
  mapping inst;
  string desc, hint;

  if (!str || !strlen(str)) return "";

  inst = _find_unique_match(str);
  if (!inst) return "";

  desc = (string)handler("props")->query_type_long(
           inst[PROP_FIELD_TYPE],
           inst[PROP_FIELD_OVERRIDES],
           inst[PROP_FIELD_STATE],
           inst[PROP_FIELD_ID]);
  if (!desc || !strlen(desc)) return "";

  // Same wrap idiom the location body uses — respects the viewer's
  // column width and frames the block with a blank line above and
  // below. look.c writes our return verbatim, so the wrap has to
  // happen here. The action-hint line is appended AFTER the wrap so
  // it lines up with the hanging-indent margin instead of drifting
  // to the flush-left column when multi_line hits its leading '\n'.
  if (desc[strlen(desc) - 1] != '\n')
    desc += "\n";
  desc = wrap(desc, (this_user() ? this_user()->query_cols() : 79), 1);

  hint = query_actions_hint(inst);
  if (strlen(hint))
    desc += "    " + hint + "\n";

  return desc;
}

// Overridden to return "" — the component has no visible short of
// its own; if look.c later tries to prepend one for a matched prop
// it should skip the empty result.
string short(varargs int dark)
{
  return "";
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
  mapping groups;
  string * keys;
  string * pieces;
  int i;
  int n;
  string line;

  if (!sizeof(props_instances)) return "";

  groups = _group_instances_by_type_material();
  keys = map_indices(groups);
  pieces = ({ });
  for (i = 0; i < sizeof(keys); i++)
  {
    mapping * insts;
    string piece;

    insts = groups[keys[i]];
    piece = _group_noun_phrase(insts);
    if (!strlen(piece)) continue;

    // Groups collapse into one entry by construction; every instance
    // shares the same state signature, so a single suffix (singular
    // or plural, per group cardinality) describes the lot.
    {
      mapping first;
      string suf;
      first = insts[0];
      suf = (string)handler("props")->query_short_state_suffix(
              first[PROP_FIELD_TYPE], first[PROP_FIELD_STATE],
              sizeof(insts) > 1);
      if (strlen(suf))
        piece += suf;
    }

    pieces += ({ piece });
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
 * Returns the comma-joined list of verbs accepted by this instance:
 * `"climb, lean, smell"`. Empty string when the instance has no
 * actions. One verb per action — the canonical action id, which
 * matches the first element of PROP_SPEC_VERBS when that field
 * lands (task 839723a4).
 *
 * Raw data shape; callers that need the player-facing hint wrap it
 * via query_actions_hint(), coder cmds consume this directly.
 */
string query_action_verbs(mapping inst)
{
  string * verbs;

  if (!inst) return "";

  // primary_only=1 → one verb per action (the first of each
  // PROP_SPEC_VERBS array) so the hint doesn't echo every synonym.
  // init() still uses the unfiltered list to register all of them
  // with add_action.
  verbs = handler("props")->query_supported_verbs(
            inst[PROP_FIELD_TYPE],
            inst[PROP_FIELD_OVERRIDES],
            1);

  if (!verbs || !sizeof(verbs)) return "";

  return implode(verbs, _LANG_PROPS_LIST_SEPARATOR);
}

/*
 * Player-facing hint line shown under `look <prop>`:
 * `"You can: climb, lean, smell."` or the localized
 * `_LANG_PROPS_NO_ACTIONS` when nothing is available.
 */
string query_actions_hint(mapping inst)
{
  string verbs;

  verbs = query_action_verbs(inst);
  if (!strlen(verbs))
    return _LANG_PROPS_NO_ACTIONS;

  return _LANG_PROPS_YOU_CAN + verbs + _LANG_PROPS_LIST_TERMINATOR;
}

/*
 * Reduce contract for long: receive ({ str, dark }).
 *
 * - No str → no contribution. The props section is rendered separately
 *   by location.c via query_props_section_string(), inserted between
 *   the exit list and the inventory. We do not duplicate it inside
 *   the long body.
 * - With a str matching an instance → return the prop's long with
 *   HOOK_EXCLUSIVE so the room body is replaced. We append a
 *   "You can: …" hint line so the player can discover what verbs
 *   the prop accepts.
 */
mixed hook_long(mixed * args)
{
  string str;
  mapping inst;
  string desc;
  string hint;

  if (!args || sizeof(args) < 1) return "";
  str = args[0];

  // No target: contribute the composed prose describing every
  // attached instance. This is what folds the "You see:" section
  // into the room body — the location no longer inserts it as a
  // separate line.
  if (!str || !strlen(str))
    return _compose_prose();

  inst = _find_unique_match(str);
  if (!inst) return "";

  desc = handler("props")->query_type_long(
           inst[PROP_FIELD_TYPE],
           inst[PROP_FIELD_OVERRIDES],
           inst[PROP_FIELD_STATE],
           inst[PROP_FIELD_ID]);

  if (!desc || !strlen(desc)) return "";

  hint = query_actions_hint(inst);
  if (strlen(hint))
    desc += "\n" + hint;

  return ({ HOOK_EXCLUSIVE, desc });
}

// Returns the material identifier for this instance — the
// per-instance override.material if set, else the type's default.
// May return "" or nil if neither is defined.
private string _instance_material(mapping inst)
{
  mapping ov;
  mapping spec;

  ov = inst[PROP_FIELD_OVERRIDES];
  if (ov && ov[PROP_OVERRIDE_MATERIAL])
    return ov[PROP_OVERRIDE_MATERIAL];

  spec = handler("props")->query_type_spec(inst[PROP_FIELD_TYPE]);
  return spec ? spec[PROP_TYPE_DEFAULT_MATERIAL] : nil;
}

// Canonical signature of the truthy state fields for an instance.
// Instances with the same signature share the same visible state
// (state suffixes look identical), so we collapse them in the same
// group. Sorted for order-independence.
private string _state_signature(mapping state)
{
  string * keys;
  string * pairs;
  int i;

  if (!state) return "";

  keys = map_indices(state);
  pairs = ({ });
  for (i = 0; i < sizeof(keys); i++)
  {
    if (!state[keys[i]]) continue;
    pairs += ({ keys[i] + "=" + ("" + state[keys[i]]) });
  }

  return implode(sort_array(pairs), ",");
}

// Groups every attached instance by (type, material, state
// signature) into a mapping. Same-state siblings collapse into one
// group so plural groups can render a shared state suffix; instances
// with differing state land in separate groups so each keeps its
// own flavour.
private mapping _group_instances_by_type_material()
{
  mapping groups;
  int i;

  groups = ([ ]);
  for (i = 0; i < sizeof(props_instances); i++)
  {
    mapping inst;
    string key;
    string mat;

    inst = props_instances[i];
    mat = _instance_material(inst);
    key = inst[PROP_FIELD_TYPE] + ":" + (mat ? mat : "") + ":" +
          _state_signature(inst[PROP_FIELD_STATE]);
    if (!groups[key])
      groups[key] = ({ });
    groups[key] += ({ inst });
  }

  return groups;
}

// Composes the noun phrase for a group of instances — article or
// count word, noun (singular or plural), material phrase — via the
// language template. No trailing period, no state suffixes.
private string _group_noun_phrase(mapping * insts)
{
  mapping first;
  mapping spec;
  int count, gender;
  string mat_id;
  string material_phrase, noun, count_word, phrase;

  count = sizeof(insts);
  first = insts[0];
  spec = handler("props")->query_type_spec(first[PROP_FIELD_TYPE]);
  if (!spec) return "";

  gender = spec[PROP_TYPE_GENDER];

  mat_id = _instance_material(first);
  if (mat_id && strlen(mat_id))
    material_phrase = (string)table("materials")->query_material_phrase(mat_id);
  else
    material_phrase = "";

  noun = spec[count == 1 ? PROP_TYPE_NOUN : PROP_TYPE_NOUN_PLURAL];
  if (!noun || !strlen(noun))
    noun = spec[PROP_TYPE_SHORT_KEY];  // last-ditch fallback

  if (count == 1)
  {
    handler("props")->set_gender(gender);
    count_word = (string)handler("props")->query_numeral();
  }
  else
  {
    count_word = query_num(count, 99);
  }

  phrase = strlen(material_phrase) ?
             _LANG_PROPS_NOUN_PHRASE :
             _LANG_PROPS_NOUN_PHRASE_NO_MATERIAL;

  return phrase;
}

// Composes one sentence for a group of same-(type, material, state)
// instances. Singleton groups use the type's full long description
// (same as `look <prop>`), which carries state suffixes. Plural
// groups use the composed noun phrase + a single state-suffix tail
// (state is uniform across the group by construction).
private string _compose_group_sentence(mapping * insts)
{
  mapping first;
  string ret;

  if (!sizeof(insts)) return "";
  first = insts[0];

  if (sizeof(insts) == 1)
  {
    ret = (string)handler("props")->query_type_long(
            first[PROP_FIELD_TYPE],
            first[PROP_FIELD_OVERRIDES],
            first[PROP_FIELD_STATE],
            first[PROP_FIELD_ID]);
    if (!ret || !strlen(ret))
      return "";
    return ret;
  }

  ret = _group_noun_phrase(insts);
  if (!strlen(ret)) return "";
  ret = capitalize(ret) + ".";

  // Apply the type's state-suffix tail once for the whole group —
  // every instance in this group carries the same state signature,
  // so a single tail describes the lot. Plural flag picks the
  // plural suffix variant when the type defines one.
  {
    string tail;
    tail = (string)handler("props")->query_state_suffixes(
             first[PROP_FIELD_TYPE], first[PROP_FIELD_STATE],
             sizeof(insts) > 1);
    if (strlen(tail))
      ret += tail;
  }

  return ret;
}

// Walks props_instances, groups by (type, material), delegates each
// group to _compose_group_sentence, joins with a space, prepends a
// leading space so the concat combinator inserts a natural separator
// between components' contributions.
private string _compose_prose()
{
  mapping groups;
  string * keys;
  string ret;
  int i;

  if (!sizeof(props_instances)) return "";

  groups = _group_instances_by_type_material();
  ret = "";
  keys = map_indices(groups);
  for (i = 0; i < sizeof(keys); i++)
  {
    string sentence;
    sentence = _compose_group_sentence(groups[keys[i]]);
    if (!strlen(sentence)) continue;
    if (strlen(ret))
      ret += " " + sentence;
    else
      ret = sentence;
  }

  return strlen(ret) ? " " + ret : "";
}

// ************************************************************
//  Verb registration
// ************************************************************

/*
 * Returns the union of verbs every attached instance supports. The
 * component itself consumes this in init() to register the verbs
 * directly on itself; the location no longer mediates verb routing.
 */
string * query_all_supported_verbs()
{
  string * seen;
  int i, j;

  seen = ({ });
  if (!sizeof(props_instances)) return seen;

  for (i = 0; i < sizeof(props_instances); i++)
  {
    string * verbs;

    verbs = handler("props")->query_supported_verbs(
              props_instances[i][PROP_FIELD_TYPE],
              props_instances[i][PROP_FIELD_OVERRIDES]);
    if (!verbs) continue;

    for (j = 0; j < sizeof(verbs); j++)
      if (member_array(verbs[j], seen) == -1)
        seen += ({ verbs[j] });
  }

  return seen;
}

/*
 * Standard DGD init() pass. Runs once per living entering the
 * location (since the component sits in the location's _inventory,
 * the canonical move/init dance reaches us). Registers every
 * supported verb to dispatch through do_prop_action.
 *
 * add_action(func, verb) for the same (player, object, func, verb)
 * is idempotent in DGD — calling init() repeatedly does not produce
 * duplicate registrations.
 */
void init()
{
  string * verbs;
  int i;

  verbs = query_all_supported_verbs();
  if (!verbs) return;

  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_prop_action", verbs[i]);
}

/*
 * Re-publish our verbs to every living already in the location after
 * a mutation that changes the supported set (props add / remove /
 * set on actions). Self-move within the same env triggers the
 * canonical _inv_remove + _inv_add + init pass: every living in the
 * location calls init() on us again (with this_player = that
 * living), and add_action re-runs against the new verb set.
 *
 * Uses move() instead of the lower-level MUDOS->set_initiator_object
 * trick so the refresh is driven by the same machinery the driver
 * uses when an object naturally enters a room.
 */
void refresh_actions()
{
  object loc;

  loc = environment(this_object());
  if (!loc) return;

  move(loc);
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
  string canonical;
  mapping * matches;
  mapping inst;
  mapping ov;
  mapping ov_actions;
  mixed * plan;
  string kind;
  mixed payload;

  // `verb` is what the player typed ("apagar", "encender", "light").
  // `canonical` is the action id the catalogue knows ("extinguish",
  // "light"). Resolution happens per-type below, after we know which
  // prop the str matched — a verb may belong to different canonical
  // actions across prop types.
  verb = query_verb();

  matches = _find_matching_instances(str);

  if (!sizeof(matches))
  {
    notify_fail(_LANG_PROPS_NO_TARGET + "\n");
    return 0;
  }

  if (sizeof(matches) > 1)
  {
    notify_fail(_LANG_PROPS_AMBIGUOUS_TARGET + "\n");
    return 0;
  }

  inst = matches[0];

  canonical = handler("props")->query_canonical_action(
                inst[PROP_FIELD_TYPE], verb);

  // honour overrides.actions[<canonical>] = PROP_VALUE_REMOVED. The
  // sentinel keys on the canonical action id (English) so builder
  // overrides written once apply across every localised verb that
  // dispatches to the same action. A plain `nil` cannot signal
  // removal because DGD deletes mapping keys on assignment.
  ov = inst[PROP_FIELD_OVERRIDES];
  if (ov && canonical)
  {
    ov_actions = ov[PROP_OVERRIDE_ACTIONS];
    if (ov_actions && ov_actions[canonical] == PROP_VALUE_REMOVED)
    {
      notify_fail(_LANG_PROPS_REMOVED_ACTION + "\n");
      return 0;
    }
  }

  plan = canonical
       ? handler("props")->query_action_plan(inst[PROP_FIELD_TYPE], canonical)
       : nil;

  // Fall back to the verb-keyed lookup if no canonical resolution —
  // covers pure-custom blueprints that route through query_action_plan's
  // "no table spec but custom present" path, where the verb is what
  // the blueprint's handle() will receive.
  if (!plan)
    plan = handler("props")->query_action_plan(inst[PROP_FIELD_TYPE], verb);

  if (!plan || sizeof(plan) < 2)
  {
    notify_fail(_LANG_PROPS_NO_HANDLER + "\n");
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

/*
 * Render a prop message template. Prefers souls-style tokens
 * ($mcname$, $mposs$, $mpronoun$, $lastarg$, …) via the souls
 * handler so messages compose gender-agreeing prose without the
 * single-slot `%s` constraint. Legacy templates that still carry a
 * bare `%s` (no `$` tokens) fall back to sprintf for backward
 * compatibility — will retire once every catalogue entry is
 * migrated.
 *
 * `arg` is exposed as $lastarg$ inside the template — used for state
 * values ("who is sitting on this chair") that souls otherwise can't
 * see. Pass "" when the template has no state substitution.
 */
private string _render_msg(string template, mixed arg)
{
  object me;
  string arg_str;

  if (!template || !strlen(template)) return "";

  arg_str = stringp(arg) ? arg : (arg == nil ? "" : ("" + arg));

  // Legacy path: template uses %s, no souls tokens present. Detect
  // via explode instead of strsrch — the latter builds an internal
  // "%s" + substr + "%s" sscanf format that chokes when substr itself
  // is a format directive character.
  if (sizeof(explode(template, "$")) == 1 &&
      sizeof(explode(template, "%s")) > 1)
    return sprintf(template, arg_str);

  // Pre-substitute $lastarg$ ourselves — souls's own $lastarg$
  // resolves against its internal parser state (set by $arg:$ /
  // $ifarg:$ constructs), not the arg we pass in. Doing it here
  // makes the token behave like a straight "the caller's arg goes
  // here" slot from the props side. Loop over every occurrence via
  // sscanf so a leading/trailing $lastarg$ substitutes too (DGD's
  // explode strips boundary empties).
  {
    string a, b;
    while (sscanf(template, "%s$lastarg$%s", a, b) == 2)
    {
      // Capitalise if the token opens the sentence — arg_str is a
      // bare lowercase player name from state, and reads badly as
      // the first word of a message otherwise.
      string sub;
      sub = (a == "") ? capitalize(arg_str) : arg_str;
      template = a + sub + b;
    }
  }

  me = this_player();
  if (!me) return template;

  // ob defaults to me — prop templates don't reference $h*$ tokens.
  // The souls handler is a project-wide singleton; loaded once.
  return (string)handler("souls")->parse_string(template, me, me, arg_str, 1);
}

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
          write(_render_msg(spec[PROP_SPEC_BLOCKED_MSG], "") + "\n");
        return 1;
      }

  // 2. requires_state — flag must be set
  flags = spec[PROP_SPEC_REQUIRES_STATE];
  if (flags)
    for (i = 0; i < sizeof(flags); i++)
      if (!st[flags[i]])
      {
        if (spec[PROP_SPEC_MISSING_MSG])
          write(_render_msg(spec[PROP_SPEC_MISSING_MSG], "") + "\n");
        return 1;
      }

  // 3. requires_state_unset — block whenever the flag has a value, even
  // when that value matches the current player. The existing-occupant
  // message is the right feedback for "you cannot sit on a chair you
  // are already sitting on".
  flags = spec[PROP_SPEC_REQUIRES_STATE_UNSET];
  if (flags)
    for (i = 0; i < sizeof(flags); i++)
      if (st[flags[i]])
      {
        if (spec[PROP_SPEC_ALREADY_SET_MSG])
          write(_render_msg(spec[PROP_SPEC_ALREADY_SET_MSG],
                            "" + st[flags[i]]) + "\n");
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
          write(_render_msg(spec[PROP_SPEC_MISSING_MATCH_MSG], "") + "\n");
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
    write(_render_msg(spec[PROP_SPEC_MSG_ME], "") + "\n");
  if (spec[PROP_SPEC_MSG_OTHERS])
  {
    object loc;
    loc = query_my_location();
    if (loc)
      tell_room(loc,
                _render_msg(spec[PROP_SPEC_MSG_OTHERS],
                            this_player()->query_cap_name()) + "\n",
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
  mapping spec;
  string material_phrase;
  string mat_id;
  int j;

  if (!str || !strlen(str)) return 0;

  // Per-instance handles (chair_1, silla_hierro, …) are intentionally
  // NOT matched here — they are coder-only. _find_by_handle serves
  // the builder cmd path directly against PROP_FIELD_ID.

  ids = (string *)handler("props")->query_id_list(inst[PROP_FIELD_TYPE]);
  spec = (mapping)handler("props")->query_type_spec(inst[PROP_FIELD_TYPE]);
  if (!ids) return 0;

  // Bare singular id.
  if (member_array(str, ids) != -1) return 1;

  // Bare plural noun.
  if (spec && spec[PROP_TYPE_NOUN_PLURAL] == str) return 1;

  // Material-composed variants — same language macro that populates
  // query_alias/query_plurals, so the ordering is defined once per
  // language and consumed everywhere consistently.
  material_phrase = "";
  mat_id = _instance_material(inst);
  if (mat_id && strlen(mat_id))
    material_phrase = (string)table("materials")->query_material_phrase(mat_id);

  if (!strlen(material_phrase)) return 0;

  for (j = 0; j < sizeof(ids); j++)
  {
    string id;
    string composed;
    id = ids[j];
    composed = _LANG_PROPS_ID_WITH_MATERIAL;
    if (composed == str) return 1;
  }

  if (spec && spec[PROP_TYPE_NOUN_PLURAL])
  {
    string id;
    string composed;
    id = spec[PROP_TYPE_NOUN_PLURAL];
    composed = _LANG_PROPS_ID_WITH_MATERIAL;
    if (composed == str) return 1;
  }

  return 0;
}

/*
 * Returns every attached instance whose ids include `str`. Handles
 * the mudlib's numeric-disambiguation idiom (`silla 2` → the 2nd
 * matching silla) by peeling a trailing integer, filtering on the
 * base, then narrowing to the requested slot. Caller decides what
 * to do with cardinality > 1.
 */
private mapping * _find_matching_instances(string str)
{
  mapping * ret;
  string base;
  int num;
  int i;

  ret = ({ });
  if (!str || !strlen(str)) return ret;

  if (sscanf(str, "%s %d", base, num) != 2 || num <= 0)
  {
    base = str;
    num = 0;
  }

  for (i = 0; i < sizeof(props_instances); i++)
    if (_str_matches_instance(base, props_instances[i]))
      ret += ({ props_instances[i] });

  if (num > 0)
  {
    if (num > sizeof(ret)) return ({ });
    return ({ ret[num - 1] });
  }

  return ret;
}

/*
 * Used by the look hook where ambiguity does not warrant an error —
 * returns the FIRST match (or nil if there are zero matches).
 */
private mapping _find_unique_match(string str)
{
  mapping * matches;
  matches = _find_matching_instances(str);
  return sizeof(matches) ? matches[0] : nil;
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
