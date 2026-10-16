// Deities: worship requirements, offerings, prayers and the deities handler.
// See /lib/room/temple/temple.md.

#ifndef DEITIES_H
#define DEITIES_H

#ifndef MAX_ALIGN_INT
#include <basic/alignment.h>
#endif

// Alignment bands for set_alignment_band, on the real alignment. The neutral
// band is the one the alignment table names neutral (0..100).
#define DEITY_ALIGN_ANY      ({ -MAX_ALIGN_INT, MAX_ALIGN_INT })
#define DEITY_ALIGN_EVIL     ({ -MAX_ALIGN_INT, -1 })
#define DEITY_ALIGN_NEUTRAL  ({ 0, 100 })
#define DEITY_ALIGN_GOOD     ({ 101, MAX_ALIGN_INT })
#define DEITY_ALIGN_NOT_EVIL ({ 0, MAX_ALIGN_INT })
#define DEITY_ALIGN_NOT_GOOD ({ -MAX_ALIGN_INT, 100 })

// Offering rules (add_offering): what a deity accepts, first match wins.
#define DEITY_OFFER_KIND     "kind"      // one of the kinds below
#define DEITY_OFFER_MATERIAL "material"  // a material id or its parent ("metal")
#define DEITY_OFFER_BASE     "base"      // the base file of one item, with ".c"
#define DEITY_OFFER_MIN      "min_value" // the least value the thing must have
#define DEITY_OFFER_WORTH    "worth"     // percent of the value it counts for
#define DEITY_OFFER_REPLY    "reply"     // what the deity answers, optional
// for corpses and body parts: whose body it was
#define DEITY_OFFER_PART      "part"      // part ids taken ({ "heart" }), kind part only
#define DEITY_OFFER_RACES     "races"     // race ids; a race takes its lineages too
#define DEITY_OFFER_ALIGN_MIN "align_min" // the real alignment of the dead, both ends in
#define DEITY_OFFER_ALIGN_MAX "align_max"
#define DEITY_OFFER_LEVEL_MIN "level_min" // the least level of the dead
#define DEITY_OFFER_PLAYERS   "players"   // 0 refuses the bodies of players (taken by default)

#define DEITY_OFFER_COINS  "coins"
#define DEITY_OFFER_ITEM   "item"    // any item
#define DEITY_OFFER_WEAPON "weapon"
#define DEITY_OFFER_ARMOUR "armour"
#define DEITY_OFFER_SHIELD "shield"
#define DEITY_OFFER_CORPSE "corpse"  // a whole body, carried or lying in the temple
#define DEITY_OFFER_BODY_PART "part" // a part taken from one (/lib/obj/death/body-part.c)

// the least value an item needs to be taken by the default rule
#define DEITY_VALUABLE_MIN 100

// Prayers (add_prayer): id -> ([ words, luck ]).
#define DEITY_PRAYER_WORDS "words"
#define DEITY_PRAYER_LUCK  "luck"

// the default prayers of the base deity
#define DEITY_PRAYER_HEAL     "heal"
#define DEITY_PRAYER_POWER    "power"
#define DEITY_PRAYER_BLESSING "blessing"
#define DEITY_PRAYER_STRENGTH "strength"
#define DEITY_PRAYER_WISDOM   "wisdom"

// How a prayer is heard: random(base + the wait left) <= luck is heard, every
// prayer lengthens the wait, past annoyed the deity scolds, past cursed it
// curses.
#define DEITY_PRAY_BASE    500
#define DEITY_PRAY_LUCK    50
#define DEITY_PRAY_ANNOYED 5000
#define DEITY_PRAY_CURSED  8000
#define DEITY_PRAY_PROP       "prayed"
#define DEITY_PRAY_CURSE_PROP "nopray"

// what answer_prayer returns
#define DEITY_PRAYER_UNANSWERED 0
#define DEITY_PRAYER_HEARD      1
#define DEITY_PRAYER_SCOLDED    -1
#define DEITY_PRAYER_CURSED     -2

// Props that can be dedicated to a deity (overrides.deity = <deity path with ".c">).
#define DEITY_PROP_OVERRIDE "deity"
#define DEITY_PROP_TYPES    ({ "altar", "statue", "ruined_statue" })

// The lifetime totals every living keeps about its worship (add_deity_total).
#define DEITY_TOT_WORSHIPPED "worshipped" // time of worship
#define DEITY_TOT_PRAYERS    "prayers"    // prayers made
#define DEITY_TOT_ANSWERED   "answered"   // prayers heard
#define DEITY_TOT_CURSED     "cursed"     // times cursed
#define DEITY_TOT_OFFERINGS  "offerings"  // number of offerings
#define DEITY_TOT_OFFERED    "offered"    // their total worth

// The devotion the deities handler keeps per deity and player: points raised
// by prayers and offerings that fall with the time, an entry at zero is gone.
// A game's handler masks the query_devotion_* functions to change them.
#define DEITY_DEVOTION_PRAYER 1      // points a prayer gives
#define DEITY_DEVOTION_WORTH  100    // worth of an offering for each point it gives
#define DEITY_DEVOTION_MAX    1000   // the most points a player can have in a deity
#define DEITY_DEVOTION_DECAY  1      // points lost every decay period
#define DEITY_DEVOTION_PERIOD 86400  // length of the decay period, in seconds

#endif
