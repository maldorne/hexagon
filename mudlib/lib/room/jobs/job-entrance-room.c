// A room where a job is taught. What the entrance does lives in
// job-entrance-actions.c; this file only makes it a room and puts up the sign
// that explains the job. The room names its job in setup() with
// set_entrance_job(<path of the job object>), and converting it to a location
// carries it to the job entrance component.

inherit room    "/lib/room.c";
inherit actions "/lib/room/jobs/job-entrance-actions.c";

void create()
{
  actions::create();
  room::create();

  create_job_entrance_sign(this_object());
}

void init()
{
  room::init();
  actions::init();
}

mixed * stats()
{
  return room::stats() + actions::stats();
}
