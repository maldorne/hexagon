
// houses.en.h — English strings for the plot and home location components.
//
// Game code uses `#include <translations/houses.h>` and the driver rewrites the
// include to .en.h / .es.h per GLOBAL_COMPILE_LANG. See
// /lib/core/driver.c::include_file.

#define _LANG_PLOT_SHORT "Empty plot"
#define _LANG_PLOT_LONG \
    "An empty, undeveloped plot of land. The ground is cleared and level, " + \
    "ready for a building to be raised on it.\n"

#define _LANG_HOME_SHORT "House"
#define _LANG_HOME_LONG \
    "The interior of a modest dwelling. Bare walls and a packed-earth floor " + \
    "await the furniture and belongings of whoever comes to live here.\n"
