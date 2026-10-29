// strategy.c -- base of every strategy: the algorithm of one boss, a state
// machine. A strategy object is a clone, one per NPC, because it keeps state.
// It is held by the strategy component (/lib/npc/components/strategy.c), which
// feeds it the NPC's events and a tick of its own.
//
// States: set_state("x") runs leave_<old>(new) then enter_<new>(old). While in
// "x" the tick calls tick_x(), which returns the seconds until the next tick
// (0 = sleep until an event wakes it with holder->start_tick()). All these
// functions are public: they are reached with call_other.
//
// A strategy owns functions (query_owned), and the lower levels are off for
// them: "wander", "follow" (monster.c), "flee" (the flee vote), "choice" and
// "target" (choose_effect, set_concentrate), "hiding" (the hidden component is
// dropped). A strategy never tries what its NPC cannot do now: it picks only
// from the usable effects and checks can_use_effect before using one directly
// (use_skill).

#include <npc/npc.h>
#include <living/skills.h>

#define HIDE_SHADOW "/lib/obj/skills/hide_sh.c"

static object npc;      // the NPC it plays
static object holder;   // the component holding it
static string state;
static int state_since;

void create()
{
  state = nil;
  state_since = 0;
}

/**
 * Binds the strategy to its NPC and to the component that holds it.
 */
void bind(object ob, object comp)
{
  npc = ob;
  holder = comp;
}

object query_npc() { return npc; }
string query_state() { return state ? state : ""; }
int query_state_age() { return time() - state_since; }

/**
 * The name this strategy is known by: its file name without the path.
 */
string query_id()
{
  string * parts;

  parts = explode(base_name(this_object()), "/");
  return parts[sizeof(parts) - 1];
}

/**
 * The functions this strategy owns; a strategy overrides this.
 */
string * query_owned() { return ({ }); }
int query_owns(string what) { return member_array(what, query_owned()) != -1; }

/**
 * Leaves the current state and enters another: leave_<old>(new), then
 * enter_<new>(old), then the tick looks at the new state at once.
 */
void set_state(string next)
{
  string old;

  old = state;
  if (old && function_exists("leave_" + old, this_object()))
    call_other(this_object(), "leave_" + old, next);
  state = next;
  state_since = time();
  if (function_exists("enter_" + next, this_object()))
    call_other(this_object(), "enter_" + next, old);
  holder->start_tick(1);
}

/**
 * Runs the tick of the current state.
 *
 * @return seconds until the next tick, 0 to stop ticking
 */
int tick()
{
  if (!state || !function_exists("tick_" + state, this_object()))
    return 0;
  return (int)call_other(this_object(), "tick_" + state);
}

// Events the component passes on; a strategy overrides what it needs.
void placed() { }
void event_enter(object ob, varargs string mess, object from, mixed avoid) { }
void event_exit(object ob, varargs string mess, object dest, mixed avoid) { }
void event_fight_in_progress(object ob, object * fighters) { }
void event_revealed(object finder) { }
void died(object killer) { }

// The chains. By default no opinion.
mixed * choose_effect(mixed * args) { return args; }
int flee_vote() { return FLEE_ABSTAIN; }

// ---- helpers -------------------------------------------------------------

/**
 * Those fighting the NPC in its own place.
 */
object * present_attackers()
{
  object * list;
  int i;

  list = ({ }) + npc->query_attacker_list();
  for (i = sizeof(list) - 1; i >= 0; i--)
    if (!list[i] || !living(list[i]) || list[i]->query_dead() ||
        environment(list[i]) != environment(npc))
      list = delete(list, i, 1);
  return list;
}

/**
 * The NPC's hit points as a percentage of its maximum.
 */
int hp_percent()
{
  if (npc->query_max_hp() < 1)
    return 0;
  return npc->query_hp() * 100 / npc->query_max_hp();
}

/**
 * Tells the room the NPC stands in, the NPC aside.
 */
void tell_here(string msg)
{
  if (environment(npc))
    tell_room(environment(npc), msg, ({ npc }));
}

/**
 * Hides the NPC with the shadow the hide skill gives a player.
 */
void hide_npc()
{
  if (!npc->query_hide_shadow())
    clone_object(HIDE_SHADOW)->setup_shadow(npc);
}

/**
 * Brings the NPC out of hiding.
 */
void unhide_npc()
{
  if (npc->query_hide_shadow())
    npc->dest_hide_shadow();
}

/**
 * The skill object of an id, or nil when the skills table does not know it.
 */
object skill_object(string id)
{
  mixed data;

  data = table("skills", npc)->query_skill_data(id);
  return pointerp(data) ? load_object(data[SKILL_DATA_PATH]) : nil;
}

/**
 * Starts a skill of the NPC on a target the way a player's command would, but
 * only when the NPC can use it now (gp, lockout, guild). Returns 0 when it
 * cannot or the cast was refused, and the caller falls back to another action.
 */
int use_skill(string id, object target)
{
  object skill;
  int ok;

  skill = skill_object(id);
  if (!skill || !target || !npc->can_use_effect(skill))
    return 0;
  catch(ok = skill->cast_effect_on(({ target }), npc));
  return ok;
}

/**
 * Whether the NPC stands in its home place.
 */
int at_home()
{
  return environment(npc) &&
         holder->query_place(environment(npc)) == holder->query_home();
}
