// responder.c -- a guard answering a crime away from its post, as an NPC
// component.
//
// The crimes handler attaches it, on demand, to the guards it sends when a
// crime is committed where no guard stands (see lib/handlers/crimes.md). The
// guard walks to the criminal, attacks them if they are still unhidden there
// and, if they have gone, follows them for as long as the pursuit lasts (inside
// its community only). When the criminal is dead or gone, or the time is up,
// it walks back to its work and the component removes itself, so nothing is
// kept on a guard that is idle.
//
// Nothing here is persisted: a trip is never saved. A responder restored from
// an npc.o has no task and removes itself.

#include <areas/area.h>

inherit component "/lib/npc/component.c";

// seconds between two looks at where the criminal is
#define RESPONDER_TICK 2

static object target;      // the criminal
static string origin;      // where the guard was sent from
static int deadline;       // when the pursuit gives up
static int busy;           // a task is under way

void create()
{
  component::create();
  set_type("responder");
  target = nil;
  origin = nil;
  deadline = 0;
  busy = 0;
  call_out("check_task", 2);
}

// A responder that is nobody's errand (restored from a save) goes away.
static void check_task()
{
  object me;

  if (busy)
    return;

  me = query_entity();
  if (me)
    me->remove_component("responder");
}

// Whether two places belong to the same community: the guard never follows
// out of it.
private int _same_community(object a, object b)
{
  object area_a, area_b;

  if (!a || !b || !a->query_location() || !b->query_location())
    return 0;

  area_a = (object)a->query_area();
  area_b = (object)b->query_area();
  if (!area_a || !area_b)
    return 0;

  return (object)area_a->query_root_area() == (object)area_b->query_root_area();
}

// Back to work: the schedule's entry for this hour, or else where the guard
// was sent from. Then the component goes.
private void _stand_down()
{
  object me, area;
  object sched;

  me = query_entity();
  target = nil;
  busy = 0;
  if (!me)
    return;

  if (!me->query_dead() && environment(me))
  {
    sched = me->query_component_by_type("schedule");
    area = (object)load_object(AREA_HANDLER)->query_area(
             me->query_npc_area_path());
    if (sched && area)
      me->resume_schedule((int)area->query_game_hour());

    if (!me->travelling() && origin &&
        (string)environment(me)->query_file_name() != origin)
      me->travel_to(origin);
  }

  // the component goes with this call: nothing after it
  me->remove_component("responder");
}

/**
 * Starts the errand: go to the criminal and keep after them for at most
 * `seconds`.
 *
 * @param criminal who to arrest
 * @param place where the crime happened (the first destination)
 * @param seconds how long before the guard gives up
 */
void respond(object criminal, object place, int seconds)
{
  object me;

  me = query_entity();
  if (!me || !criminal || !environment(me))
  {
    if (me)
      me->remove_component("responder");
    return;
  }

  target = criminal;
  origin = (string)environment(me)->query_file_name();
  deadline = time() + seconds;
  busy = 1;

  // the first move is made now, the rest by tick
  call_out("tick", 0);
}

// One look: attack if the criminal is here, otherwise go to where they are.
static void tick()
{
  object me, here, there;

  me = query_entity();
  if (!me)
    return;

  if (!target || target->query_dead() || time() >= deadline)
  {
    _stand_down();
    return;
  }

  here = environment(me);
  there = environment(target);
  if (!here || !there)
  {
    _stand_down();
    return;
  }

  if (here == there)
  {
    if (!target->query_hidden() &&
        member_array(target, me->query_attacker_list()) == -1)
      me->attack_ob(target);
  }
  else if (!sizeof(me->query_attacker_list()) && !me->query_action_pending() &&
           (!me->travelling() || me->query_travel_goal() != there))
  {
    // out of the community, or no way to reach them: it is over
    if (!_same_community(here, there) || !me->travel_to(there))
    {
      _stand_down();
      return;
    }
  }

  call_out("tick", RESPONDER_TICK);
}

string query_info()
{
  return busy ? "after " + (target ? target->query_name() : "?") : "idle";
}
