#ifndef AREAS_VACANCY_H
#define AREAS_VACANCY_H

// A vacancy is a job a settlement offers and somebody holds: the barman's post
// behind one bar, the four farmers its fields need, the guards its citizenship
// fields at the gates. One record shape for all of them -- what used to be a
// role board, a POI vacancy list and a derived guard count.
//
// The record says what the job is and where, never who does it. Who holds a
// seat is read from the census, which is the only record of a person.
//
//   ([ VACANCY_JOB:    "barman",
//      VACANCY_COUNT:  1,
//      VACANCY_WORKS_AT: "/save/.../ap.o", // where it is held
//      VACANCY_SOURCE: template_id,
//      VACANCY_FIXED:  1,                  // one post, held at that one place
//      VACANCY_SPREAD: 1,                  // holders scatter over like places
//      VACANCY_CLASS:  "/lib/obj/classes/mixed.c",
//      VACANCY_HOME:   "/save/.../plot_30_-2_0.o" ])

// the job's name, as the builder and the census know it
#define VACANCY_JOB       "job"
// how many hold it at this place
#define VACANCY_COUNT     "count"
// where the job is held: a POI, a workplace, a guarded gate
#define VACANCY_WORKS_AT  "works_at"
// the type its holders are drawn from, by template id
#define VACANCY_SOURCE    "source"
// the job is one post held at one place: its holder is replaced there shortly
// after dying rather than waiting for the settlement pass. The opposite of
// VACANCY_SPREAD. Nothing to do with the place being a point of interest -- it
// usually is one, but a fixed post at an ordinary location behaves the same.
#define VACANCY_FIXED     "fixed"
// holders spread over the area's other locations of the same kind instead of
// crowding the one the job names
#define VACANCY_SPREAD    "spread"
// what the job trains its holders in
#define VACANCY_CLASS     "class"
// the house that comes with the job, if it comes with one
#define VACANCY_HOME      "home"
// its holders are housed among the settlement's own, the way a roster source
// flagged "resident" is. The opposite of VACANCY_HOME, which pins every holder
// of the post to one house: a town needs one barman and he sleeps over the bar,
// but its four farmers each want a roof.
#define VACANCY_RESIDENT  "resident"

// The census row of somebody who holds a job: which job, where they work it,
// and where they are.
//
// A job's name is its identity, so a settlement offers one of each. Where the
// job is held is the vacancy's business, not the holder's: a spread job seats
// each of its people in a different field, and any of them may be at home, or
// halfway there, at the hour you ask.
#define CENSUS_VACANCY        "vacancy"
// where this person works, which a spread job moves away from the place the
// job itself names
#define CENSUS_WORKS_AT       "works_at"
// where the person is right now
#define CENSUS_LOCATION       "current_location"

// Seconds before the holder of a fixed post is replaced at its workplace.
#define VACANCY_RESPAWN_DELAY 300

#endif
