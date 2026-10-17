// Job room component: the location is the main hall of the house where a job
// is taught, with the same behaviour as the job room
// (/lib/room/jobs/job-room.c), both from job-room-actions.c. On every load it
// puts up the sign that says what can be done in the hall.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/jobs/job-room-actions.c";

void create()
{
  actions::create();
  component::create();

  set_type(LOCATION_COMPONENT_JOB_ROOM);
}

object query_job_room_place() { return query_entity(); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (!loc)
    return;

  create_job_room_sign(loc);
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_job_room_sign();
}

string query_info()
{
  return strlen(query_room_job()) ? query_room_job() : "no job";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "room_job" : query_room_job() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (args && stringp(args["room_job"]))
    set_room_job(args["room_job"]);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
