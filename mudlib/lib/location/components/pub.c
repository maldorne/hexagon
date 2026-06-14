
#include <room/location.h>

inherit component  "/lib/location/component.c";
inherit attendable "/lib/ventures/attendable.c";
inherit sign       "/lib/ventures/pub-sign.c";
inherit menu       "/lib/ventures/pub-menu.c";
inherit actions    "/lib/ventures/pub-actions.c";
inherit food       "/lib/ventures/food.c";

void create()
{
  attendable::create();
  menu::create();
  component::create();

  set_type(LOCATION_COMPONENT_PUB);
}

void init() 
{
  actions::init();
}

void dest_me() 
{

}

// will be called after creation + init_auto_load_attributes,
// providing the parent location object
void initialize(object location)
{
  component::initialize(location);

  // add a sign with available commands and the menu
  create_sign(location);

  // include the pub in the ventures handler
  handler("ventures")->include_pub(location->query_file_name());
}

// Chain autoload through every mixin that owns persistent state. Each
// mixin namespaces its keys with its own prefix (see /lib/ventures/*.c)
// so the merged mapping is collision-free.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         attendable::query_auto_load_attributes() +
         menu::query_auto_load_attributes();
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  attendable::init_auto_load_attributes(args);
  menu::init_auto_load_attributes(args);
}

mixed * stats()
{
  return component::stats() +
        attendable::stats()+
        menu::stats();
}
