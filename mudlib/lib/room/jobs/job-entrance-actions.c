// Job entrance: the place where a job is taught, where it is explained and
// where one takes it up or gives up the job one has. Shared by the job
// entrance room (/lib/room/jobs/job-entrance-room.c) and the job entrance
// location component (/lib/location/components/job-entrance.c).
//
// A school may enrol only its own people (set_entrance_citizenships): joining
// is filtered, members use any house of the job.
//
// One job at a time. A job levels by itself as its experience is earned
// (adjust_job_xp in /lib/living/social.c), so there is no advancing here; the
// general coste command shows what the next level asks. Its rules (normas)
// are read in the job room (/lib/room/jobs/job-room-actions.c), by those who
// hold it.
//
// Ported from the CcMud job entrance (/std/jobentrance.c), neverbot 2002-2006.

#include <language.h>

// the job taught here, as the path of its object, ending in ".c"
private string entrance_job;
// the diplomacy nodes whose people this entrance enrols; empty admits anybody
private string * entrance_citizenships;
static object entrance_sign;

int query_job_entrance() { return 1; }

// Where the entrance stands. The component answers with its location.
object query_job_entrance_place() { return this_object(); }

void create()
{
  entrance_job = "";
  entrance_citizenships = ({ });
}

string query_entrance_job() { return entrance_job; }

string * query_entrance_citizenships() { return entrance_citizenships; }

/**
 * Names the diplomacy nodes whose people may join the job here: those who
 * hold one of them or an ally of one, and those whose race is the people of
 * one. Only joining is filtered; whoever holds the job uses any of its
 * houses.
 *
 * @param nodes diplomacy node names; an empty list admits anybody
 */
void set_entrance_citizenships(string * nodes)
{
  entrance_citizenships = nodes ? nodes : ({ });
}

/**
 * Names the job taught here.
 *
 * @param path the job object, with or without ".c"; "" for none
 */
void set_entrance_job(string path)
{
  if (!path)
    path = "";
  if (strlen(path) && (strlen(path) < 3 || path[strlen(path) - 2 ..] != ".c"))
    path += ".c";
  entrance_job = path;
}

private object _job()
{
  if (!strlen(entrance_job))
    return nil;
  return load_object(entrance_job);
}

// What the sign of the entrance says: the job, what it is and who it admits,
// and how to take it up or give it up.
private string _sign_text()
{
  object job;
  string long;

  job = _job();
  if (!job)
    return _LANG_JOB_ENTRANCE_SIGN_NO_JOB;

  long = (string)job->query_long();
  return _LANG_JOB_ENTRANCE_SIGN_TITLE + (string)job->query_short() + "\n\n" +
         (long ? long + "\n" : "") + _LANG_JOB_ENTRANCE_SIGN_COMMANDS;
}

// Put up the sign that explains the job, replacing any earlier one, so a place
// never has two.
object create_job_entrance_sign(object where)
{
  if (entrance_sign)
    entrance_sign->dest_me();

  entrance_sign = where->add_sign(_LANG_JOB_ENTRANCE_SIGN_LONG, _sign_text());
  return entrance_sign;
}

// Take the sign down again, when the entrance leaves its place.
void take_down_job_entrance_sign()
{
  if (entrance_sign)
    entrance_sign->dest_me();
  entrance_sign = nil;
}

// Whether somebody belongs to the people this entrance enrols: by the
// citizenship they hold (or an ally of it) or by their race.
private int admits(object who)
{
  object diplomacy, city;
  string held, root, race;
  string * races;
  int i;

  if (!sizeof(entrance_citizenships))
    return 1;

  diplomacy = handler("diplomacy", who);
  if (!diplomacy)
    return 1;

  held = "";
  if (who->query_city_ob())
  {
    city = load_object(who->query_city_ob());
    if (city)
      held = (string)city->query_name();
  }
  race = who->query_race_ob();

  for (i = 0; i < sizeof(entrance_citizenships); i++)
  {
    if (strlen(held) &&
        diplomacy->is_ally(held, entrance_citizenships[i]))
      return 1;

    // the people of the node: its own races, or those of the citizenship at
    // the top of its chain when no node in the graph declares any
    races = (string *)diplomacy->query_people_races(entrance_citizenships[i]);
    if (!sizeof(races))
    {
      root = (string)diplomacy->query_root_citizenship(entrance_citizenships[i]);
      if (!strlen(root))
        root = entrance_citizenships[i];
      city = load_object("/games/" + game_name(who) + "/obj/citizenships/" +
                         root + ".c");
      races = city ? (string *)city->query_races() : ({ });
    }

    if (race && member_array(race, races) != -1)
      return 1;
  }

  return 0;
}

void init()
{
  add_action("do_join_job", _LANG_JOB_ENTRANCE_CMDS_JOIN);
  add_action("do_leave_job", _LANG_JOB_ENTRANCE_CMDS_LEAVE);
}

int do_join_job(string str)
{
  object me, job;

  me = this_player();
  job = _job();
  if (!me || !job)
  {
    notify_fail(_LANG_JOB_ENTRANCE_NO_JOB);
    return 0;
  }

  if (me->query_job_ob() == entrance_job)
  {
    notify_fail(_LANG_JOB_ENTRANCE_ALREADY_MEMBER);
    return 0;
  }

  if (me->query_job_ob())
  {
    notify_fail(_LANG_JOB_ENTRANCE_OTHER_JOB);
    return 0;
  }

  if (!job->query_legal_player(me))
  {
    notify_fail(_LANG_JOB_ENTRANCE_NOT_LEGAL);
    return 0;
  }

  if (!admits(me))
  {
    notify_fail(_LANG_JOB_ENTRANCE_NOT_ADMITTED);
    return 0;
  }

  me->set_job_ob(entrance_job);
  if (me->query_job_ob() != entrance_job)
  {
    notify_fail(_LANG_JOB_ENTRANCE_NOT_LEGAL);
    return 0;
  }

  log_file("jobs", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " joins " + entrance_job + "\n");

  tell_object(me, _LANG_JOB_ENTRANCE_JOINED);
  tell_room(environment(me), _LANG_JOB_ENTRANCE_JOINED_OTHERS, ({ me }));
  me->save();
  return 1;
}

int do_leave_job(string str)
{
  object me, job;

  me = this_player();
  if (!me || !me->query_job_ob())
  {
    notify_fail(_LANG_JOB_ENTRANCE_NO_JOB_TO_LEAVE);
    return 0;
  }

  job = load_object(me->query_job_ob());
  // a job that does not load is not left either, in case it is broken
  if (!job || !job->can_abandon(me))
  {
    notify_fail(_LANG_JOB_ENTRANCE_CANNOT_LEAVE);
    return 0;
  }

  tell_object(me, _LANG_JOB_ENTRANCE_LEAVE_CONFIRM);
  input_to("confirm_leave_job", 0, me);
  return 1;
}

void confirm_leave_job(string answer, object me)
{
  object job;
  string path;

  if (!me)
    return;

  answer = answer ? lower_case(trim(answer)) : "";
  if (member_array(answer, _LANG_JOB_ENTRANCE_YES) == -1)
  {
    tell_object(me, _LANG_JOB_ENTRANCE_LEAVE_CANCELLED);
    return;
  }

  path = me->query_job_ob();
  job = path ? load_object(path) : nil;
  if (job && !job->on_leave(me))
  {
    tell_object(me, _LANG_JOB_ENTRANCE_CANNOT_LEAVE);
    return;
  }

  log_file("jobs", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " leaves " + path + " (" + me->query_job_xp() + " job xp lost)\n");

  // the level is kept in the record of jobs held; the experience is not
  me->set_job_ob("");
  me->set_job_xp(0);
  tell_object(me, _LANG_JOB_ENTRANCE_LEFT);
  me->save();
}

mixed * stats()
{
  return ({ ({ "Entrance Job", entrance_job }),
            ({ "Entrance Citizenships", implode(entrance_citizenships, ", ") }) });
}
