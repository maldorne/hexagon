// entity.c -- what an object needs to carry components.
//
// An entity (an NPC, a location, ...) inherits this and gets: attaching,
// reseeding and dropping components, finding them, saving and restoring them,
// telling them of events, and the hook chains through which they take part in
// the entity's own functions. Components are the other half
// (/lib/core/ecs/component.c).
//
// The entity says where its kind of component lives (query_component_dir) and
// may react when its set of components changes (components_changed): a location
// saves itself, an NPC does nothing. It calls entity::create() from its own
// create, sync_component_info() before saving, init_components() after
// restoring and destruct_components() when it goes.
//
// Two ways to reach the components:
//   - run_on_components(func, args...): a broadcast with the function's own
//     arguments, for events and orders. Side effects only.
//   - the hook chains: a component lists in query_hooks the entity functions it
//     takes part in, with a priority, and the entity runs the chain of one of
//     them with run_pipeline (each hook transforms the args) or run_reduce (each
//     hook returns a piece, folded with a combinator). Hooks take one array of
//     args, so a chain threads it unchanged.

#include <ecs/ecs.h>

// type -> persisted attrs, travelling in the entity's own savefile
mapping component_info;
// the live components, re-cloned on restore
static object * components;
// function -> the components hooking it, in priority order. Derived, never
// saved.
static mapping hook_chains;
// verb -> ({ component, function }), for the verbs components asked for. It
// holds live objects, which mean nothing across a save.
static mapping component_actions;

void create()
{
  component_info = ([ ]);
  components = ({ });
  hook_chains = ([ ]);
  component_actions = ([ ]);
}

/**
 * The directory the components of this kind of entity are cloned from, with a
 * trailing slash. Every entity defines it.
 */
string query_component_dir() { return nil; }

/**
 * Called whenever a component is attached, reseeded or dropped. Empty by
 * default; an entity that has to save itself then redefines it.
 */
void components_changed() { }

object * query_components() { return components ? components : ({ }); }

object query_component_by_type(string type)
{
  int i;

  for (i = 0; components && i < sizeof(components); i++)
    if (components[i] && components[i]->query_type() == type)
      return components[i];

  return nil;
}

int has_component(string type) { return query_component_by_type(type) != nil; }

// Walk the components, ask each which functions it hooks and at what priority,
// group by function, sort by priority (lower first). Ties keep load order.
private void rebuild_hook_chains()
{
  mapping by_func;  // func_name -> ({ ({ priority, component }), ... })
  string * funcs;
  int i, j, k;

  hook_chains = ([ ]);
  by_func = ([ ]);

  for (i = 0; components && i < sizeof(components); i++)
  {
    mapping hooks;
    string * hook_funcs;

    if (!components[i])
      continue;

    hooks = components[i]->query_hooks();
    if (!hooks)
      continue;

    hook_funcs = map_indices(hooks);
    for (j = 0; j < sizeof(hook_funcs); j++)
    {
      if (undefinedp(by_func[hook_funcs[j]]))
        by_func[hook_funcs[j]] = ({ });
      by_func[hook_funcs[j]] += ({ ({ hooks[hook_funcs[j]], components[i] }) });
    }
  }

  funcs = map_indices(by_func);
  for (i = 0; i < sizeof(funcs); i++)
  {
    mixed * pairs;
    object * chain;

    // small insertion sort by priority, stable
    pairs = by_func[funcs[i]];
    for (j = 1; j < sizeof(pairs); j++)
    {
      mixed * cur;
      cur = pairs[j];
      k = j - 1;
      while (k >= 0 && pairs[k][0] > cur[0])
      {
        pairs[k + 1] = pairs[k];
        k -= 1;
      }
      pairs[k + 1] = cur;
    }

    chain = ({ });
    for (j = 0; j < sizeof(pairs); j++)
      chain += ({ pairs[j][1] });
    hook_chains[funcs[i]] = chain;
  }
}

// A component stands nowhere, so it cannot carry a verb: the entity registers
// what it asked for and forwards the call back (_component_action).
private void _register_component_actions(object c)
{
  mapping wanted;
  string * verbs;
  int i;

  wanted = (mapping)c->query_component_actions();
  if (!mappingp(wanted) || !map_sizeof(wanted))
    return;

  if (!component_actions)
    component_actions = ([ ]);

  verbs = map_indices(wanted);
  for (i = 0; i < sizeof(verbs); i++)
  {
    component_actions[verbs[i]] = ({ c, wanted[verbs[i]] });
    add_action("_component_action", verbs[i]);
  }
}

// Forget the verbs a component asked for, the action included, so a verb it no
// longer wants stops answering.
private void _unregister_component_actions(object c)
{
  string * verbs;
  int i;

  if (!component_actions)
    return;

  verbs = map_indices(component_actions);
  for (i = 0; i < sizeof(verbs); i++)
    if (component_actions[verbs[i]][0] == c)
    {
      map_delete(component_actions, verbs[i]);
      remove_action("_component_action", verbs[i]);
    }
}

/**
 * Where every component verb lands. The verb typed says which component asked
 * for it and what to call there.
 */
int _component_action(string str)
{
  mixed * bound;

  bound = component_actions ? component_actions[query_verb()] : nil;
  if (!bound || !bound[0])
    return 0;

  return (int)call_other(bound[0], bound[1], str);
}

// Clone a component, stamp its type, seed its attrs and bind it to this entity.
private object _spawn_component(string type, mapping attrs)
{
  object c;

  c = clone_object(this_object()->query_component_dir() + type + ".c");
  if (!c)
    return nil;

  c->set_type(type);
  c->init_auto_load_attributes(attrs ? attrs : ([ ]));
  c->initialize(this_object());
  components += ({ c });
  _register_component_actions(c);
  return c;
}

// Let a component go: it cleans up in dest_me, and is destructed if that did
// not.
private void _drop_component(object c)
{
  _unregister_component_actions(c);
  components -= ({ c });
  c->dest_me();
  if (c)
    destruct(c);
}

/**
 * Re-clones every component named in a restored component_info, then tells
 * them all that the set is complete.
 */
void init_components(mapping info)
{
  string * types;
  object * loaded;
  int i;

  if (!components)
    components = ({ });
  if (!info)
    return;

  types = map_indices(info);
  for (i = 0; i < sizeof(types); i++)
    _spawn_component(types[i], info[types[i]]);

  rebuild_hook_chains();

  // a component that depends on another can only reach it once all of them
  // exist; the list is copied, as one may attach another component here
  loaded = ({ }) + components;
  for (i = 0; i < sizeof(loaded); i++)
    if (loaded[i])
      loaded[i]->components_ready();
}

/**
 * Attaches a component of `type` seeded with `attrs`, or reseeds the live one,
 * so the next read sees the seed. Called by whatever builds the entity (a
 * template, a conversion) and by builder commands.
 */
void add_component(string type, mapping attrs)
{
  object live;

  if (!components)
    components = ({ });
  if (!component_info)
    component_info = ([ ]);
  if (!attrs)
    attrs = ([ ]);

  component_info[type] = attrs;

  live = query_component_by_type(type);
  if (live)
  {
    // the seed may change the verbs it asks for
    _unregister_component_actions(live);
    live->init_auto_load_attributes(attrs);
    _register_component_actions(live);
  }
  else
  {
    live = _spawn_component(type, attrs);
    if (!live)
      return;
    rebuild_hook_chains();
    live->components_ready();
  }

  this_object()->components_changed();
}

/**
 * Drops the component of `type`, live instance and saved state.
 */
void remove_component(string type)
{
  object live;

  if (component_info)
    map_delete(component_info, type);

  // the live one goes too, or the next save would pull it straight back into
  // component_info
  live = query_component_by_type(type);
  if (live)
    _drop_component(live);

  rebuild_hook_chains();
  this_object()->components_changed();
}

/**
 * Lets every component go, for an entity that is going itself. Its saved
 * state is left as it is.
 */
void destruct_components()
{
  object * live;
  int i;

  live = ({ }) + query_components();
  for (i = 0; i < sizeof(live); i++)
    if (live[i])
      _drop_component(live[i]);

  components = ({ });
  hook_chains = ([ ]);
}

/**
 * Pulls each live component's current attrs back into component_info, so the
 * entity's save_object keeps them up to date. Called before every save.
 */
void sync_component_info()
{
  int i;

  if (!components)
    return;
  if (!component_info)
    component_info = ([ ]);

  for (i = 0; i < sizeof(components); i++)
    if (components[i])
      component_info[components[i]->query_type()] =
        components[i]->query_auto_load_attributes();
}

/**
 * Calls `func` with `args` on every component that defines it: how an entity
 * tells its components of an event, with the event's own arguments.
 */
void run_on_components(string func, mixed args...)
{
  object * live;
  int i;

  live = ({ }) + query_components();
  for (i = 0; i < sizeof(live); i++)
    if (live[i] && function_exists(func, live[i]))
      call_other(live[i], func, args...);
}

/**
 * Tells every component the entity is now standing in the world.
 */
void components_placed()
{
  run_on_components("placed");
}

/**
 * Pipeline dispatcher. Threads `args` through every hook of `func` in priority
 * order, each returning the (possibly transformed) args for the next. Returns
 * the final args; the caller runs its own implementation with them.
 */
mixed * run_pipeline(string func, mixed * args)
{
  object * chain;
  int i;

  if (!hook_chains || undefinedp(hook_chains[func]))
    return args;

  chain = hook_chains[func];
  for (i = 0; i < sizeof(chain); i++)
    args = chain[i]->call_hook(func, args);

  return args;
}

/**
 * Reduce dispatcher. Asks every hook of `func`, in priority order, for its
 * piece and folds it into `acc`, seeded by the caller, with the combinator
 * named (a function of this entity: combinator(acc, piece) -> acc).
 *
 * A hook claims authority by returning ({ HOOK_EXCLUSIVE, value }): the
 * combinator drops what it had built and starts again from that value. Later
 * hooks still run and add on top of it, so with several exclusive pieces the
 * first one is the baseline and the rest augment it.
 */
mixed run_reduce(string func, mixed * args, mixed acc, string combinator)
{
  object * chain;
  int i;

  if (!hook_chains || undefinedp(hook_chains[func]))
    return acc;

  chain = hook_chains[func];
  for (i = 0; i < sizeof(chain); i++)
  {
    mixed piece;
    piece = chain[i]->call_hook(func, args);
    acc = call_other(this_object(), combinator, acc, piece);
  }

  return acc;
}

/**
 * Combinator: any component saying yes settles it. For the gates where one
 * objection is enough and the rest of the chain cannot overrule it.
 */
int _any_true(mixed acc, mixed piece)
{
  return acc || (piece ? 1 : 0);
}

/**
 * Combinator: string concatenation, nil or empty pieces adding nothing.
 * Honours HOOK_EXCLUSIVE by replacing the accumulator.
 */
string _concat_string(mixed acc, mixed piece)
{
  if (pointerp(piece) && sizeof(piece) == 2 && piece[0] == HOOK_EXCLUSIVE)
    return piece[1];
  if (!piece || !stringp(piece) || !strlen(piece))
    return acc;
  if (!acc) acc = "";
  return acc + piece;
}

/**
 * Combinator: integer sum. HOOK_EXCLUSIVE replaces the running total.
 */
int _sum_int(mixed acc, mixed piece)
{
  if (pointerp(piece) && sizeof(piece) == 2 && piece[0] == HOOK_EXCLUSIVE)
    return piece[1];
  if (!intp(piece))
    return acc;
  return acc + piece;
}
