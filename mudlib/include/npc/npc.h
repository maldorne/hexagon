
#define NPC_OB       "/lib/npc"

#define MONSTER_HAND "/lib/handlers/npcs.c"
#define NO_SPAM      "no_spam"

// #define NO_WANDER
#define MIN_PLAYER_LEVEL 1
#define MIN_MONSTER_LEVEL 10
// Idle wander cadence: a step every 300 + random(300) heart_beats -- a slow,
// occasional drift.
#define MOVE_DEFAULT ({ 300, 300 })
// Directed-travel cadence: a step every heart_beat (~one room per 2s), so a
// walking NPC is visible and interactable instead of teleporting. Per-NPC speed
// overrides this via set_travel_speed.
#define TRAVEL_DEFAULT ({ 1, 0 })

// Template fields a builder fills in by hand, copied from the NPC's source .c
// during conversion. The sampler cannot produce any of them: either the source
// has no way to express them (sentience, an equipment kit, a daily timetable),
// or the value is rolled per clone so a sample would show one throw of the dice
// instead of the rule behind it (a stat range, a starting purse). Anything a
// clone reveals honestly -- chatter, wander zones, wimpy, aggression, the social
// objects -- is sampled instead and does not belong on this list.
//
// add_template carries this whole list across a re-extraction, so reconverting
// an area never destroys the work.
#define HAND_SET_TEMPLATE_FIELDS ({ \
    "level",              /* pin an absolute level, ignoring the area band  */ \
    "level_area_modifier",/* sit N levels off the area average             */ \
    "sentient",           /* generated name + gender, gets a schedule      */ \
    "equipment",          /* kit rolled once at the first materialization  */ \
    "timetable",          /* hour-of-day work/home schedule                */ \
    "extracted_from",     /* the type this one was copied from, if any     */ \
    "random_stats",       /* ([ "low": n, "high": n ]) rolled per spawn    */ \
    "stats",              /* individual stats applied over the roll        */ \
    "max_hp", "max_gp",   /* current hp/gp start at the maximum            */ \
    "money",              /* ([ "type": s, "base": n, "spread": n ])       */ \
  })
