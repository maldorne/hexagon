// pacing.c — self-directed NPC movement: the cadence primitive plus directed
// travel, the two things that make an NPC walk on its own clock.
//
// Cadence is a single heart_beat countdown that releases one step every N beats,
// N = base + random(rand) so a crowd does not step in lockstep. It is pure beat
// counting (a decrement and a compare) -- nothing to do with the action-queue
// time budget (time_left / default_action_time), so slowing a mover here never
// touches a player and never defers a command into the heart_beat's tick budget.
//
// Two cadences coexist: a slow idle-wander interval and a brisk travel interval.
// move_ready() re-arms with whichever matches the current mode, so switching
// between wandering and travelling needs no save/restore -- the countdown simply
// re-arms from the right interval next time it fires.
//
// Directed travel lives here too: a route (a list of canonical directions from
// the fine pathfinder) walked one step per cadence tick. The route is transient
// (static: a trip is never saved) -- a restored NPC re-plans from its schedule,
// it does not resume a half-walked path. Only the countdown and the two
// intervals are ordinary vars.
//
// monster.c keeps the idle-wander behaviour (do_move_after, move_zones) and the
// per-beat dispatch that chooses travel over wander; it inherits this file.

#include <npc/npc.h>
#include <room/location.h>
#include <room/room.h>
#include <maps/maps.h>

// prototypes (same-file forward references)
int travelling();

int wander_base, wander_rand;   // idle-wander cadence ({ base, rand } in beats)
int travel_base, travel_rand;   // directed-travel cadence
int move_cooldown;              // beats left until the next step is allowed

static string *travel_path;     // remaining canonical directions to the goal
static object travel_goal;      // destination location

void create()
{
  wander_base = 0;
  wander_rand = 0;
  travel_base = TRAVEL_DEFAULT[0];
  travel_rand = TRAVEL_DEFAULT[1];
  move_cooldown = 0;
  travel_path = ({ });
  travel_goal = nil;
}

// The idle-wander cadence: a step every base + random(rand) beats. Setting it
// while not travelling re-arms the countdown so it takes effect from the next
// due step.
void set_wander_speed(int base, int rand)
{
  if (base < 0) base = 0;
  if (rand < 0) rand = 0;
  wander_base = base;
  wander_rand = rand;
  if (!travelling())
    move_cooldown = base + (rand ? random(rand) : 0);
}

// The directed-travel cadence (a step every base + random(rand) beats). A brisk
// messenger or a slow elder overrides the default.
void set_travel_speed(int base, int rand)
{
  if (base < 0) base = 0;
  if (rand < 0) rand = 0;
  travel_base = base;
  travel_rand = rand;
}

int * query_wander_speed() { return ({ wander_base, wander_rand }); }
int * query_travel_speed() { return ({ travel_base, travel_rand }); }

// One beat of the cadence. Returns 1 when a step is due -- re-arming with the
// interval of the current mode (travel while a route is pending, else wander) --
// otherwise counts down and returns 0. With the active interval unset (both 0)
// it returns 0, so a mover with no configured speed never moves on its own.
int move_ready()
{
  int base, rand;

  if (travelling())
  {
    base = travel_base;
    rand = travel_rand;
  }
  else
  {
    base = wander_base;
    rand = wander_rand;
  }

  if (!base && !rand)
    return 0;

  if (--move_cooldown > 0)
    return 0;

  move_cooldown = base + (rand ? random(rand) : 0);
  return 1;
}

// ---------------------------------------------------------------------------
// Directed travel
// ---------------------------------------------------------------------------

int travelling() { return travel_path && sizeof(travel_path) > 0; }
object query_travel_goal() { return travel_goal; }

// Ask the fine pathfinder for a route from here to a destination location and
// begin walking it, one step per travel-cadence tick. dest may be a location
// object or a location .o file name. Returns the number of steps, or 0 when
// there is no route (or no destination / not in a room). The route is stored
// canonical (language-neutral) and released a step at a time by travel_step.
int travel_to(mixed dest)
{
  object destination;
  string *dirs;

  destination = objectp(dest) ? dest :
                LOCATION_HANDLER->load_location(dest);
  if (!destination || !environment())
    return 0;

  dirs = PATHFIND_HANDLER->find_path(environment(), destination);
  if (!dirs)
    return 0;

  travel_path = dirs;
  travel_goal = destination;

  // arm the countdown at the travel interval so the first step is due promptly
  move_cooldown = travel_base + (travel_rand ? random(travel_rand) : 0);

  if (!this_object()->query_heart_beat())
    this_object()->set_heart_beat(1);

  return sizeof(dirs);
}

// Abandon the current route. Called on arrival, and the hook to call when the
// NPC is interrupted (combat, later conversation) so it stops mid-errand. The
// cadence needs no restore: move_ready re-arms from the wander interval as soon
// as travelling() is false.
void stop_travel()
{
  travel_path = ({ });
  travel_goal = nil;
}

// Release the next step of the route: issue one direction through the normal
// command path (so exits, blocks and messages all apply), then drop it. When the
// route is spent, end the trip. One call per travel-cadence tick.
void travel_step()
{
  string dir;

  if (!travelling())
    return;

  dir = travel_path[0];
  travel_path = travel_path[1..];

  // the pathfinder speaks canonical English ("east"); the movement command is
  // in the mud's current language ("este"), so localise before issuing it.
  this_object()->queue_action(ROOM_HAND->localize_dir(dir));

  if (!sizeof(travel_path))
    stop_travel();
}
