// Mountain component. Attach to steep rock: peaks, slopes, passes. Contributes
// to the sector-level "mountain" tally so the world map draws a brown
// triangle.
//
// A mountain may declare a kind (MOUNTAIN_KIND_VOLCANO); with none it is plain
// rock. The kind is saved with the location. The sector keeps counting it as
// mountain; when most of the mountain locations of a sector are volcano the
// world map draws the triangle in the volcano colour (see
// /lib/handlers/worldmap.c).
//
// A room declares it by answering query_volcano() with 1: room2loc attaches
// the component (or sets the kind on one already there).
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

// "" for plain mountain, else a MOUNTAIN_KIND_* (saved)
private string kind;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_MOUNTAIN);
  kind = "";
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

/**
 * The kind of this mountain, "" for a plain one.
 */
string query_kind() { return kind; }

/**
 * Sets the kind of this mountain.
 *
 * @param s  MOUNTAIN_KIND_VOLCANO, or "" for plain mountain
 */
void set_kind(string s)
{
  kind = s ? s : "";
  if (query_entity())
    query_entity()->components_changed();
}

mapping query_auto_load_attributes()
{
  // a plain mountain saves nothing more than its type
  return component::query_auto_load_attributes() +
         (strlen(kind) ? ([ "mountain_kind": kind ]) : ([ ]));
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (stringp(args["mountain_kind"]))
    kind = args["mountain_kind"];
}

mixed * stats()
{
  return component::stats() + ({ ({ "Kind", kind, }) });
}
