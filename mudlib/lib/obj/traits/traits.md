# Traits

A trait is a lasting condition a living carries: a wound that bleeds, a poison, later a curse or a blessing. The behaviour lives in one object per kind of trait; the living keeps only the data of each one it carries, so a trait is loaded once, never cloned, and uses no shadows.

Files: `lib/obj/traits/<id>.c` (generic) and `games/<game>/obj/traits/<id>.c` (a game's own), each inheriting `trait.c` (`TRAIT_BASE`). The living's side is `lib/living/traits.c`, inherited by `lib/living/living.c`; constants in `<living/traits.h>`.

## The living

`mapping traits` (id -> data mapping) is a saved variable. Players and persisted NPCs write it with the rest of their variables (`save_object` in `player.o` and in the census `npc.o`); both call `restore_traits()` after `restore_object`, which sums the effects again and wakes the heart_beat. A death clears them all (`TRAIT_END_DEATH`).

An id resolves to a file: the game's `obj/traits/<id>.c` first (the game of the living, `game_root`), then `/lib/obj/traits/<id>.c`. The file found is stored in the data under `"file"` so a carried trait resolves wherever the living stands; do not set that key.

| call | what |
|---|---|
| `add_trait(id, data)` | copies and stores the data, calls `trait_added`; 0 if the id resolves to nothing or the trait carried was kept (see merge) |
| `remove_trait(id, reason)` | removes it, then calls `trait_removed`; `reason` is a `TRAIT_END_*` (`removed` by default) |
| `has_trait(id)`, `query_trait(id)`, `query_traits()` | presence, a copy of one's data (nil), a copy of all (id -> data) |
| `clear_traits(reason)` | removes every trait |
| `query_trait_stat(name)`, `query_trait_flag(flag)` | what the traits add to a stat; how many traits raise a flag |
| `query_trait_look()` | the lines the traits add to how the living looks |

Data has to be savable: numbers, strings, arrays and mappings of them, never an object. A `"source"` given as an object is stored by its object name. `"expires"` is an optional absolute `time()`; the living ends the trait (`expired`) in the heart_beat that finds it past.

## The hooks of a trait

All receive the living and the trait's data (the same mapping the living stores, so a hook may complete it in place).

| hook | when |
|---|---|
| `trait_effects(living, data)` | fixed contributions, summed on every change: `([ TRAIT_EFFECT_STATS : ([ "dex" : -1 ]), TRAIT_EFFECT_FLAGS : ({ "flag" }) ])`; stat names in `TRAIT_STATS` |
| `trait_look(living, data)` | one line for `look`, in the mud's language, or nil |
| `trait_added(living, data)` | after it was added |
| `trait_removed(living, data, reason)` | after it left; `reason` tells why (`expired`, `removed`, `replaced`, `death`, `error`) |
| `trait_tick(living, data)` | every heart_beat, before combat; return 1 to say it is over (removed as `expired`) |
| `trait_merge(living, old, incoming)` | the id is already carried and added again: return `old` to keep it, `incoming` to replace it (default; `trait_removed` with `replaced`, then `trait_added`) |

The summary (stat deltas, flags) is rebuilt only on add, remove, restore and expiry; the stat queries read it with no walk of the traits. A living with no traits returns from `tick_traits` at once. A hook that errors is caught and logged (`debug("traits")`); a tick that errors removes the trait (`error`).

## Writing one

```c
#include <living/traits.h>
#include <language.h>

inherit TRAIT_BASE;

mapping trait_effects(object living, mapping data)
{
  return ([ TRAIT_EFFECT_STATS : ([ "dex" : -1 ]) ]);
}

string trait_look(object living, mapping data)
{
  return _LANG_TRAIT_SLOWED_LOOK;
}
```

Strings go in the directory's `.lang.<lang>.h` as everywhere. `damage-over-time.c` is the base of the traits that hurt every so often: data `damage` (+ optional `spread`), `left`, `every`, optional `first`, `source`; the damage goes through `adjust_hp` crediting the source while it is still around; the trait gives its messages in `trait_pulse`; a worse one (more damage left) wins a merge.

## Traits in the tree

- `bleeding.c`: a wound on a body part (`part` and its `article`, named by whoever adds it); raises the flag `bleeding`. Healing closes it by removing it with the reason `cured` (a game's healing spells decide how often), which tells the living the wound closes.
- `poisoned.c`: venom in the blood; raises the flag `poisoned`.

## Wired and pending

Wired: the stat deltas (`stats_trait` in `lib/living/stats.c`, summed in `query_str` ... `query_wil`; the carry capacity follows strength), the look line (`long` in `lib/living/living.c`), the heart_beat tick, the save, death, and the npc heart_beat staying on while a trait is carried (`check_heart_beat` in `lib/monster.c`).

Pending (the flags are declared, nothing reads them yet): can act, can move, can speak, can hear, can see, to-hit, armour class, damage bonus, regeneration (`heal_hp` / `heal_gp`), and a trait that survives death.
