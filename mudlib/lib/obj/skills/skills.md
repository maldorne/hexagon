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

### Intent and targets

Every effect declares whom it is for with `set_effect_intent(EFFECT_INTENT_*)` (`<living/effects.h>`), read with `query_effect_intent()`: `help` when it benefits its target (a cure, a blessing, a ward), `harm` when it hurts it (damage, a curse, a hold), `none` (the default) when there is no target to judge (hiding, climbing, detecting, a light that lights the place). Whoever picks targets for an effect, an NPC above all, chooses allies for a help and enemies for a harm. The bases of a game set their default in the function that configures them (`set_area`, `set_wounds`, `set_cure`...) and a spell or skill that differs calls `set_effect_intent` after it.

The target types decide who is hit:

- `TARGET_TYPE_ONE` / `TOUCH`: one living thing, named in the text or handed over.
- `TARGET_TYPE_MANY`: every distinct living thing the caster names, with no maximum. The caster names them separated by commas or by the word for "and" (`formular <spell> orco, kobold y goblin`; "all" names everybody), read like any other name list (`find_match`). Each one must be valid like a single target (`allow_on_*`, `post_extra_checks`) and be in the caster's place; a name nobody answers to, and a target that is refused are dropped with a message to the caster. Every round the targets are checked again against the range (`set_range`, 0 means they must stay in the place) and the ones that left are dropped for good; the effect ends when none is left. A round function receives the array of the targets still there as `target`, and the ones lost in `out_of_range`.
- `TARGET_TYPE_ALL`: everyone in the place, friend or foe, without naming anyone.

A one-target syntax does not change. For a caller that already chose its targets (an NPC) there is a clean entry point: `cast_effect_on(object * targets, object caster [, int quiet])` of `BASE_EFFECT`, which runs the same checks as `cast_effect` but takes the targets instead of the text (many-target: all the valid ones; single or touch: the first; other types ignore them). The balance dump carries `intent`.

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

## Balance: `query_balance`

An effect that damages or heals describes one cast in `mapping query_balance(object caster)` (`BASE_EFFECT`; the default returns nil: neither). It is a pure function for the `balance` command (`packages/balance/`), which writes the numbers of a game as CSV files (`guilds.csv` and `jobs.csv` list what each guild and job teaches or relates, with the same columns): no rolls, no messages, no change of state, and the numbers come from the same helpers the cast uses, so there is one formula. It reads the caster's level and power the way the cast does (`PROPERTY_CAST_LEVEL` and `PROPERTY_CAST_POWER`, see "At a level"), so the level of its curves is the one in the guild that teaches the ability, and the stats of the caster.

`make_balance(caster, kind, rounds, range, type, note [, extra])` builds the answer; the keys:

| key | what |
|---|---|
| `kind` | `"damage"` or `"heal"` |
| `average`, `minimum`, `maximum` | hit points one target gets over the whole cast, all rounds (average is a float; the other two ints) |
| `rounds` | combat rounds from the start of the cast until the last of it is delivered |
| `damage_type` | the damage type, `""` when none |
| `cost` | the gp of the cast: a spell at its lowest category level and the caster's power, a skill its `gp_cost` |
| `note` | short free text: `"per hit"`, `"undead only"`, what was assumed; only what cannot be a number |
| `extra` | optional mapping name -> int, float or string with numbers only some effects have (see below) |

The targeting data are not passed: `make_balance` reads them from the effect itself with `query_targeting()` (names in `query_targeting_keys()`), so every row carries what the effect really declares: `target_type` (the `TARGET_TYPE_*`), `intent` (`EFFECT_INTENT_*`), `reach` (`EFFECT_REACH_*`), `common` (1 when the effect is common), `start_range` (exits away a target may be when the effect starts: 0 unless the reach is ranged, as targets are looked for in the caster's place), `range` (`query_range`: exits away a target that moves is still followed once it has started, measured by the range handler, at most 10), `lockout_time`, `needs_faith`, `needs_sight` (a blinded caster cannot use it on another; single, touch and many targets), `touch_protectable` (a protector of the target may take the touch) and every `allow_*` permission of `BASE_EFFECT`. A base that adds a targeting rule overrides `query_targeting_keys` and `query_targeting`.

`extra` is for what only some effects have: the dump adds one column per name (the union over all effects, empty where an effect lacks it), after `cost`. Used so far: projectile spells `projectiles`, `fired_per_round`, `hit_penalty_per_step`; area spells `rings` (places beyond the caster's that are reached) and `ring_percent` (the damage left in each, `"100/66/44"`); the inferno `caster_burn_percent`; the disintegrate ray `hit_penalty_far`.

Every projectile or touch is assumed to land and no target to resist, unless the note says otherwise. Helpers of the base: `dice_range(dice, faces, bonus)` gives `({ minimum, average, maximum })`, `scale_range(caster, range)` applies the `fix_amount` of the spell base (stat adjustment) to it and `repeat_range(range, times)` repeats it. Those with an amount that depends on the situation (hit location, armour class, the gp of the caster) answer for the plain case and say so in the note.

## Common, guild and job effects

Every effect is a guild or job effect unless it is marked common with `set_common_effect(1)` in its `setup()` (`is_common_effect()`). A common effect is open to everyone, whatever their guild or job, even one a guild or job teaches (a job skill a member keeps using after leaving). The common skills are climb, dismembering, evade, focus, follow, gutting, hide, judge, orientation, pray, protect, pulling, repair, search, skinning, slice, steal and swim; no spell is common. Search, follow, evade and protect are also default skills (`DEFAULT_SKILLS` in `include/living/skills.h`): every player is given them at each login.

A guild or job effect can only be used inside a guild or a job that allows it (`query_allows(key)` of the guild or job, with `key` = `query_effect_key()`: the spell's full path with `.c`, the skill's id, compared exactly). A guild allows what it teaches (`add_guild_spell` by path, `add_guild_skill` by id, which is the skill's file name) and what it relates (`add_guild_related_spell`, `add_guild_related_skill`: tied to the guild but not taught in its room, learned through quests or items). Related effects are not offered by `aprender`, not granted on joining or levelling, not in `query_guild_spells` / `query_guild_skills` and not given by the NPC template `guild_abilities`; read them with `query_guild_related_spells()` / `query_guild_related_skills()`.

A job does the same with `add_job_skill(id, job_level [, value])`, `add_job_spell(path, job_level [, power])`, `add_job_related_skill(id)` and `add_job_related_spell(path)` (`lib/job.c`; readers `query_job_skills`, `query_job_spells`, `query_job_related_skills`, `query_job_related_spells`, `query_job_spell_power`). Guild and job share the machinery in `lib/social/abilities.c` (neutral names `add_ability_skill`, `query_ability_spells`, `query_teaches`, `query_allows`, `grant_ability_skills`, `teach_ability_spells`...), the guild and job names being thin wrappers; the level that gates an ability is the guild level for a guild and the job level for a job. The taught skills of a job are granted on joining it, when its level rises and at login; its taught spells are learned with `aprender` in the job room, by job level. Leaving the job takes nothing back, but a holder can only use the effect while their guild or their job allows it.

A guild or job may add a line to the help of an effect for its members: mask `query_effect_note(key, player)` (`lib/social/abilities.c`, nil by default); `help()` of `BASE_EFFECT` appends the answer of the reader's guild and then of their job.

`cast_effect` refuses a guild or job effect with `MSG_NOT_TAUGHT` (`query_usable_by(caster)`) unless the cast carries a given level, so a scroll works for anyone who can read it. A character who leaves a guild or job keeps the spells and skills but cannot use them until joining a guild or job that allows them. An NPC follows the same rule: one that casts needs `guild` and `guild_level` in its template.

## The level of a spell or skill

One number feeds every formula of an effect (amounts, dice, durations, counts): `query_casting_level(caster)` of `BASE_EFFECT`. It is, in order:

1. the level a cast was given (`PROPERTY_CAST_LEVEL`: a scroll or an item, see "At a level");
2. for a common effect, the class level of the caster (`query_level()`);
3. for an effect the caster's guild allows, the guild level (`query_guild_level()`), with no fallback to the class level;
4. for an effect the caster's job allows (and the guild does not), the job level (`query_job_level()`).

At least 1. The level of a target (resist rolls, hold person, holy word) is not this one: it stays the class level (`query_level()`). So do the baselines of the balance tool (hit points, weapon blow).

`hechizos` and `habilidades` mark with `(!)` the ones neither the current guild nor the current job allows, with a legend line; common effects are never marked.

## Guild and job skills

A guild teaches skills with `add_guild_skill(id, guild_level [, value])` in its `setup` (`lib/guild.c`); `grant_guild_skills` gives them on joining, on reaching the level and at login. Leaving a guild takes nothing back: a player keeps what they learned, but cannot use it outside a guild or job that allows it. The job has the same with `add_job_skill` and `grant_job_skills`.

Outside a guild that teaches them, skills are refused by the rule above. A skill that wants its own in-character message for those outside its guild (a paladin's faith) also checks in `extra_checks`: `query_guild_refusal(caster, <guild path>)` (`lib/effects/guild_fixes.c`, inherited by every effect). For someone outside that guild it returns the guild's `query_non_member_refusal` (nil by default, so the skill stays free; a guild masks it with a message in character). For a member, or when no path is given, it returns the guild's `query_powers_refusal` (a message while the guild withholds its gifts, nil otherwise).

## Spells

A spell is an effect like a skill: it inherits `BASE_EFFECT` and calls `set_effect_type(EFFECT_IS_SPELL)` in `setup`, and its rounds, lasting effects and checks are the ones above. Its categories are spheres (divine magic: clerics, paladins) or schools (arcane magic: mages, sorcerers), with the spell's level in each: `add_category("<sphere or school>", 3)`.

- **Knowing a spell.** `lib/living/spells.c` keeps `spells`, `id -> ({ path })`, saved with the player. The id is the spell's file name without `.c`; the path is found again in the catalogue (below) every time, so a spell may move, and the saved one is only the fallback. `add_spell(id, path)`, `remove_spell(id)`, `query_spell(id)`, `query_spells()`. An NPC template lists them under `spells`.
- **Casting.** `formular <spell> [<target>]` finds the known spell whose name or alias starts the text (the longest name wins) and calls its `cast_effect` with the rest as the target. `hechizos` lists the known spells.
- **Reach and follow.** Two distances. `query_effect_reach()` says where the target must be when the effect starts, one of `EFFECT_REACH_*` (`<living/effects.h>`): `self` (the caster), `touch`, `single` (one target in the same place), `ranged` (one target that may be in another place), `area` (everyone in the place), `wide` (everyone in several places). A target is always looked for in the caster's place, so it is derived from the target type (`TARGET_TYPE_TOUCH` touch, `ONE`/`MANY` single, `ALL` area, anything else self) and only an effect that finds its target elsewhere sets `ranged` with `set_effect_reach(id)` (also touch spells that use `TARGET_TYPE_ONE`, area spells that name their targets, wide ones). `set_range(n)` is the other distance: how many exits away a single target that moves is still followed once the effect has started; 0 means it must stay in the caster's place. `help` shows both («Alcance» and «Si huye»), `hechizos` lists the reach beside each spell; the ids are English and never shown, the names are in `lib/effects/.lang.*.h`.
- **Taking spells away.** A living with the property `nocast` (`NOCAST_PROP`, in `<common/properties.h>`; a timed one ends by itself) cannot `formular` anything, though it may still use skills (the `noskill` property stops both). A spell that makes the caster pay for its power with his own spells adds it.
- **Armour.** `formular` asks the member's guild `query_legal_casting_armour(piece)` for every worn piece and refuses the cast if any answers 0 (`lib/living/spells.c`). The piece itself is passed, so a guild can judge its material (`query_material()`, 2 is metal), its kind (`query_armour_type_name()`) or which piece it is (`query_armour_name()`). The default in `lib/guild.c` only asks `query_legal_armour` about the kind, so a guild that says nothing restricts nothing.
- **Spheres and schools.** The base asks the caster `query_effect_category(category)`: 0 unknown, 1 minor, 2 neutral, 3 major. A member's guild answers it from what its `setup` declares, `set_spheres(([ "<sphere>": EFFECT_CAT_MAJOR ]))` for divine magic and `set_schools(([ "<school>": EFFECT_CAT_MINOR ]))` for arcane (`query_category_knowledge` on the guild); the old per-living lists (`add_sphere`) count when higher. The spell's level is its level in the best known category it belongs to (1 when none), and the gp cost grows with that level, double for a minor category and half for a major one. `esferas` and `escuelas` list them.
- **The catalogue.** `lib/tables/spells.c` (`table("spells")`, extended by each game at `/games/<game>/tables/spells.c`) lists every spell (`add_spell(id, path)`, or `add_spells_in(dir)` for every `<dir>/<sphere>/<spell>.c`) and declares every category with `add_spell_category(id, SPELL_CATEGORY_SPHERE | SPELL_CATEGORY_SCHOOL, name)`: its kind and the name the player reads, which `esferas`, `escuelas` and the effect's help show (`query_category_name` on the guild asks it). The mudlib declares none.
- **Guild spells.** `add_guild_spell(path, guild_level[, power])` in the guild's `setup`. A member learns them with `aprender` in the guild room: every spell of a level up to theirs that they do not know yet, so a spell moved to a lower level later reaches whoever is past it. With nothing new, `aprender` names the next spell and its level. `normas` lists them by level. Leaving the guild takes nothing back, but a spell can only be cast inside a guild that teaches it (see the rule above); a spell that wants its own message for outsiders checks `query_guild_refusal` like a skill.
- **Power of one spell.** The optional third argument of `add_guild_spell` is `EFFECT_CAT_MINOR`, `EFFECT_CAT_NEUTRAL` or `EFFECT_CAT_MAJOR`: for that guild and that spell only, it replaces the power the caster would get from the spell's categories, and it lets a member cast the spell without knowing any of them. `query_guild_spell_power(path)` on the guild (and on a living, which asks its guild) answers 0 when none is set. `query_effect_power(caster)` of `BASE_EFFECT` is the one place that decides the power (the override, else the best category), and the gp cost of a spell (double for minor, half for major) and the spell's own formulas (`query_power` of a game's spell base) read it. The spell level that prices the spell is still its level in the best category the caster knows (1 when none).

### Scrolls and casting at a given level

A scroll (`/lib/obj/scroll.c`, an item) holds one spell. An item inherits it and writes the spell in `setup`: `set_scroll_spell("<path of the spell>")` (nil leaves it blank) and `set_scroll_level(n)`, the guild level 1 to 20 the spell runs at. The scroll is named after the spell (`pergamino de <spell>`, with the plain-ASCII aliases) and priced by the spell's lowest category level times the scroll level.

- **Reading.** `leer <scroll> [sobre <target>]` (`read <scroll> [on <target>]`): `lib/cmds/player/read.c` splits the target off and calls `read_scroll(reader, target)` on the scroll; an NPC calls the same function. The reader must carry it and see (`check_dark`).
- **Who can.** `query_scroll_refusal(reader)`: the spell's `query_guild_refusal` (a guild withholding its gifts) and `query_effect_power(reader)` (some category of the spell known through the reader's guild or the reader's own spheres, or a power the guild gives it). A scroll is an item like any other: whoever knows the magic reads it, whatever guild sold it. Anyone else finds runes; `look` says which. Knowing the category only as minor (power 1) botches 20 in 100 readings: the scroll burns.
- **The cast.** `read_scroll` calls `cast_effect_at(target, reader, level, power)` of the spell (`BASE_EFFECT`): the normal `cast_effect` (every check, the target search, the rounds) with no gp spent and no begin messages. When it starts the scroll crumbles; when it is refused (no target, a guild refusal) the scroll stays and `notify_fail` says why.
- **At a level.** `cast_effect_at` holds the level on the caster as the timed property `PROPERTY_CAST_LEVEL` (`<living/effects.h>`) for as long as the rounds last; a normal cast clears it. `query_given_level(caster)` of `BASE_EFFECT` returns it (0 when none). `query_casting_level(caster)` reads it first, so every formula that asks it (the spell bases' `query_caster_level` is only that level scaled) follows the scroll's level. A spell or skill that reads `query_guild_level()` or `query_level()` of the caster directly ignores it: always ask `query_casting_level`.
- **With a power.** The scroll is written with a power too (`set_scroll_power`, 1 minor to 3 major, default 2 neutral): `cast_effect_at` holds it as `PROPERTY_CAST_POWER` and `query_effect_power` returns it while the rounds last, so the formulas follow the writer's knowledge, not the reader's. The reader's own power only decides whether they can read it and whether they botch it, both checked before the cast.
- **Selling them.** A shop stocks by file: one small item per scroll (`scroll-of-<spell>.c` in the guild's `items/`, inheriting the scroll and writing its spell and level in `setup`), added with `add_permanent_goods`.

## Body parts

`skinning`, `gutting`, `dismembering` and `pulling` take parts from a corpse, each the parts its race gives that skill; they share `base-extraction.c`. See `/lib/obj/death/body-parts.md`.

## Listing

The `skills` command draws a bar for percentage skills, one slot per level for levelled ones and a single slot for fixed ones; passives keep `(*)`:

```
	Climb                      [********************] (100%)
	Berserk                    [#][#][ ]              (2/3)
	Tracking                   [#]                    (known)
```
