# Entities and components

An **entity** is an object that carries **components**: small objects, one per
type, each holding one slice of its behaviour. NPCs and locations are entities;
a schedule, a guard post or a toll is a component of an NPC, a shop, the props
or a maze is a component of a location.

| | file |
|---|---|
| what an entity has | `/lib/core/ecs/entity.c` (inherited by `/lib/npc.c` and `/lib/location.c`) |
| what a component is | `/lib/core/ecs/component.c` |
| NPC components | base `/lib/npc/component.c` (object half), in `/lib/npc/components/` |
| location components | base `/lib/location/component.c` (container half), in `/lib/location/components/` |
| hook priorities | `<ecs/ecs.h>` |

Each kind of entity has its own component base, which inherits
`/lib/core/ecs/component.c` together with the object or container half it needs.
A new kind of entity inherits `entity.c`, defines `query_component_dir()` and
gets everything below.

## The entity

- `add_component(type, attrs)`: attaches a component of `type`, cloned from
  `query_component_dir() + type + ".c"`, seeded with `attrs`; on a live one it
  reseeds it. A `type` that starts with a slash is the full path of a component
  file without the `.c`, so a game ships components of its own (it inherits the
  entity's component base and is kept under that path).
- `remove_component(type)`: drops the live component and its saved state.
- `query_component_by_type(type)`, `has_component(type)`, `query_components()`.
- `run_on_components(func, args...)`: calls `func` with those arguments on
  every component that defines it. This is how an entity passes on its events
  (`event_enter`, `event_moved`, ...) and orders (`do_schedule`).
- `components_placed()`: tells every component the entity is now standing in
  the world.
- `components_changed()`: called after every attach, reseed or drop. Empty by
  default; a location saves itself there.

The entity calls `entity::create()` from its create, `sync_component_info()`
before saving, `init_components(component_info)` after restoring and
`destruct_components()` when it goes.

### Persistence

`component_info` (type -> attrs) is a variable of the entity and travels in
its savefile; the live components are static and re-cloned on restore. Before
every save the entity pulls each component's `query_auto_load_attributes()`
into it; on restore each one gets them back through
`init_auto_load_attributes()`. A component never saves itself.

### Hooks

A component can take part in the entity's own functions. It lists them in
`query_hooks()` as `([ function : HOOK_PRIORITY_* ])` and answers each in
`hook_<function>(mixed * args)`. The entity sorts each function's chain by
priority (lower first, ties in load order) and runs it in one of two ways:

- `run_pipeline(func, args)`: each hook returns the args, possibly changed,
  for the next. A location runs its exits through it: a gate can stop the
  move, a maze can change where it leads.
- `run_reduce(func, args, acc, combinator)`: each hook returns a piece, folded
  into `acc` with `_concat_string`, `_sum_int` or `_any_true`. A location builds
  its long, short and light this way. A piece `({ HOOK_EXCLUSIVE, value })`
  replaces everything gathered so far.

### Verbs

A component stands nowhere, so it cannot carry a verb. One that wants a verb
lists it in `query_component_actions()` as `([ verb : function ])`; the entity
adds the action and calls that function on the component.

## The component

In the order things happen to it:

1. `create()`.
2. `set_type(type)`: stamped by the entity, the key it is kept by.
3. `init_auto_load_attributes(attrs)`: its saved state.
4. `initialize(entity)`: bound to its entity, then `query_entity()` returns it.
5. `components_ready()`: every component of the entity exists, for one that
   depends on another.
6. `placed()`: the entity is standing in the world.
7. `dest_me()`: the entity drops it or is going itself.

Every hook in that list is optional. `query_info()` gives a one-line summary for
the `components` command, `stats()` the component's part of the entity's stats.
