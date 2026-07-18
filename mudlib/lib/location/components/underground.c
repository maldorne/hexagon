// Underground component. Attach to any location that sits below the
// surface: a natural cave, a mine gallery, a sewer tunnel, a dungeon
// chamber. Contributes to the sector-level "underground" tally so the
// world map can render the sector as subterranean.
//
// Classifier `kind` names the character of the space. No implicit
// ordering — pick whichever fits:
//   cave | tunnel | mine | dungeon

#include <room/location.h>

inherit component "/lib/location/component.c";

// cave | tunnel | mine | dungeon
private string kind;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_UNDERGROUND);
  kind = "cave";
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
