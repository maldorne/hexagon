// Library component: the location is a library, with the same behaviour as
// the library room (/lib/room/libraries/library-room.c), both from
// library-actions.c. On every load it puts up the sign that explains the
// library and, once the location's components are all there, adds a shelf if
// the location has none among its props. The
// books on its shelves are saved with the location.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/libraries/library-actions.c";

void add_library_props();

void create()
{
  actions::create();
  component::create();

  set_type(LOCATION_COMPONENT_LIBRARY);
}

object query_library_place() { return query_my_location(); }

void save_library()
{
  if (query_my_location())
    query_my_location()->save_me();
}

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (!loc)
    return;

  create_library_sign(loc);
}

// Once every component of the location exists, the props one can be reached.
void components_ready()
{
  add_library_props();
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_library_sign();
}

// Every library has shelves: give it one, attaching the props component if
// the location has none yet. A location that already has shelves keeps them.
void add_library_props()
{
  object loc, props;

  loc = query_my_location();
  if (!loc)
    return;

  props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!props)
  {
    loc->add_component(LOCATION_COMPONENT_PROPS, ([ "props_instances": ({ }) ]));
    props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
    if (!props)
      return;
  }

  if (sizeof(props->query_instances_by_type("shelf")))
    return;

  props->add_prop_instance("shelf", "shelf_1", ([
      "description" : query_library_shelf_description(),
      "removed_actions" : ([ "browse" : 1 ]),
    ]));
  props->refresh_actions();

  // saved once the location has finished loading
  call_out("save_location", 0);
}

void save_location()
{
  if (query_my_location())
    query_my_location()->save_me();
}

string query_info()
{
  return sizeof(query_library_books()) + " books";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "library_books" : query_library_books() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["library_books"]))
    set_library_books(args["library_books"]);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
