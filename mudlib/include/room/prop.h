
// prop.h
//
// Compile-time constants for the location props subsystem. This file
// only defines the symbolic names referenced by:
//
//   /lib/handlers/props.c           (registry / plan oracle)
//   /lib/location/prop.c            (base for custom blueprints)
//   /lib/location/props/*.c         (custom blueprints)
//   /lib/location/components/props.c (the component that owns instances)
//   /lib/tables/props.c       (the data table)
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
#define PROP_OVERRIDE_REMOVED_ACTIONS "removed_actions"
#define PROP_OVERRIDE_PROPS        "props"
#define PROP_OVERRIDE_SENSES       "senses"
// Text written on this one instance (a plaque, an epitaph, a carved name),
// shown by an action that declares PROP_SPEC_SHOW_TEXT.
#define PROP_OVERRIDE_INSCRIPTION  "inscription"
// The language the inscription is written in (a language id of the languages
// handler); the common tongue when unset. A reader who does not know it sees
// it garbled.
#define PROP_OVERRIDE_LANGUAGE     "language"
// A colour for this one instance ("roja"), named where the type's texts carry
// $colour$ and in the room's list of props.
#define PROP_OVERRIDE_COLOUR       "colour"
// What this one instance depicts ("un halcón con las alas abiertas"), named
// where the type's texts carry $symbol$.
#define PROP_OVERRIDE_SYMBOL       "symbol"
// More words this one instance answers to (({ "piedras", "montones" })),
// added to the type's own ids. A generic type with no ids of its own (scenery)
// is named only by these.
#define PROP_OVERRIDE_IDS          "ids"
// How a group of instances sharing one short is named, without the count
// ("receptacles"); a group whose short has no plural is named by the short of
// its first instance.
#define PROP_OVERRIDE_PLURAL       "plural"

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
// Said instead of the above when the one already holding the field is the
// player themselves: "you are already lying there" reads very differently from
// "somebody is already lying there".
#define PROP_SPEC_ALREADY_SELF_MSG      "already_self_msg"
#define PROP_SPEC_SET_STATE             "set_state"
#define PROP_SPEC_CLEAR_STATE           "clear_state"
#define PROP_SPEC_MSG_ME                "msg_me"
#define PROP_SPEC_MSG_OTHERS            "msg_others"
// The override key whose text the action shows to the player after
// PROP_SPEC_MSG_ME (PROP_OVERRIDE_INSCRIPTION for reading). An instance
// without that text gets PROP_SPEC_MISSING_MSG instead, and nothing changes.
#define PROP_SPEC_SHOW_TEXT             "show_text"

// ------------------------------------------------------------
//  Type-entry sub-keys inside /lib/tables/props.c.
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
// Keep the material out of what the room says. The prop still has one -- the
// data is there for anything that cares -- but a room where every fixture is
// announced as wooden reads like an inventory, so a type can ask to be named
// plainly.
#define PROP_TYPE_HIDE_MATERIAL   "hide_material"
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

// Plural counterparts of the three suffix maps above. Same shape
// (state_field -> suffix_string); the renderer picks the plural
// variant when a group carries more than one instance, and falls
// back to the singular map when a plural variant is not defined —
// so authors only fill these in where the singular reads badly on
// several instances ("Está tirada" -> "Están tiradas").
#define PROP_TYPE_STATE_SUFFIXES_PLURAL      "state_suffixes_plural"
#define PROP_TYPE_LONG_SUFFIXES_PLURAL       "long_suffixes_plural"
#define PROP_TYPE_LONG_SUFFIXES_UNSET_PLURAL "long_suffixes_unset_plural"

// Mapping `override_key -> suffix_string` appended to the long description
// when the instance sets that override, so an optional detail (what a rune
// depicts) adds a sentence only where there is one. Not used when the
// instance has its own description.
#define PROP_TYPE_OVERRIDE_SUFFIXES "override_suffixes"

// Mapping `state_field -> light`: the light an instance gives the location
// while that state field is truthy (a lit rune, a burning fire).
#define PROP_TYPE_LIGHT           "light"

#define PROP_TYPE_ACTIONS         "actions"

// What a type smells, sounds, tastes or feels like, for the sense commands:
// ([ "smell": text, ... ]). An instance overrides any of them with
// overrides.senses.
#define PROP_TYPE_SENSES          "senses"

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

#endif // PROP_H
