// Coast component. Attach to any location at a land–water boundary:
// a sandy beach, a rocky sea cliff, a fishing dock, a bustling
// harbour. Contributes to the sector-level "coast" tally so the
// world map can render the sector as shoreline.
//
// Classifier `kind` names the shape of the interface. No implicit
// ordering — pick whichever fits:
//   beach | cliff | port | harbor

#include <room/location.h>

inherit component "/lib/location/component.c";

// beach | cliff | port | harbor
private string kind;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_COAST);
  kind = "beach";
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
