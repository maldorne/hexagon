// Fines component. The location is the office where what a record owes is
// paid, and where somebody else's bail is posted: the same behaviour as the
// fines office room (/lib/room/justice/fines-office.c), both from
// fines-actions.c. The law is the one of the place's area, unless the room it
// was converted from named its own.
//
// Nothing of the sign is stored: it is put up on every load from the fines
// help document, so changing that document changes every office.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/justice/fines-actions.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FINES);
}

void dest_me() {}

object query_fines_place() { return query_entity(); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (loc)
    create_fines_sign(loc);
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_fines_sign();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         (fines_law && strlen(fines_law) ? ([ "fines_law" : fines_law ]) : ([ ]));
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["fines_law"]))
    set_fines_law(args["fines_law"]);
}

string query_info()
{
  string law;

  law = query_fines_jurisdiction();
  return strlen(law) ? "law of " + law : "no law";
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
