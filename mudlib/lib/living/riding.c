// Variables necesarias para tener una montura
// neverbot 7/2005
// 11/2026 - Rewritten as the rider's half of the riding system (the mount a
//           player owned is not kept here any more).

// riding.c -- the rider's half of mounts and vehicles.
//
// A living that sits on a steed remembers which one and in which seat; nothing
// else is kept, and none of it is saved (a ride does not outlast a login). The
// rules, the texts and the moving belong to the riding handler
// (/lib/handlers/riding.c): this file only holds the state and answers the
// questions the rest of the mudlib already asks of a rider (query_riding,
// query_mount). The steed is whatever answers query_rideable(): an NPC with the
// rideable component, or an item built on /lib/obj/vehicle.c.

static object steed;  // what this living rides
static int seat;      // the seat it sits in; 0 drives

void create()
{
  steed = nil;
  seat = 0;
}

/**
 * Whether this living is riding something.
 */
int query_riding() { return steed != nil; }

/**
 * What this living rides, or nil. A steed that was destructed is nobody's.
 */
object query_mount() { return steed; }

/**
 * The seat this living sits in: 0 is the driver's.
 */
int query_seat() { return steed ? seat : 0; }

/**
 * Sits this living on a steed. Whoever calls it is the riding handler; to
 * board a steed, ask the handler (board).
 */
void set_mount(object ob, int which)
{
  steed = ob;
  seat = which;
}

/**
 * Takes this living off its steed, with no words and no checks; the handler
 * (dismount) is the one that tells.
 */
void clear_mount()
{
  steed = nil;
  seat = 0;
}

/**
 * What `look` adds to a rider: where they sit. The rider is registered as its
 * own extra look when it boards.
 */
string extra_look()
{
  if (!steed)
    return "";

  return (string)handler("riding", this_object())->query_look(this_object());
}

/**
 * The words that follow a rider's name in a room listing.
 */
string query_riding_listing()
{
  if (!steed)
    return "";

  return (string)handler("riding", this_object())->query_listing(this_object());
}

mixed * stats()
{
  return ({
    ({ "Mount (nosave)", steed, }),
    ({ "Seat (nosave)", seat, }),
    });
}
