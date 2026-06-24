
// prop.h
//
// Compile-time constants for the location props subsystem. See
// the Nottario doc `dev/props.md` for the full design — this file
// only defines the symbolic names referenced by:
//
//   /lib/handlers/props.c           (registry / plan oracle)
//   /lib/location/prop.c            (base for custom blueprints)
//   /lib/location/props/*.c         (custom blueprints)
//   /lib/location/components/props.c (the component that owns instances)
//   /lib/tables/props_table.c       (the data table)
//
// Player-visible strings live in <translations/props.h> and resolve to
// the localized .lang.*.h variant at compile time.

#ifndef PROP_H
#define PROP_H

// ------------------------------------------------------------
//  Instance shape — keys in each entry of props_instances on the
//  location component.
//
//  ({
//    PROP_FIELD_TYPE:      "chair",
//    PROP_FIELD_ID:        "throne",          // optional
//    PROP_FIELD_OVERRIDES: ([ ... ]),         // design-time overrides
//    PROP_FIELD_STATE:     ([ ... ]),         // runtime state
//  })
// ------------------------------------------------------------

#define PROP_FIELD_TYPE       "type"
#define PROP_FIELD_ID         "id"
#define PROP_FIELD_OVERRIDES  "overrides"
#define PROP_FIELD_STATE      "state"

// ------------------------------------------------------------
//  Override sub-keys inside an instance's `overrides` mapping.
//  All optional; missing keys fall back to the type defaults.
//
//  PROP_OVERRIDE_PROPS holds static design-time flags that the
//  dispatcher reads alongside the runtime state when checking
//  blocked_by.
// ------------------------------------------------------------

#define PROP_OVERRIDE_MATERIAL     "material"
#define PROP_OVERRIDE_DESCRIPTION  "description"
#define PROP_OVERRIDE_SHORT        "short"
#define PROP_OVERRIDE_ACTIONS      "actions"
#define PROP_OVERRIDE_PROPS        "props"

// ------------------------------------------------------------
//  Plan kinds returned by handler("props")->query_action_plan().
//  See /lib/location/components/props.c::do_prop_action and §5
//  of props.md for the dispatch contract.
// ------------------------------------------------------------

#define PROP_PLAN_GENERIC  "generic"
#define PROP_PLAN_CUSTOM   "custom"

// ------------------------------------------------------------
//  Action-spec sub-keys, used by the table entries that describe
//  a generic action and consumed by the component's _execute_generic.
//  Every field is optional except PROP_SPEC_KIND.
// ------------------------------------------------------------

#define PROP_SPEC_KIND                  "kind"
// Per-action verb array. Strings the player can type to trigger this
// action. Always an array (single-verb actions use a one-element
// array). Lives in the language file so each locale can have its own
// list of synonyms. The mapping key of the action stays in English as
// the canonical id; the dispatcher resolves the typed verb against
// each action's PROP_SPEC_VERBS to find which canonical action runs.
#define PROP_SPEC_VERBS                 "verbs"
#define PROP_SPEC_BLOCKED_BY            "blocked_by"
#define PROP_SPEC_BLOCKED_MSG           "blocked_msg"
#define PROP_SPEC_REQUIRES_STATE        "requires_state"
#define PROP_SPEC_REQUIRES_STATE_UNSET  "requires_state_unset"
#define PROP_SPEC_REQUIRES_STATE_MATCH  "requires_state_match"
#define PROP_SPEC_MISSING_MSG           "missing_msg"
#define PROP_SPEC_MISSING_MATCH_MSG     "missing_match_msg"
#define PROP_SPEC_ALREADY_SET_MSG       "already_set_msg"
#define PROP_SPEC_SET_STATE             "set_state"
#define PROP_SPEC_CLEAR_STATE           "clear_state"
#define PROP_SPEC_MSG_ME                "msg_me"
#define PROP_SPEC_MSG_OTHERS            "msg_others"

// ------------------------------------------------------------
//  Type-entry sub-keys inside /lib/tables/props_table.c.
//  Mirror order is the same one used in props.md §2.1.
// ------------------------------------------------------------

#define PROP_TYPE_ID_LIST         "id_list"
// SHORT_KEY is the fully-composed short string the renderer falls back
// to when the type doesn't declare the noun/material/gender pieces
// needed to compose a phrase at render time. Prefer NOUN + NOUN_PLURAL
// + GENDER + DEFAULT_MATERIAL when you can.
#define PROP_TYPE_SHORT_KEY       "short_key"
#define PROP_TYPE_LONG_KEY        "long_key"
// Compositional pieces used by the renderer to build the short at
// run time, with per-instance material override. GENDER exists
// because some languages (notably Spanish) need it to pick the
// right article when composing the noun phrase.
#define PROP_TYPE_NOUN            "noun"
#define PROP_TYPE_NOUN_PLURAL     "noun_plural"
#define PROP_TYPE_GENDER          "gender"
#define PROP_TYPE_DEFAULT_MATERIAL "default_material"
#define PROP_TYPE_MATERIALS       "materials"
#define PROP_TYPE_DEFAULT_STATE   "default_state"
#define PROP_TYPE_STATE_SUFFIXES  "state_suffixes"
// Mapping `state_field -> suffix_string` appended to the long
// description when that state field is truthy. Parallel to
// PROP_TYPE_STATE_SUFFIXES (which targets the short / section line);
// this one targets the body of `look <prop>`. Suffix strings may
// contain a single %s, which gets the truthy state value.
#define PROP_TYPE_LONG_SUFFIXES   "long_suffixes"
// Inverse of LONG_SUFFIXES: appends when the field is FALSY or unset.
// Use to describe the default state of a prop in its long body without
// hardcoding it into PROP_TYPE_LONG_KEY (which should stay invariant
// and describe only structure).
#define PROP_TYPE_LONG_SUFFIXES_UNSET   "long_suffixes_unset"
#define PROP_TYPE_ACTIONS         "actions"

// ------------------------------------------------------------
//  Sentinel values.
//
//  PROP_VALUE_PLAYER_NAME, when appearing in a spec's set_state
//  or requires_state_match mapping, is resolved by the generic
//  executor to this_player()->query_name() at apply time. Lets
//  table data refer to "the player who triggered the action"
//  without hardcoding a name.
// ------------------------------------------------------------

#define PROP_VALUE_PLAYER_NAME  "__player_name__"

//  PROP_VALUE_REMOVED marks an action as explicitly stripped from an
//  instance's catalogue. Stored under overrides.actions[verb] = this
//  sentinel, the dispatcher returns _LANG_PROPS_REMOVED_ACTION when
//  that verb fires on the instance. A plain `nil` cannot serve as the
//  marker because DGD deletes mapping keys on assignment to nil.
#define PROP_VALUE_REMOVED  "__removed__"

// ------------------------------------------------------------
//  Gender values used by PROP_TYPE_GENDER. Mirrors the raw ints
//  /lib/core/basic/gender.c reads (case 1, case 2, else). These
//  symbolic names exist so the catalogue stays readable.
// ------------------------------------------------------------

#define GENDER_MALE     1
#define GENDER_FEMALE   2
#define GENDER_NEUTER   0

#endif // PROP_H
