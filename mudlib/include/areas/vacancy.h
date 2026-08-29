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
//      VACANCY_POI:    1,                  // anchored to a point of interest
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
// anchored to a point of interest: its holder respawns there after a delay
// rather than waiting for the settlement pass
#define VACANCY_POI       "poi"
// holders spread over the area's other locations of the same kind instead of
// crowding the one the job names
#define VACANCY_SPREAD    "spread"
// what the job trains its holders in
#define VACANCY_CLASS     "class"
// the house that comes with the job, if it comes with one
#define VACANCY_HOME      "home"

// The census row of somebody who holds a job. Three places, and they are three
// different questions: which post this is, where this person works it, and
// where the person happens to be.
//
// A spread job seats each holder in a different field, so its holders work in
// places the post itself does not name; and any of them may be at home, or
// halfway there, at the hour you ask.
#define CENSUS_VACANCY        "vacancy"
// which post: the place the job is held, matching the vacancy's works_at
#define CENSUS_VACANCY_AT     "vacancy_at"
// where this person works it, which a spread job moves away from the post
#define CENSUS_WORKS_AT       "works_at"
// where the person is right now
#define CENSUS_LOCATION       "current_location"

#endif
