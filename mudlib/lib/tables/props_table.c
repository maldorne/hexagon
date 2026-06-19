
/*
 * props_table.c
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
 * Per-game overrides live at /games/<game>/tables/props_table.c
 * (standard hexagon table convention). A per-game entry with the
 * same key replaces the base entry on the registry side.
 *
 * Player-visible strings come from <translations/props.h>, which
 * the driver rewrites to the .en.h / .es.h variant according to
 * GLOBAL_COMPILE_LANG.
 */

#include <room/prop.h>
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
        _LANG_PROP_CHAIR_ID_ALIAS_2,
      }),

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_CHAIR_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_CHAIR_LONG,
      PROP_TYPE_MATERIALS:  ({ "wood" }),

      PROP_TYPE_DEFAULT_STATE: ([
        "tipped":    0,
        "occupant":  nil,
      ]),

      PROP_TYPE_STATE_SUFFIXES: ([
        "tipped":    _LANG_PROP_CHAIR_SUFFIX_TIPPED,
        "occupant":  _LANG_PROP_CHAIR_SUFFIX_OCCUPIED,
      ]),

      PROP_TYPE_ACTIONS: ([

        // sit: requires occupant unset (or self), sets occupant.
        // Blocked by tipped state and by overrides.props.broken.
        "sit": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
          PROP_SPEC_BLOCKED_BY:           ({ "tipped", "broken" }),
          PROP_SPEC_BLOCKED_MSG:          _LANG_PROP_CHAIR_CANT_SIT,
          PROP_SPEC_REQUIRES_STATE_UNSET: ({ "occupant" }),
          PROP_SPEC_ALREADY_SET_MSG:      _LANG_PROP_CHAIR_OCCUPIED_BY,
          PROP_SPEC_SET_STATE:            ([ "occupant": PROP_VALUE_PLAYER_NAME ]),
          PROP_SPEC_MSG_ME:               _LANG_PROP_CHAIR_SIT_ME,
          PROP_SPEC_MSG_OTHERS:           _LANG_PROP_CHAIR_SIT_OTHERS,
        ]),

        // stand: only the current occupant can do it.
        "stand": ([
          PROP_SPEC_KIND:                 PROP_PLAN_GENERIC,
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
          PROP_SPEC_REQUIRES_STATE:   ({ "tipped" }),
          PROP_SPEC_MISSING_MSG:      _LANG_PROP_CHAIR_NOT_TIPPED,
          PROP_SPEC_CLEAR_STATE:      ({ "tipped" }),
          PROP_SPEC_MSG_ME:           _LANG_PROP_CHAIR_RIGHT_ME,
          PROP_SPEC_MSG_OTHERS:       _LANG_PROP_CHAIR_RIGHT_OTHERS,
        ]),

      ]),
    ]),

    // ------------------------------------------------------------
    //  table — no state in v1
    // ------------------------------------------------------------
    "table": ([

      PROP_TYPE_ID_LIST: ({
        _LANG_PROP_TABLE_ID,
        _LANG_PROP_TABLE_ID_ALIAS_1,
      }),

      PROP_TYPE_SHORT_KEY:  _LANG_PROP_TABLE_SHORT,
      PROP_TYPE_LONG_KEY:   _LANG_PROP_TABLE_LONG,
      PROP_TYPE_MATERIALS:  ({ "wood" }),

      PROP_TYPE_DEFAULT_STATE: ([ ]),

      PROP_TYPE_ACTIONS: ([

        "smell": ([
          PROP_SPEC_KIND:     PROP_PLAN_GENERIC,
          PROP_SPEC_MSG_ME:   _LANG_PROP_TABLE_SMELL_ME,
        ]),

        "lean_on": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_MSG_ME:       _LANG_PROP_TABLE_LEAN_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_TABLE_LEAN_OTHERS,
        ]),

        "climb": ([
          PROP_SPEC_KIND:         PROP_PLAN_GENERIC,
          PROP_SPEC_MSG_ME:       _LANG_PROP_TABLE_CLIMB_ME,
          PROP_SPEC_MSG_OTHERS:   _LANG_PROP_TABLE_CLIMB_OTHERS,
        ]),

      ]),
    ]),

  ]);
}

mapping query_types() { return types; }
