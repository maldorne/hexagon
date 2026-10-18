# Skills

A skill is an object under `lib/obj/skills/` (a game may ship its own under its tree) plus a row in the skills table (`lib/tables/skills.c`; `/games/<game>/tables/skills.c` resolves first and may add rows with `add_skill`). A living's skills live in `lib/living/skills.c`: `known_skills` and `skill_list[id]`, a copy of the row saved in the player file.

## Row

```
id : ({ path, initial value, times used, ACTIVE_SKILL | PASSIVE_SKILL [, kind [, levels]] })
```

Indices are `SKILL_DATA_*` in `<living/skills.h>`. The last two entries are optional: a four-entry row is a percentage skill.

## Kinds

The stored value means the kind. There is no conversion between kinds: a levelled skill at level 2 is not 66%, the skill's own code asks for its level and behaves differently.

| Kind | Define | Stored value | Improves with use |
|---|---|---|---|
| percentage | `SKILL_KIND_PERCENTAGE` | ability 1-100 | yes (every 50 uses, a roll for +1%) |
| levelled | `SKILL_KIND_LEVELLED` + `levels` | level 1..N | no |
| fixed | `SKILL_KIND_FIXED` | 1 (known) | no |

```
SKILL_BERSERK : ({ SKILLS_PATH + SKILL_BERSERK, 1, 0, ACTIVE_SKILL, SKILL_KIND_LEVELLED, 3, }),
```

How a level rises or falls is not the mudlib's business: no learn command, trainer or automatic rule. Whoever wants it calls the API (a guild, a quest, a script).

## API (`lib/living/skills.c`)

- `query_skill_kind(id)`: the kind, read from the table now (falls back to the stored row; default percentage).
- `query_known_skill(id)`: whether it is known; the test for a fixed skill.
- Percentage: `query_skill_ability(id)` (0 for another kind), `adjust_skill_ability(id, delta, silence)` (clamped 1-100).
- Levelled: `query_skill_level(id)` (0 if unknown or not levelled), `query_skill_max_level(id)`, `set_skill_level(id, n)`, `adjust_skill_level(id, delta, silence)` (clamped 1..N, tells the player when it rises).
- Any kind: `query_skill_value(id)` (the stored value, 0 if unknown) and `set_skill_value(id, value, silence)`: teaches the skill if needed and sets the value by kind (ability, level, nothing for fixed). Templates, class `set_starting_skill`, guild skills and quest rewards use it; the last three only raise.

Old saves are not migrated: the kind comes from the table, and a stored value out of range (a skill that changed kind or lost levels) is clamped when read.

## Effects: how a skill or spell runs

A skill or spell object inherits `BASE_EFFECT` (`lib/effects/base_effect.c`, in `<living/effects.h>`) and configures itself in `setup`: name, cost, lockout, target type, permissions (`allow_*`), and `set_rounds`. The living's side is `lib/living/effects.c`.

### Execution

`set_rounds(({ "round1", "do_it" }))` lists what happens in each round: a function name of the effect (`int f(caster, target, out_of_range, round, quiet)`), a message for the caster, or a pair of messages (caster, room). `cast_effect` checks everything and stores the effect on the caster with `add_active_effect`; the living runs one round per combat round (every two heart_beats) from `do_active_effects`, and `set_fast_casting(1)` runs the first round at once. A round that returns 0 or less interrupts the effect; after the last round the caster pays the gp, gets the lockout (`LOCK <file>`) and, for a skill, a use towards improving it.

Only one effect runs at a time: while one is in execution any other fails with `MSG_BUSY`.

### Lasting effects

Anything an effect keeps doing after its rounds ended (renew a target, a damage over time, a bonus that wears off) is a lasting effect: a second list on the living, never saved, which does not block other skills or spells. Every heart_beat, in or out of combat and before combat runs, the living calls `source->lasting_tick(living, data, ticks_left)` (ticks count down to 1). It ends when the ticks run out, when the tick returns 0, when the call errors or when the source object is gone; then `source->lasting_end(living, id, data, reason)` runs, with `reason` one of `LASTING_END_*` (`expired`, `done`, `error`, `removed`, `replaced`, `cleared`, `death`), so the effect gives its own message and undoes what it did. Both run inside `catch`.

From an effect, three helpers of the base (the id is the effect's file):

```
int do_it(object caster, mixed target, mixed out_range, int time, int quiet)
{
  start_lasting(caster, 10, target);   // ten heart_beats; again = replace
  return 1;
}

int lasting_tick(object caster, mixed data, int ticks_left)
{
  if (!data || environment(data) != environment(caster))
    return 0;                          // ends it, reason "done"
  data->adjust_hp(-1, caster);
  return 1;
}

void lasting_end(object caster, string id, mixed data, string reason)
{
  if (reason == LASTING_END_EXPIRED)
    tell_object(caster, "It wears off.\n");
}
```

`stop_lasting(caster)` ends it (`removed`), `query_lasting(caster)` says whether it is in force. The default `lasting_tick` returns 1 (an effect that only lasts and undoes itself in `lasting_end`). On the living: `add_lasting_effect(id, source, func, ticks, data)`, `remove_lasting_effect(id)`, `query_lasting_effect(id)`, `query_lasting_effects()`, `clear_lasting_effects([reason])`; `stats` lists them. `focus.c` is the example in the tree.

No skill or spell keeps its own `call_out` chain to repeat something: anything periodic is a lasting effect, so recovery and death reach it.

### Recovery and death

`heart_beat_failed()` on the living drops every effect in execution, every lasting effect (`cleared`) and the action queue. The heart_beat handler (`lib/core/secure/heart_beats.c`) calls it when a heart_beat errors, then turns the heart_beat off; the `restart` command calls it and turns the heart_beat back on, whether it was off or not. Dying ends the lasting effects with `death`.

## Guild skills

A guild teaches skills with `add_guild_skill(id, guild_level [, value])` in its `setup` (`lib/guild.c`); `grant_guild_skills` gives them on joining, on reaching the level and at login. Leaving a guild takes nothing back: a player may join another and keeps what they learned.

A skill that only the members of one guild may use (a paladin's faith) checks it in `extra_checks`: `query_guild_refusal(caster, <guild path>)` (`lib/effects/guild_fixes.c`, inherited by every effect). For someone outside that guild it returns the guild's `query_non_member_refusal` (nil by default, so the skill stays free; a guild masks it with a message in character). For a member, or when no path is given, it returns the guild's `query_powers_refusal` (a message while the guild withholds its gifts, nil otherwise).

## Spells

A spell is an effect like a skill: it inherits `BASE_EFFECT` and calls `set_effect_type(EFFECT_IS_SPELL)` in `setup`, and its rounds, lasting effects and checks are the ones above. Its categories are spheres (divine magic: clerics, paladins) or schools (arcane magic: mages, sorcerers), with the spell's level in each: `add_category("<sphere or school>", 3)`.

- **Knowing a spell.** `lib/living/spells.c` keeps `spells`, `id -> ({ path })`, saved with the player. The id is the spell's file name without `.c`; the path is found again in the catalogue (below) every time, so a spell may move, and the saved one is only the fallback. `add_spell(id, path)`, `remove_spell(id)`, `query_spell(id)`, `query_spells()`. An NPC template lists them under `spells`.
- **Casting.** `formular <spell> [<target>]` finds the known spell whose name or alias starts the text (the longest name wins) and calls its `cast_effect` with the rest as the target. `hechizos` lists the known spells.
- **Reach.** `query_effect_reach()` says how far an effect reaches, one of `EFFECT_REACH_*` (`<living/effects.h>`): `self` (the caster), `touch`, `single` (one target in the same place), `ranged` (one target, maybe in another place), `area` (everyone in the place), `wide` (everyone in several places). It is derived from the target type and `set_range`: `TARGET_TYPE_TOUCH` is touch, `ONE`/`MANY` is single with range 0 and ranged above it (the range counts exits from the caster), `ALL` is area, anything else is self. `set_effect_reach(id)` overrides it (touch spells that use `TARGET_TYPE_ONE`, area spells that name their targets, wide ones). `help` shows it as «Alcance»/«Reach» (a ranged one with its range) and `hechizos` lists it beside each spell, through `query_effect_reach_name()`; the ids are English and never shown, the names are in `lib/effects/.lang.*.h`.
- **Spheres and schools.** The base asks the caster `query_effect_category(category)`: 0 unknown, 1 minor, 2 neutral, 3 major. A member's guild answers it from what its `setup` declares, `set_spheres(([ "<sphere>": EFFECT_CAT_MAJOR ]))` for divine magic and `set_schools(([ "<school>": EFFECT_CAT_MINOR ]))` for arcane (`query_category_knowledge` on the guild); the old per-living lists (`add_sphere`) count when higher. The spell's level is its level in the best known category it belongs to (1 when none), and the gp cost grows with that level, double for a minor category and half for a major one. `esferas` and `escuelas` list them.
- **The catalogue.** `lib/tables/spells.c` (`table("spells")`, extended by each game at `/games/<game>/tables/spells.c`) lists every spell (`add_spell(id, path)`, or `add_spells_in(dir)` for every `<dir>/<sphere>/<spell>.c`) and declares every category with `add_spell_category(id, SPELL_CATEGORY_SPHERE | SPELL_CATEGORY_SCHOOL, name)`: its kind and the name the player reads, which `esferas`, `escuelas` and the effect's help show (`query_category_name` on the guild asks it). The mudlib declares none.
- **Guild spells.** `add_guild_spell(path, guild_level)` in the guild's `setup`. A member learns them with `aprender` in the guild room: every spell of a level up to theirs that they do not know yet, so a spell moved to a lower level later reaches whoever is past it. With nothing new, `aprender` names the next spell and its level. `normas` lists them by level. Leaving the guild takes nothing back; a spell only members may cast checks `query_guild_refusal` like a skill.

## Body parts

`skinning`, `gutting`, `dismembering` and `pulling` take parts from a corpse, each the parts its race gives that skill; they share `base-extraction.c`. See `/lib/obj/death/body-parts.md`.

## Listing

The `skills` command draws a bar for percentage skills, one slot per level for levelled ones and a single slot for fixed ones; passives keep `(*)`:

```
	Climb                      [********************] (100%)
	Berserk                    [#][#][ ]              (2/3)
	Tracking                   [#]                    (known)
```
