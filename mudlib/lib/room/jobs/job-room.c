// A room that is the main hall of the house where a job is taught. What the
// hall does lives in job-room-actions.c; this file only makes it a room and
// puts up the sign that says what can be done in it. The room names its job
// in setup() with set_room_job(<path of the job object>), and converting it to
// a location carries it to the job room component.

inherit room    "/lib/room.c";
inherit actions "/lib/room/jobs/job-room-actions.c";

void create()
{
  actions::create();
  room::create();

  create_job_room_sign(this_object());
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
