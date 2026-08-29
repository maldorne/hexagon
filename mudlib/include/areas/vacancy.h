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

// census row: the job its holder was taken on for, and where that job is held
#define CENSUS_VACANCY        "vacancy"
#define CENSUS_WORKS_AT       "works_at"
// where the person actually is right now, which a spread job or a day's
// walking moves away from the place the job is held
#define CENSUS_LOCATION       "current_location"

#endif
