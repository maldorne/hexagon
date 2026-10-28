// Crimes: every default number of the crimes handler lives here. A game's own
// crimes handler (/games/<game>/handlers/crimes.c) can override any of them by
// redefining the matching query_* function.

#ifndef _CRIMES_H
#define _CRIMES_H

#define CRIMES_HANDLER "/lib/handlers/crimes"

// the shared default; a game's handler keeps its own file
#define CRIMES_SAVE "/save/crimes"

// --- offence types (persisted in the records: do not rename) ----------------

#define CRIME_THEFT         "theft"
#define CRIME_ASSAULT       "assault"
#define CRIME_MURDER        "murder"
#define CRIME_TRESPASS      "trespass"
#define CRIME_GUARD_ASSAULT "guard_assault"

#define CRIME_TYPES ({ CRIME_THEFT, CRIME_ASSAULT, CRIME_MURDER, \
                       CRIME_TRESPASS, CRIME_GUARD_ASSAULT })

// --- law code ---------------------------------------------------------------

// points each offence adds when a citizenship's law does not say otherwise;
// 0 would make it legal
#define CRIME_DEFAULT_LAW ([ CRIME_THEFT : 10, CRIME_ASSAULT : 15, \
                             CRIME_MURDER : 60, CRIME_TRESPASS : 5, \
                             CRIME_GUARD_ASSAULT : 40 ])

// whether the guards of a citizenship kill instead of arresting (1 or 0)
#define CRIME_DEFAULT_LETHAL 1

// --- status -----------------------------------------------------------------

#define CRIME_STATUS_CLEAN   "clean"
#define CRIME_STATUS_SUSPECT "suspect"
#define CRIME_STATUS_WANTED  "wanted"
#define CRIME_STATUS_OUTLAW  "outlaw"

// from the lowest to the highest
#define CRIME_STATUSES ({ CRIME_STATUS_CLEAN, CRIME_STATUS_SUSPECT, \
                          CRIME_STATUS_WANTED, CRIME_STATUS_OUTLAW })

// points from which each status applies
#define CRIME_SUSPECT_POINTS 10
#define CRIME_WANTED_POINTS  40
#define CRIME_OUTLAW_POINTS  100

// --- records ----------------------------------------------------------------

// points lost every period (real time, applied lazily when a record is read)
#define CRIME_DECAY_POINTS 5
#define CRIME_DECAY_PERIOD 86400

// recent offences kept for each player and citizenship
#define CRIME_RECENT_MAX 20

// whether the offences of NPCs are recorded (the event fires either way)
#define CRIME_RECORD_NPCS 0

// --- witnesses --------------------------------------------------------------

// minimum light for a witness without a race to judge it (an NPC) to see
#define CRIME_WITNESS_MIN_LIGHT 25

// --- fights -----------------------------------------------------------------

// seconds during which the same aggressor attacking the same victim again is
// not reported twice
#define CRIME_FIGHT_WINDOW 60

// seconds a fight start is remembered, to tell a murder from a death in
// self-defence
#define CRIME_FIGHT_MEMORY 1800

// --- consequences (used by the guards, later phases) ------------------------

// percentage of the points that survive being killed by a guard
#define CRIME_DEATH_KEEP_PERCENT 50

// seconds before a pursuing guard gives up
#define CRIME_PURSUIT_GIVE_UP 300

// guards that answer a crime
#define CRIME_RESPONDERS 2

// fine, in base coins, for each point
#define CRIME_FINE_PER_POINT 5

// seconds of jail for each point
#define CRIME_JAIL_SECONDS_PER_POINT 30

// --- justice: beaten, arrested, fined (phase 4) -----------------------------

// seconds a living beaten by a non-lethal guard stays passed out
#define CRIME_PASSED_OUT_SECONDS 30

// longest sentence, in seconds, whatever the points
#define CRIME_JAIL_MAX_SECONDS 1800

// percentage of the points that remain once a sentence is served
#define CRIME_RELEASE_KEEP_PERCENT 25

// points a sentence is counted from for an NPC that has no record
#define CRIME_NPC_ARREST_POINTS 10

// the fine grows by this percentage for each level of whoever pays it
#define CRIME_FINE_LEVEL_PERCENT 5

// --- capture: a guard takes a beaten target to prison ------------------------

// a guard is sent for a beaten target only when it stands closer than this: the
// larger of the differences in x and y between its place and the target's,
// plus the difference in z (what the pathfinding counts as steps)
#define CRIME_ARREST_GUARD_RANGE 30

// heart beats the target of an arrest stays unconscious past each look of the
// escort
#define CRIME_CAPTIVE_HOLD 5

// seconds the whole arrest may take, from the call to the cell door
#define CRIME_ARREST_SECONDS 900

// bribing a jailer: base coins for each point owed, chance of success in
// percent, and the points a failed attempt adds
#define CRIME_BRIBE_COST_PER_POINT 10
#define CRIME_BRIBE_CHANCE         40
#define CRIME_BRIBE_FAIL_POINTS    10

#endif
