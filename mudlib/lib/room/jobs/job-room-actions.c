// Job room: the main hall of the house where a job is taught, where those who
// hold the job gather. Shared by the job room (/lib/room/jobs/job-room.c) and
// the job room location component (/lib/location/components/job-room.c).
// Those who hold the job read its rules here (normas). A job levels by itself,
// so there is no advancing; the general coste command shows the next level.

#include <language.h>

// the job this is the hall of, as the path of its object, ending in ".c"
private string room_job;
static object room_sign;

int query_job_room() { return 1; }

// Where the hall stands. The component answers with its location.
object query_job_room_place() { return this_object(); }

void create()
{
  room_job = "";
}

string query_room_job() { return room_job; }

/**
 * Names the job this is the hall of.
 *
 * @param path the job object, with or without ".c"; "" for none
 */
void set_room_job(string path)
{
  if (!path)
    path = "";
  if (strlen(path) && (strlen(path) < 3 || path[strlen(path) - 2 ..] != ".c"))
    path += ".c";
  room_job = path;
}

// Put up the sign that says what can be done in the hall, replacing any earlier
// one, so a place never has two.
object create_job_room_sign(object where)
{
  if (room_sign)
    room_sign->dest_me();

  room_sign = where->add_sign(_LANG_JOB_ROOM_SIGN_LONG, _LANG_JOB_ROOM_SIGN_TEXT);
  return room_sign;
}

// Take the sign down again, when the hall leaves its place.
void take_down_job_room_sign()
{
  if (room_sign)
    room_sign->dest_me();
  room_sign = nil;
}

void init()
{
  add_action("do_rules", _LANG_JOB_ROOM_CMDS_RULES);
}

// normas: the rules of the job, read from its object and its rules book, for
// those who hold it.
int do_rules(string str)
{
  string text;
  object me;

  me = this_player();
  if (!me)
    return 0;

  if (!strlen(room_job))
  {
    notify_fail(_LANG_JOB_ROOM_NO_JOB);
    return 0;
  }

  if (me->query_job_ob() != room_job)
  {
    notify_fail(_LANG_JOB_ROOM_NOT_MEMBER);
    return 0;
  }

  text = handler("rules")->query_rules(room_job, me, query_job_room_place());
  if (!text)
  {
    notify_fail(_LANG_JOB_ROOM_NO_JOB);
    return 0;
  }

  tell_object(me, text);
  return 1;
}

mixed * stats()
{
  return ({ ({ "Room Job", room_job }) });
}
