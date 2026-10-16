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

## Guild skills

A guild teaches skills with `add_guild_skill(id, guild_level [, value])` in its `setup` (`lib/guild.c`); `grant_guild_skills` gives them on joining, on reaching the level and at login. Leaving a guild takes nothing back: a player may join another and keeps what they learned.

A skill that only the members of one guild may use (a paladin's faith) checks it in `extra_checks`: `query_guild_refusal(caster, <guild path>)` (`lib/effects/guild_fixes.c`, inherited by every effect). For someone outside that guild it returns the guild's `query_non_member_refusal` (nil by default, so the skill stays free; a guild masks it with a message in character). For a member, or when no path is given, it returns the guild's `query_powers_refusal` (a message while the guild withholds its gifts, nil otherwise).

## Listing

The `skills` command draws a bar for percentage skills, one slot per level for levelled ones and a single slot for fixed ones; passives keep `(*)`:

```
	Climb                      [********************] (100%)
	Berserk                    [#][#][ ]              (2/3)
	Tracking                   [#]                    (known)
```
