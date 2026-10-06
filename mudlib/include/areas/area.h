
#define AREA_HANDLER "/lib/handlers/areas"

// How many ancestor areas a lookup may climb before giving up, so a malformed
// area path cannot spin forever.
#define AREA_MAX_ANCESTRY 8
#define AREA_STORAGE_OBJECT "/lib/location/area"
// periodic NPC population sweep (see /lib/handlers/population.c)
#define POPULATION_HANDLER "/lib/handlers/population"

// How far along an area is. A draft is still being shaped: its map may be
// wiped and rebuilt, and nothing that outlives a location -- a family -- may be
// founded in it yet. A settled area holds people, houses and families, so the
// wipe is refused and only the steps that preserve what is there remain.
#define AREA_DRAFT   "draft"
#define AREA_SETTLED "settled"

// A house on the community's books: who lives in it, which family owns it, and
// whether it was raised for something in particular (a barracks, a guildhall)
// and so is never handed to whoever is homeless.
// A communal house (a barracks) is shared by whoever holds its job: nobody living
// there founds a family by it nor marries anybody into it.
//   ([ location file : ([ HOUSE_RESIDENTS: ({ uuid }), HOUSE_OWNER: surname,
//                         HOUSE_KEPT: 1, HOUSE_COMMUNAL: 1 ]) ])
#define HOUSE_RESIDENTS "residents"
#define HOUSE_OWNER     "owner"
#define HOUSE_KEPT      "kept"
#define HOUSE_COMMUNAL  "communal"
