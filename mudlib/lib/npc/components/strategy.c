// strategy.c -- one algorithm for one NPC, as an NPC component (bosses).
//
// It keeps a clone of the strategy named by the template, loaded from
// /games/<game>/strategies/<id>.c or, failing that, /lib/npc/strategies/<id>.c.
// It joins the NPC's choose_effect and flee chains, passes on its events, and
// owns the call_out tick the strategy runs on (only while the strategy asks
// for it). The strategy declares the functions it owns (query_owned) and the
// NPC switches the lower levels off for them (query_strategy_owns in
// /lib/npc.c). Only the id and the home place are persisted: the state of a
// fight is not, a restored NPC starts again from placed().
//
// Attributes: "id", the strategy; "home", the location file it returns to (set
// at the first placement when absent).
//
//   "strategy" : ([ "id" : "<strategy id>" ])

#include <npc/npc.h>
#include <ecs/ecs.h>

inherit component "/lib/npc/component.c";

// default seconds between ticks
#define STRATEGY_TICK 2

private string strategy_id;
private string strategy_home;
static object strategy;
static int tick_handle;

void create()
{
  component::create();
  strategy_id = nil;
  strategy_home = nil;
  strategy = nil;
  tick_handle = 0;
}

string query_info()
{
  return strategy ? strategy_id + ":" + strategy->query_state() : "none";
}

object query_strategy() { return strategy; }
string query_home() { return strategy_home; }
int query_owns(string what) { return strategy ? (int)strategy->query_owns(what) : 0; }

/**
 * The name a place is known by: the file a location is loaded from.
 */
string query_place(object env)
{
  if (!env)
    return nil;
  return function_exists("query_file_name", env) ? env->query_file_name()
                                                 : file_name(env);
}

private string strategy_path(string id)
{
  object npc;
  string game, path;

  npc = query_entity();
  game = npc ? game_name(npc) : "";
  if (game && strlen(game))
  {
    path = "/games/" + game + "/strategies/" + id + ".c";
    if (file_size(path) > 0)
      return path;
  }

  path = "/lib/npc/strategies/" + id + ".c";
  return file_size(path) > 0 ? path : nil;
}

private void unload_strategy()
{
  if (strategy)
    destruct_object(strategy);
  strategy = nil;
}

private void load_strategy()
{
  string path;

  if (!query_entity() || !strategy_id)
    return;
  if (strategy && strategy->query_id() == strategy_id)
    return;

  unload_strategy();
  path = strategy_path(strategy_id);
  if (!path)
  {
    stderr("strategy: no file for '" + strategy_id + "'\n");
    return;
  }

  catch(strategy = clone_object(path));
  if (strategy)
    strategy->bind(query_entity(), this_object());
}

void initialize(object ob)
{
  component::initialize(ob);
  load_strategy();
}

mapping query_auto_load_attributes()
{
  mapping m;

  m = component::query_auto_load_attributes();
  if (strategy_id)
    m["id"] = strategy_id;
  if (strategy_home)
    m["home"] = strategy_home;
  return m;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (stringp(args["id"]))
    strategy_id = args["id"];
  if (stringp(args["home"]))
    strategy_home = args["home"];
  // a reseed of a live component may change the strategy
  load_strategy();
}

// ---- the tick -------------------------------------------------------------

/**
 * Asks for a tick, unless one is already waiting. Public: the strategy calls
 * it, and the tick itself is reached by call_out.
 */
void start_tick(varargs int delay)
{
  if (!tick_handle)
    tick_handle = call_out("tick", delay > 0 ? delay : STRATEGY_TICK);
}

void stop_tick()
{
  if (tick_handle)
    remove_call_out(tick_handle);
  tick_handle = 0;
}

void tick()
{
  object npc;
  int next;

  tick_handle = 0;
  npc = query_entity();
  if (!strategy || !npc || npc->query_dead() || !environment(npc))
    return;

  next = strategy->tick();
  // the strategy may have asked for a tick itself while it ran
  if (next > 0 && !tick_handle)
    tick_handle = call_out("tick", next);
}

// ---- hooks and events -----------------------------------------------------

mapping query_hooks()
{
  // the strategy decides last in choose_effect, so it can narrow what the
  // other components left
  return ([ "choose_effect": HOOK_PRIORITY_TRACE, "flee": HOOK_PRIORITY_GATE ]);
}

mixed * hook_choose_effect(mixed * args)
{
  return strategy ? strategy->choose_effect(args) : args;
}

int hook_flee(mixed * args)
{
  return strategy ? (int)strategy->flee_vote() : FLEE_ABSTAIN;
}

void placed()
{
  object npc;

  npc = query_entity();
  load_strategy();
  if (!strategy || !npc)
    return;

  if (!strategy_home && environment(npc))
    strategy_home = query_place(environment(npc));

  // hiding is owned: a hidden component would hide it behind its back
  if (strategy->query_owns("hiding") && npc->has_component("hidden"))
    npc->remove_component("hidden");

  strategy->placed();
  start_tick(1);
}

void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (strategy)
    strategy->event_enter(ob, mess, from, avoid);
}

void event_exit(object ob, varargs string mess, object dest, mixed avoid)
{
  if (strategy)
    strategy->event_exit(ob, mess, dest, avoid);
}

void event_fight_in_progress(object ob, object * fighters)
{
  if (strategy)
    strategy->event_fight_in_progress(ob, fighters);
}

void event_revealed(object finder)
{
  if (strategy)
    strategy->event_revealed(finder);
}

void died(object killer)
{
  if (strategy)
    strategy->died(killer);
  stop_tick();
}

void dest_me()
{
  stop_tick();
  unload_strategy();
  component::dest_me();
}
