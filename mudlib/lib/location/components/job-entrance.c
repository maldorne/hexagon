// Job entrance component: the location is where a job is taught, with the same
// behaviour as the job entrance room (/lib/room/jobs/job-entrance-room.c), both
// from job-entrance-actions.c. On every load it puts up the sign that explains
// the job.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/jobs/job-entrance-actions.c";

void create()
{
  actions::create();
  component::create();

  set_type(LOCATION_COMPONENT_JOB_ENTRANCE);
}

object query_job_entrance_place() { return query_entity(); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (!loc)
    return;

  create_job_entrance_sign(loc);
}

// Name the job, and say so on the sign at once.
void set_entrance_job(string path)
{
  actions::set_entrance_job(path);
  if (query_entity())
    create_job_entrance_sign(query_entity());
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_job_entrance_sign();
}

string query_info()
{
  return strlen(query_entrance_job()) ? query_entrance_job() : "no job";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "entrance_job" : query_entrance_job() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (args && stringp(args["entrance_job"]))
    actions::set_entrance_job(args["entrance_job"]);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
