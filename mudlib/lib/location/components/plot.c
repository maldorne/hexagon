// Plot component. Marks a buildable empty lot -- a location carved out by a
// programmer (the builder-ring `build plot <dir>` command) and registered with
// its area, waiting for a house to be raised on it. It provides the location's
// default title and description (a vacant lot); building a house adds the `home`
// component, which takes over the description.
//
// The lot is never created procedurally: a programmer runs the command, and if
// the target coordinate is occupied it refuses. This component is the persisted
// marker that tells the area a location is an available plot.

#include <room/location.h>
#include <translations/houses.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_PLOT);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

mapping query_hooks()
{
  return ([ "short": HOOK_PRIORITY_STRUCTURE,
            "long":  HOOK_PRIORITY_STRUCTURE ]);
}

// The empty lot's title. Exclusive: it is the whole short.
mixed hook_short(mixed * args)
{
  return ({ HOOK_EXCLUSIVE, _LANG_PLOT_SHORT });
}

// The empty lot's description. args = ({ str, dark }); a non-empty str means the
// player is looking at a specific item, not the room, so contribute nothing.
mixed hook_long(mixed * args)
{
  if (args[0] && strlen(args[0]))
    return "";
  return ({ HOOK_EXCLUSIVE, _LANG_PLOT_LONG });
}
