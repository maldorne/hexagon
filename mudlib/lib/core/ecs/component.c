// component.c -- what every component is, whatever entity it belongs to.
//
// A component is a small object carrying one slice of an entity's behaviour
// (a schedule, a shop, a patrol, the props of a place). It is cloned once per
// entity, never stands anywhere on its own, and hears of the world only through
// its entity (see /lib/core/ecs/entity.c). This file inherits nothing: each
// kind of entity has its own base, which inherits this plus the object or
// container half it needs, and redefines create, stats and the auto load pair
// to call both.
//
// PERSISTENCE: the entity is the courier, the component the owner of its state.
// The entity's `component_info` (type -> attrs) travels in the entity's own
// savefile; the live instance is static and re-cloned on restore. Declare every
// persisted field in query_auto_load_attributes and restore it in
// init_auto_load_attributes, pairing them exactly, and namespace the keys
// (shop_goods, schedule_entries, ...) so composed mixins never collide. The
// entity refreshes the snapshot on every save, so a component never saves
// itself. State that does not fit a flat mapping lives in side files whose
// paths derive from the entity, never from a setter.
//
// LIFECYCLE, in order:
//   create()                       the clone exists
//   set_type(type)                 stamped by the entity, the key it is kept by
//   init_auto_load_attributes(m)   its persisted state
//   initialize(entity)             bound to its entity
//   components_ready()             every component of the entity exists
//   placed()                       the entity is standing in the world
//   dest_me()                      the entity is going, or dropped it

#include <ecs/ecs.h>

private string type;   // component type: the blueprint's file name
static object entity;  // the entity this component belongs to

void create()
{
  type = "base";
  entity = nil;
}

int is_component() { return 1; }

/**
 * Binds the component to its entity, after creation and
 * init_auto_load_attributes. Override to wire up whatever needs the entity,
 * undoing it in dest_me if the component can be removed.
 */
void initialize(object ob)
{
  entity = ob;
}

object query_entity() { return entity; }

string query_type() { return type; }
void set_type(string t) { type = t; }

/**
 * Called once every component of the entity exists, for one that depends on
 * another. Empty by default.
 */
void components_ready() { }

/**
 * Called once the entity is standing somewhere, which initialize is too early
 * for. Empty by default.
 */
void placed() { }

/**
 * A one-line summary of the component's essential state, for programmers (the
 * `components` command shows it next to the type). Empty by default.
 */
string query_info() { return ""; }

/**
 * The verbs this component wants a player to use on its entity, as
 * ([ verb : function here ]). A component stands nowhere, so it cannot carry a
 * verb itself: the entity registers them and forwards the call back.
 */
mapping query_component_actions() { return ([ ]); }

/**
 * The entity functions this component takes part in, as
 * ([ function : HOOK_PRIORITY_* ]); each is answered by hook_<function>(args).
 * The contract of each hook (transform the args, or return a piece) is the
 * entity's, documented where it runs the chain. For example:
 *   mapping query_hooks() {
 *     return ([ "do_exit_command": HOOK_PRIORITY_TRANSFORM ]);
 *   }
 * then hook_do_exit_command(mixed * args) does the work.
 */
mapping query_hooks() { return ([ ]); }

/**
 * The one call site the entity uses to reach hook_<func> on any component.
 */
mixed call_hook(string func, mixed * args)
{
  return call_other(this_object(), "hook_" + func, args);
}

/**
 * Persisted state. The base only carries its type; a component extends both
 * halves in lockstep.
 */
mapping query_auto_load_attributes()
{
  return ([ "type" : type ]);
}

void init_auto_load_attributes(mapping args)
{
  if (args && !undefinedp(args["type"]))
    type = args["type"];
}

mixed * stats()
{
  return ({ ({ "Component Type", type }) });
}
