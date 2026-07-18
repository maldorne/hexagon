// Road component. Attach to a location that is a traversable line
// through the terrain: a footpath through the woods, a country lane,
// a paved imperial highway, a city street segment. The sector-level
// map renderer treats sectors with roads as candidates for corridor
// glyphs (─ │ ┼ etc), joining adjacent sectors that share the
// direction of travel.
//
// Classifier `kind` grades traffic capacity, roughly small → large:
//   path < trail < road < highway

#include <room/location.h>

inherit component "/lib/location/component.c";

// path | trail | road | highway
private string kind;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_ROAD);
  kind = "path";
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_kind() { return kind; }
void set_kind(string s) { kind = s; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "kind": kind, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["kind"]))
    kind = args["kind"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Kind", kind, }), });
}
