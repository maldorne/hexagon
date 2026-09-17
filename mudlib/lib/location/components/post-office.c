// Post office component: the location becomes a place to read and send mail,
// with the same behaviour as the post office room.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/ventures/post-office-actions.c";

void create()
{
  component::create();

  set_type(LOCATION_COMPONENT_POST_OFFICE);
}

void init()
{
  actions::init();
}
