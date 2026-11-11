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
// save files (saved with the player) and the mounts that are out in the world
// now (not saved: they are made again from their files at login).

static object mount;  // what this living rides
static int seat;      // the seat it sits in; 0 drives

string * mount_ids;           // the save files of the mounts it owns
static object * owned_mounts; // the owned mounts that are out in the world

void create()
{
  mount = nil;
  seat = 0;
  mount_ids = ({ });
  owned_mounts = ({ });
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
 * The owned mounts that are out in the world now.
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
    ({ "Owned mounts (nosave)", owned_mounts, }),
    });
}
