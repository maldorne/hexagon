
// A place to locate the controller
#define HOME "/games/hexagon/areas/admin/weather.c"

#define DIPLOMACY_HANDLER "/obj/handlers/diplomacy.c"
#define DIPLOMACY_TABLE "/tables/diplomacy.c"

#define BASE_GUARD "/std/guard.c"

// Log file name
#define LOG_DIPLOMACY "diplomacy"

#define NEUTRAL "Neutral"
// Names of the different diplomacies/citizenships
#define CIUDAD_CAPITAL "Ciudad Capital"
#define DARGAAD "Dargaad"
#define SQARGELD "Sqargeld"
#define DELKHAINE "Del-Khâine"
#define NODD "Nodd"
#define ROTH "Roth"
#define MONGUL "Mongûl"
#define NEUBURG "Neuburg"
#define ALORA "Alora"
#define ANDUNN "Andunn"
#define NHORROD "Nhorrod"


#define ZONES ({ CIUDAD_CAPITAL, DARGAAD, SQARGELD, DELKHAINE, NODD, \
                 ROTH, MONGUL, NEUBURG, ALORA, ANDUNN, NHORROD })


// Positions in the diplomacy info array
#define DIPLOMACY_PARENT_CITY 0
#define DIPLOMACY_ALLIES 1
#define DIPLOMACY_ENEMIES 2


// Points of interest: types of rooms
#define POI_TOWN_SQUARE 1
#define POI_TOWN_DOOR 2
#define POI_GUARD_CREATION 3
#define POI_GUARD_POINT 4

