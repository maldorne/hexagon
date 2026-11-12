// Variables necesarias para tener una montura
// neverbot 7/2005
// 11/2026 - Rewritten as the rider's half of the riding system; the mounts a
//           player owns are kept here as a list of save-file ids.

// riding.c -- the rider's half of mounts and vehicles.
//
// A living that sits on a mount remembers which one and in which seat; none of
// it is saved (a ride does not outlast a login). The rules, the texts and the
// moving belong to the riding handler (/lib/handlers/riding.c): this file only
// holds the state and answers the questions the rest of the mudlib already asks
// of a rider (query_riding, query_mount). The mount is whatever answers
// query_rideable(): an NPC with the rideable component, or an item built on
// /lib/obj/vehicle.c.
//
// It also holds what a player keeps of the mounts it owns: the ids of their
// save files and the id of the one that is out (both saved with the player), and
// the mounts that are in the world now (not saved: the one out is made again
// from its file at login).
//
// And the state of travelling on its own (travel <direction>): a chain of
// call_outs, one step each, whose steps are chosen by the riding handler.

#include <living/riding.h>

static object mount;  // what this living rides
static int seat;      // the seat it sits in; 0 drives

string * mount_ids;           // the save files of the mounts it owns
string mount_out;             // the id of the one that is out; empty when none
static object * owned_mounts; // the owned mounts that are in the world

static int ride_travel_handle;     // call_out of the next step; 0 when not travelling
static string ride_travel_dir;     // the direction it is following
static object ride_travel_place;   // where it stood when the last step was sent
static int ride_travel_count;      // steps taken since it started

void create()
{
  mount = nil;
  seat = 0;
  mount_ids = ({ });
  mount_out = "";
  owned_mounts = ({ });
  ride_travel_handle = 0;
}

/**
 * The ids of the mounts this living owns, each the name of a save file in its
 * mounts folder.
 */
string * query_mount_ids()
{
  if (!mount_ids)
    mount_ids = ({ });
  return mount_ids + ({ });
}

/**
 * Adds a mount id to the owned ones.
 */
void add_mount_id(string id)
{
  if (!mount_ids)
    mount_ids = ({ });
  if (member_array(id, mount_ids) == -1)
    mount_ids += ({ id });
}

/**
 * Takes a mount id out of the owned ones.
 */
void remove_mount_id(string id)
{
  if (mount_ids)
    mount_ids -= ({ id });
}

/**
 * The id of the owned mount that is out, or an empty string when all are stored.
 */
string query_mount_out()
{
  return stringp(mount_out) ? mount_out : "";
}

/**
 * Sets which owned mount is out; an empty string when none is. The riding
 * handler calls it.
 */
void set_mount_out(string id)
{
  mount_out = id ? id : "";
}

/**
 * The owned mounts that are in the world now.
 */
object * query_owned_mounts()
{
  if (!owned_mounts)
    owned_mounts = ({ });
  owned_mounts -= ({ nil });
  return owned_mounts + ({ });
}

/**
 * Notes an owned mount that is out; the riding handler calls it when it makes
 * one.
 */
void add_owned_mount(object ob)
{
  if (!owned_mounts)
    owned_mounts = ({ });
  if (ob && member_array(ob, owned_mounts) == -1)
    owned_mounts += ({ ob });
}

/**
 * Whether this living is riding something.
 */
int query_riding() { return mount != nil; }

/**
 * What this living rides, or nil. A mount that was destructed is nobody's.
 */
object query_mount() { return mount; }

/**
 * The seat this living sits in: 0 is the driver's.
 */
int query_seat() { return mount ? seat : 0; }

/**
 * Sits this living on a mount. Whoever calls it is the riding handler; to
 * board a mount, ask the handler (board).
 */
void set_mount(object ob, int which)
{
  mount = ob;
  seat = which;
}

/**
 * Takes this living off its mount, with no words and no checks; the handler
 * (dismount) is the one that tells.
 */
void clear_mount()
{
  mount = nil;
  seat = 0;
}

/**
 * Whether this rider is travelling on its own.
 */
int query_ride_traveling() { return ride_travel_handle != 0; }

/**
 * Stops the travelling on its own, telling the rider unless `quiet`.
 */
void stop_ride_travel(varargs int quiet)
{
  if (!ride_travel_handle)
    return;

  remove_call_out(ride_travel_handle);
  ride_travel_handle = 0;
  ride_travel_dir = nil;
  ride_travel_place = nil;

  if (!quiet && mount)
    handler("riding", this_object())->tell_travel(this_object(), "travel_stop");
}

/**
 * Sends the next step: the call_out the chain is made of. It stops when the
 * rider is no longer fit to drive, when the last step did not move it, or when
 * the handler finds no way on; otherwise it queues the exit and arms the next.
 */
void ride_travel_step()
{
  object riding, env;
  string dir;

  ride_travel_handle = 0;
  riding = handler("riding", this_object());
  env = environment(this_object());

  if (!mount || !env || (int)this_object()->query_dead() ||
      (int)this_object()->query_seat() != RIDING_DRIVER_SEAT)
  {
    ride_travel_dir = nil;
    return;
  }

  if (this_object()->query_fighting())
  {
    ride_travel_dir = nil;
    riding->tell_travel(this_object(), "travel_fight");
    return;
  }

  // the last step went nowhere: a closed door, a refusal
  if (env == ride_travel_place)
  {
    ride_travel_dir = nil;
    riding->tell_travel(this_object(), "travel_blocked");
    return;
  }

  dir = riding->query_travel_exit(this_object(), env, ride_travel_dir);
  if (!dir)
  {
    ride_travel_dir = nil;
    riding->tell_travel(this_object(), "travel_lost");
    return;
  }

  if (dir != ride_travel_dir)
  {
    ride_travel_dir = dir;
    if (ride_travel_count)
      riding->tell_travel(this_object(), "travel_follow", dir);
  }

  ride_travel_count++;
  ride_travel_place = env;

  // a step is skipped now and then, so the pace is not mechanical
  if (random(RIDING_TRAVEL_PAUSE))
    this_object()->queue_action(dir);
  else
    ride_travel_place = nil;

  ride_travel_handle = call_out("ride_travel_step",
                                riding->query_travel_delay(this_object()));
}

/**
 * Starts travelling in a direction: one step at a time along the way, the
 * handler picking the exit at each place (see ride_travel_step). A way that
 * does not exist here is not started.
 * @return 0 when `dir` leads nowhere here, 1 when the command is dealt with
 */
int start_ride_travel(string dir)
{
  object riding;
  string way;

  if (!mount)
    return 0;

  riding = handler("riding", this_object());
  if ((int)this_object()->query_seat() != RIDING_DRIVER_SEAT)
  {
    riding->tell_travel(this_object(), "refuse_passenger");
    return 1;
  }

  way = riding->query_travel_start(this_object(), dir);
  if (!way)
    return 0;

  // the way exists but the mount cannot take it: the handler said why
  if (!strlen(way))
    return 1;

  stop_ride_travel(1);
  ride_travel_dir = way;
  ride_travel_place = nil;
  ride_travel_count = 0;

  riding->tell_travel(this_object(), "travel_start", way);
  ride_travel_handle = call_out("ride_travel_step", 1);
  return 1;
}

/**
 * What `look` adds to a rider: where they sit. The rider is registered as its
 * own extra look when it boards.
 */
string extra_look()
{
  if (!mount)
    return "";

  return (string)handler("riding", this_object())->query_look(this_object());
}

/**
 * The words that follow a rider's name in a room listing.
 */
string query_riding_listing()
{
  if (!mount)
    return "";

  return (string)handler("riding", this_object())->query_listing(this_object());
}

mixed * stats()
{
  return ({
    ({ "Mount (nosave)", mount, }),
    ({ "Seat (nosave)", seat, }),
    ({ "Mount ids", mount_ids, }),
    ({ "Mount out", mount_out, }),
    ({ "Travelling (nosave)", ride_travel_handle ? ride_travel_dir : "no", }),
    ({ "Owned mounts (nosave)", owned_mounts, }),
    });
}
