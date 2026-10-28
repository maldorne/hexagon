// escort.c -- a guard taking a beaten criminal to prison, as an NPC component.
//
// The crimes handler gives it to a free guard (see capture in
// lib/handlers/crimes.md) and calls escort(). The guard walks to the place
// where the criminal lies, announces the arrest, and then walks to a cell of
// the jurisdiction, pulling the criminal along: on every look the criminal is
// moved to where the guard stands. When both are in the cell the handler jails
// the criminal; the guard then goes back to its schedule and the component
// removes itself.
//
// The criminal is kept unconscious by the handler (hold_captive) on every look.
// If anything breaks the escort -- the criminal dies or is gone, there is no
// route, the time is up -- the handler is told to let the criminal go and the
// guard goes back to work.
//
// Nothing here is persisted: a trip is never saved. An escort restored from an
// npc.o has no task and removes itself.

#include <areas/area.h>
#include <language.h>

inherit component "/lib/npc/component.c";

// seconds between two looks
#define ESCORT_TICK 2

static object captive;       // the criminal
static string winner_name;   // the player who beat it
static string citizenship;   // whose cells receive it
static string origin;        // where the guard was sent from
static string cell;          // the cell being walked to, "" before the arrest
static int deadline;         // when the escort gives up
static int busy;             // a task is under way

void create()
{
  component::create();
  set_type("escort");
  captive = nil;
  winner_name = nil;
  citizenship = nil;
  origin = nil;
  cell = "";
  deadline = 0;
  busy = 0;
  call_out("check_task", 2);
}

/**
 * An escort that is nobody's errand (restored from a save) goes away.
 */
void check_task()
{
  object me;

  if (busy)
    return;

  me = query_entity();
  if (me)
    me->remove_component("escort");
}

/**
 * Back to work: the schedule's entry for this hour, or else where the guard
 * was sent from. Then the component goes.
 */
private void stand_down()
{
  object me, area, sched;

  me = query_entity();
  captive = nil;
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
  me->remove_component("escort");
}

/**
 * The arrest is given up: the criminal is let go.
 */
private void fail()
{
  object criminal;

  criminal = captive;
  if (criminal)
    handler("crimes", query_entity())->release_captive(criminal);

  stand_down();
}

/**
 * Starts the errand: go to the criminal and take them to a cell.
 *
 * @param criminal who to take
 * @param winner the name of the player who beat them
 * @param citizenship_name the citizenship whose cells receive them
 * @param seconds how long before the guard gives up
 */
void escort(object criminal, string winner, string citizenship_name,
            int seconds)
{
  object me;

  me = query_entity();
  if (!me || !criminal || !environment(me))
  {
    if (me)
      me->remove_component("escort");
    return;
  }

  captive = criminal;
  winner_name = winner;
  citizenship = citizenship_name;
  origin = (string)environment(me)->query_file_name();
  cell = "";
  deadline = time() + seconds;
  busy = 1;

  // the first move is made now, the rest by tick
  call_out("tick", 0);
}

/**
 * Walks to the first cell of the jurisdiction the guard has a route to.
 * Returns 0 when there is none.
 */
private int choose_cell(object me)
{
  string * cells;
  int i;

  cells = (string *)handler("crimes", me)->query_cells(citizenship);
  for (i = 0; i < sizeof(cells); i++)
    if (me->travel_to(cells[i]))
    {
      cell = cells[i];
      return 1;
    }

  return 0;
}

/**
 * The arrest is announced where the criminal lies, and the criminal is in
 * custody from then on.
 */
private void announce(object me, object place)
{
  object winner;

  handler("crimes", me)->take_custody(me, captive);

  tell_room(place, replace_string(replace_string(_LANG_ESCORT_ARRESTS,
              "$guard$", capitalize((string)me->query_cap_name())),
              "$who$", (string)captive->query_cap_name()), ({ captive }));

  // the player who won hears of it even when they have left
  winner = find_player(winner_name);
  if (winner && environment(winner) != place)
    tell_object(winner, replace_string(_LANG_ESCORT_ARRESTS_AWAY, "$who$",
                                       (string)captive->query_cap_name()));
}

/**
 * The criminal is dragged to where the guard stands.
 */
private int drag(object me, object from, object to)
{
  string guard_name, who;

  guard_name = capitalize((string)me->query_cap_name());
  who = (string)captive->query_cap_name();

  tell_room(from, replace_string(replace_string(_LANG_ESCORT_DRAG_FROM,
              "$guard$", guard_name), "$who$", who), ({ captive, me }));
  if (!captive->move_living("X", to, "none"))
    return 0;

  tell_room(to, replace_string(replace_string(_LANG_ESCORT_DRAG_TO,
              "$guard$", guard_name), "$who$", who), ({ captive, me }));
  return 1;
}

/**
 * One look: reach the criminal, announce the arrest, then walk to the cell
 * pulling the criminal along.
 */
void tick()
{
  object me, here, there;

  me = query_entity();
  if (!me)
    return;

  if (!captive || captive->query_dead() || me->query_dead() ||
      time() >= deadline)
  {
    fail();
    return;
  }

  here = environment(me);
  there = environment(captive);
  if (!here || !there)
  {
    fail();
    return;
  }

  handler("crimes", me)->hold_captive(captive);

  // a fight comes first
  if (sizeof(me->query_attacker_list()))
  {
    call_out("tick", ESCORT_TICK);
    return;
  }

  if (!strlen(cell))
  {
    if (here == there)
    {
      announce(me, here);
      if (!choose_cell(me))
      {
        fail();
        return;
      }
    }
    else if (!me->query_action_pending() &&
             (!me->travelling() || me->query_travel_goal() != there) &&
             !me->travel_to(there))
    {
      fail();
      return;
    }

    call_out("tick", ESCORT_TICK);
    return;
  }

  if (here != there && !drag(me, there, here))
  {
    fail();
    return;
  }

  if (!me->travelling() && !me->query_action_pending())
  {
    if ((int)handler("crimes", me)->deliver_prisoner(me, captive, cell))
    {
      stand_down();
      return;
    }

    // not there yet: a step was lost on the way
    if (!me->travel_to(cell))
    {
      fail();
      return;
    }
  }

  call_out("tick", ESCORT_TICK);
}

string query_info()
{
  return busy ? "taking " + (captive ? captive->query_name() : "?") : "idle";
}
