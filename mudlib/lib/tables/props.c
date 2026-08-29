
/*
 * props.c
 *
 * Base catalogue of declarative prop types consumed by
 * /lib/handlers/props.c (the registry / plan oracle). Every entry
 * here describes a fully data-driven type: id list, descriptions,
 * materials, default state, state-driven render suffixes, and an
 * action catalogue with one of the six declarative primitives per
 * action.
 *
 * For types that need real LPC logic (a fountain that heals, a
 * statue that consults a quest handler) the action `kind` becomes
 * PROP_PLAN_CUSTOM and a matching .c file at
 * /lib/location/props/<type>.c provides the handler. None of the
 * v1 catalogue needs that.
 *
 * Per-game overrides live at /games/<game>/tables/props.c
 * (standard hexagon table convention). A per-game entry with the
 * same key replaces the base entry on the registry side.
 *
 * Player-visible strings come from <translations/props.h>, which
 * the driver rewrites to the .en.h / .es.h variant according to
 * GLOBAL_COMPILE_LANG.
 */

#include <room/prop.h>
#include <basic/gender.h>
#include <item/material.h>
#include <translations/props.h>

private mapping types;

void create()
{
  types = ([

    // ------------------------------------------------------------
    //  chair — full state cycle: sit/stand, tip/right
    // ------------------------------------------------------------
    "chair": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_CHAIR_ID,
        _LANG_PROP_CHAIR_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_CHAIR_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_CHAIR_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WOOD,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_CHAIR_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_CHAIR_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WOOD }),

      PROP_TYPE_DEFAULT_STATE: ([
        "tipped":    0,
        "occupant":  nil,
      ]),

      PROP_TYPE_STATE_SUFFIXES: ([
        "tipped":    _LANG_PROP_CHAIR_SUFFIX_TIPPED,
        "occupant":  _LANG_PROP_CHAIR_SUFFIX_OCCUPIED,
      ]),

      PROP_TYPE_STATE_SUFFIXES_PLURAL: ([
        "tipped":    _LANG_PROP_CHAIR_SUFFIX_TIPPED_PLURAL,
      ]),

      PROP_TYPE_LONG_SUFFIXES: ([
        "tipped":    _LANG_PROP_CHAIR_LONG_TIPPED,
        "occupant":  _LANG_PROP_CHAIR_LONG_OCCUPIED,
      ]),

      PROP_TYPE_LONG_SUFFIXES_PLURAL: ([
        "tipped":    _LANG_PROP_CHAIR_LONG_TIPPED_PLURAL,
      ]),

      PROP_TYPE_ACTIONS: ([

        // sit: requires occupant unset (or self), sets occupant.
        // Blocked by tipped state and by overrides.props.broken.
        "sit": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:                _LANG_PROP_CHAIR_SIT_VERBS,
          PROP_SPEC_BLOCKED_BY:           ({ "tipped", "broken" }),
          PROP_SPEC_BLOCKED_MSG:          _LANG_PROP_CHAIR_CANT_SIT,
          PROP_SPEC_REQUIRES_STATE_UNSET: ({ "occupant" }),
          PROP_SPEC_ALREADY_SET_MSG:      _LANG_PROP_CHAIR_OCCUPIED_BY,
          PROP_SPEC_ALREADY_SELF_MSG:     _LANG_PROP_CHAIR_ALREADY_SITTING,
          PROP_SPEC_SET_STATE:            ([ "occupant": PROP_VALUE_PLAYER_NAME ]),
          PROP_SPEC_MSG_ME:               _LANG_PROP_CHAIR_SIT_ME,
          PROP_SPEC_MSG_OTHERS:           _LANG_PROP_CHAIR_SIT_OTHERS,
        ]),

        // stand: only the current occupant can do it.
        "stand": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:                _LANG_PROP_CHAIR_STAND_VERBS,
          PROP_SPEC_REQUIRES_STATE_MATCH: ([ "occupant": PROP_VALUE_PLAYER_NAME ]),
          PROP_SPEC_MISSING_MATCH_MSG:    _LANG_PROP_CHAIR_NOT_SITTING,
          PROP_SPEC_CLEAR_STATE:          ({ "occupant" }),
          PROP_SPEC_MSG_ME:               _LANG_PROP_CHAIR_STAND_ME,
          PROP_SPEC_MSG_OTHERS:           _LANG_PROP_CHAIR_STAND_OTHERS,
        ]),

        // tip: blocked by overrides.props.nailed and by already-tipped
        // state. Sets tipped; clears occupant (whoever was sitting
        // falls off).
        "tip": ([
          PROP_SPEC_KIND:           PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:          _LANG_PROP_CHAIR_TIP_VERBS,
          PROP_SPEC_BLOCKED_BY:     ({ "nailed", "tipped" }),
          PROP_SPEC_BLOCKED_MSG:    _LANG_PROP_CHAIR_NAILED_OR_TIPPED,
          PROP_SPEC_SET_STATE:      ([ "tipped": 1 ]),
          PROP_SPEC_CLEAR_STATE:    ({ "occupant" }),
          PROP_SPEC_MSG_ME:         _LANG_PROP_CHAIR_TIP_ME,
          PROP_SPEC_MSG_OTHERS:     _LANG_PROP_CHAIR_TIP_OTHERS,
        ]),

        // right: undo tip. Requires tipped state.
        "right": ([
          PROP_SPEC_KIND:             PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:            _LANG_PROP_CHAIR_RIGHT_VERBS,
          PROP_SPEC_REQUIRES_STATE:   ({ "tipped" }),
          PROP_SPEC_MISSING_MSG:      _LANG_PROP_CHAIR_NOT_TIPPED,
          PROP_SPEC_CLEAR_STATE:      ({ "tipped" }),
          PROP_SPEC_MSG_ME:           _LANG_PROP_CHAIR_RIGHT_ME,
          PROP_SPEC_MSG_OTHERS:       _LANG_PROP_CHAIR_RIGHT_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  table — no state
    // ------------------------------------------------------------
    "table": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_TABLE_ID,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_TABLE_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_TABLE_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WOOD,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_TABLE_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_TABLE_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WOOD }),

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "smell": ([
          PROP_SPEC_KIND:     PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:    _LANG_PROP_TABLE_SMELL_VERBS,
          PROP_SPEC_MSG_ME:   _LANG_PROP_TABLE_SMELL_ME,
        ]),

        "lean": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:        _LANG_PROP_TABLE_LEAN_VERBS,
          PROP_SPEC_MSG_ME:       _LANG_PROP_TABLE_LEAN_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_TABLE_LEAN_OTHERS,
        ]),

        "climb": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:        _LANG_PROP_TABLE_CLIMB_VERBS,
          PROP_SPEC_MSG_ME:       _LANG_PROP_TABLE_CLIMB_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_TABLE_CLIMB_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  statue — decorative, stateless. Pray action for flavour.
    // ------------------------------------------------------------
    "statue": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_STATUE_ID,
        _LANG_PROP_STATUE_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_STATUE_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_STATUE_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_METAL,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_STATUE_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_STATUE_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_METAL }),

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "pray": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:        _LANG_PROP_STATUE_PRAY_VERBS,
          PROP_SPEC_MSG_ME:       _LANG_PROP_STATUE_PRAY_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_STATUE_PRAY_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  altar — decorative, stateless. Pray action for flavour.
    // ------------------------------------------------------------
    "altar": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_ALTAR_ID,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_ALTAR_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_ALTAR_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_MALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_STONE,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_ALTAR_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_ALTAR_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_STONE }),

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "pray": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:        _LANG_PROP_ALTAR_PRAY_VERBS,
          PROP_SPEC_MSG_ME:       _LANG_PROP_ALTAR_PRAY_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_ALTAR_PRAY_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  fountain — stateless decorative + drink action.
    // ------------------------------------------------------------
    "fountain": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_FOUNTAIN_ID,
        _LANG_PROP_FOUNTAIN_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_FOUNTAIN_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_FOUNTAIN_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_STONE,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_FOUNTAIN_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_FOUNTAIN_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_STONE }),

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "drink": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:        _LANG_PROP_FOUNTAIN_DRINK_VERBS,
          PROP_SPEC_MSG_ME:       _LANG_PROP_FOUNTAIN_DRINK_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_FOUNTAIN_DRINK_OTHERS,
        ]),

        "smell": ([
          PROP_SPEC_KIND:     PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:    _LANG_PROP_FOUNTAIN_SMELL_VERBS,
          PROP_SPEC_MSG_ME:   _LANG_PROP_FOUNTAIN_SMELL_ME,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  fireplace — stateful (lit/unlit) with light/extinguish.
    // ------------------------------------------------------------
    "fireplace": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_FIREPLACE_ID,
        _LANG_PROP_FIREPLACE_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_FIREPLACE_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_FIREPLACE_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_STONE,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_FIREPLACE_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_FIREPLACE_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_STONE }),

      PROP_TYPE_DEFAULT_STATE: ([ "lit": 0, ]),

      PROP_TYPE_STATE_SUFFIXES: ([
        "lit": _LANG_PROP_FIREPLACE_SUFFIX_LIT,
      ]),

      PROP_TYPE_LONG_SUFFIXES: ([
        "lit": _LANG_PROP_FIREPLACE_LONG_LIT,
      ]),

      PROP_TYPE_LONG_SUFFIXES_UNSET_PLURAL: ([
        "lit": _LANG_PROP_FIREPLACE_LONG_UNLIT_PLURAL,
      ]),

      PROP_TYPE_LONG_SUFFIXES_UNSET: ([
        "lit": _LANG_PROP_FIREPLACE_LONG_UNLIT,
      ]),

      PROP_TYPE_ACTIONS: ([

        "light": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:                _LANG_PROP_FIREPLACE_LIGHT_VERBS,
          PROP_SPEC_REQUIRES_STATE_UNSET: ({ "lit" }),
          PROP_SPEC_ALREADY_SET_MSG:      _LANG_PROP_FIREPLACE_ALREADY_LIT,
          PROP_SPEC_SET_STATE:            ([ "lit": 1 ]),
          PROP_SPEC_MSG_ME:               _LANG_PROP_FIREPLACE_LIGHT_ME,
          PROP_SPEC_MSG_OTHERS:           _LANG_PROP_FIREPLACE_LIGHT_OTHERS,
        ]),

        "extinguish": ([
          PROP_SPEC_KIND:             PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:            _LANG_PROP_FIREPLACE_EXT_VERBS,
          PROP_SPEC_REQUIRES_STATE:   ({ "lit" }),
          PROP_SPEC_MISSING_MSG:      _LANG_PROP_FIREPLACE_NOT_LIT,
          PROP_SPEC_CLEAR_STATE:      ({ "lit" }),
          PROP_SPEC_MSG_ME:           _LANG_PROP_FIREPLACE_EXT_ME,
          PROP_SPEC_MSG_OTHERS:       _LANG_PROP_FIREPLACE_EXT_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  bar — tavern counter. Stateless decorative.
    // ------------------------------------------------------------
    "bar": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_BAR_ID,
        _LANG_PROP_BAR_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_BAR_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_BAR_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WOOD,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_BAR_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_BAR_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WOOD }),

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "lean": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:        _LANG_PROP_BAR_LEAN_VERBS,
          PROP_SPEC_MSG_ME:       _LANG_PROP_BAR_LEAN_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_BAR_LEAN_OTHERS,
        ]),

        "smell": ([
          PROP_SPEC_KIND:     PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:    _LANG_PROP_BAR_SMELL_VERBS,
          PROP_SPEC_MSG_ME:   _LANG_PROP_BAR_SMELL_ME,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  bunk — one sleeper at a time
    // ------------------------------------------------------------
    "bunk": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_BUNK_ID,
        _LANG_PROP_BUNK_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_BUNK_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_BUNK_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_MALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WOOD,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_BUNK_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_BUNK_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WOOD }),

      PROP_TYPE_DEFAULT_STATE: ([
        "occupant": nil,
      ]),

      PROP_TYPE_STATE_SUFFIXES: ([
        "occupant": _LANG_PROP_BUNK_SUFFIX_OCCUPIED,
      ]),

      PROP_TYPE_LONG_SUFFIXES: ([
        "occupant": _LANG_PROP_BUNK_LONG_OCCUPIED,
      ]),

      PROP_TYPE_ACTIONS: ([

        "lie": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:                _LANG_PROP_BUNK_LIE_VERBS,
          PROP_SPEC_REQUIRES_STATE_UNSET: ({ "occupant" }),
          PROP_SPEC_ALREADY_SET_MSG:      _LANG_PROP_BUNK_OCCUPIED_BY,
          PROP_SPEC_ALREADY_SELF_MSG:     _LANG_PROP_BUNK_ALREADY_LYING,
          PROP_SPEC_SET_STATE:            ([ "occupant": PROP_VALUE_PLAYER_NAME ]),
          PROP_SPEC_MSG_ME:               _LANG_PROP_BUNK_LIE_ME,
          PROP_SPEC_MSG_OTHERS:           _LANG_PROP_BUNK_LIE_OTHERS,
        ]),

        "rise": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:                _LANG_PROP_BUNK_RISE_VERBS,
          PROP_SPEC_REQUIRES_STATE_MATCH: ([ "occupant": PROP_VALUE_PLAYER_NAME ]),
          PROP_SPEC_MISSING_MATCH_MSG:    _LANG_PROP_BUNK_NOT_LYING,
          PROP_SPEC_CLEAR_STATE:          ({ "occupant" }),
          PROP_SPEC_MSG_ME:               _LANG_PROP_BUNK_RISE_ME,
          PROP_SPEC_MSG_OTHERS:           _LANG_PROP_BUNK_RISE_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  rack — pegs on a wall, no state
    // ------------------------------------------------------------
    "rack": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_RACK_ID,
        _LANG_PROP_RACK_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_RACK_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_RACK_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_MALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WOOD,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_RACK_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_RACK_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WOOD, MAT_METAL }),
      PROP_TYPE_HIDE_MATERIAL: 1,

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "search": ([
          PROP_SPEC_KIND:       PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:      _LANG_PROP_RACK_SEARCH_VERBS,
          PROP_SPEC_MSG_ME:     _LANG_PROP_RACK_SEARCH_ME,
          PROP_SPEC_MSG_OTHERS: _LANG_PROP_RACK_SEARCH_OTHERS,
        ]),

        "smell": ([
          PROP_SPEC_KIND:   PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:  _LANG_PROP_RACK_SMELL_VERBS,
          PROP_SPEC_MSG_ME: _LANG_PROP_RACK_SMELL_ME,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  arms rack — the weapons belong to somebody, no state
    // ------------------------------------------------------------
    "arms_rack": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_ARMS_ID,
        _LANG_PROP_ARMS_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_ARMS_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_ARMS_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_MALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WOOD,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_ARMS_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_ARMS_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WOOD, MAT_METAL }),
      PROP_TYPE_HIDE_MATERIAL: 1,

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "count": ([
          PROP_SPEC_KIND:       PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:      _LANG_PROP_ARMS_COUNT_VERBS,
          PROP_SPEC_MSG_ME:     _LANG_PROP_ARMS_COUNT_ME,
          PROP_SPEC_MSG_OTHERS: _LANG_PROP_ARMS_COUNT_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  cobweb — decorative, stateless. Scenery of a spider lair.
    // ------------------------------------------------------------
    "cobweb": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_COBWEB_ID,
        _LANG_PROP_COBWEB_ID_ALIAS_1,
      }),

      PROP_TYPE_NOUN:             _LANG_PROP_COBWEB_NOUN,
      PROP_TYPE_NOUN_PLURAL:      _LANG_PROP_COBWEB_NOUN_PLURAL,
      PROP_TYPE_GENDER:           GENDER_FEMALE,
      PROP_TYPE_DEFAULT_MATERIAL: MAT_WEB,

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_COBWEB_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_COBWEB_LONG,
      PROP_TYPE_MATERIALS:  ({ MAT_WEB }),
      // the short and long already say what it is woven of, so the phrase
      // would only repeat itself
      PROP_TYPE_HIDE_MATERIAL: 1,

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "touch": ([
          PROP_SPEC_KIND:       PROP_PLAN_GENERIC,
          PROP_SPEC_VERBS:      _LANG_PROP_COBWEB_TOUCH_VERBS,
          PROP_SPEC_MSG_ME:     _LANG_PROP_COBWEB_TOUCH_ME,
          PROP_SPEC_MSG_OTHERS: _LANG_PROP_COBWEB_TOUCH_OTHERS,
        ]),

      ]),
    ]),

  ]);
}

mapping query_types() { return types; }
