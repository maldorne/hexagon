
#define AREA_HANDLER "/lib/handlers/areas"

// How many ancestor areas a lookup may climb before giving up, so a malformed
// area path cannot spin forever.
#define AREA_MAX_ANCESTRY 8
#define AREA_STORAGE_OBJECT "/lib/location/area"
// periodic NPC population sweep (see /lib/handlers/population.c)
#define POPULATION_HANDLER "/lib/handlers/population"
