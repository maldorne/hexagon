# Class experience and levels

A character's class decides two things about experience: how much of each kind
of xp the character earns, and how much xp each level costs. Both are set in the
class's `setup()`, so a game tunes its progression by editing its class files.

## What a level costs

```
xp for the next level = cost x level x growth ^ (level - 5)
```

- `cost` is per class: `set_level_xp_cost(n)`, 1500 when a class says nothing.
- `growth` is how much more each level costs than the one before:
  `set_level_xp_growth(f)`, 1.3 by default (30% more per level).
- Level 5 is where a new character starts (`FIRST_PLAYED_LEVEL` in `class.c`);
  the levels below it are given automatically.

The `level` factor follows the xp a creature gives, which also grows with its
level, so a character fighting creatures of its own level needs a number of
kills that only grows by `growth`: about 30% more each level.

`query_next_level_xp(player)` returns the figure. `MAX_LEVEL` (20) is the last
level.

## What a character earns

`set_xp_types(([ ... ]))` gives each kind of xp a percentage. The kinds are in
`<user/xp.h>`:

| kind | earned from | where |
|---|---|---|
| `KILL_XP` | killing a creature: its level x 60, shared among the attackers, scaled by the death statistics (1.0 until there is a week of data, then between 0.8 and 2.0) | `/lib/living/death.c` |
| `ARMED_COMBAT_XP`, `UNARMED_COMBAT_XP` | every round: the damage dealt x (victim level / own level) | `/lib/living/combat.c` |
| `MAGIC_XP` | every round of a spell: what the spell did | `/lib/living/effects.c` |
| `SKILL_XP` | (declared, not awarded by anything yet) | |

A kind the class does not list gets 100%. Two rules shape the numbers:

- Killing something more than 2 levels below the killer gives no kill xp, so a
  character has to move on to harder places to keep levelling.
- The combat division is an integer one: a victim of a lower level than the
  attacker gives no combat xp at all; one of the same level or higher gives the
  damage dealt, which over a whole fight is about the victim's hit points
  (constitution x level).

So against a creature of its own level a character earns, per kill, about:

```
level x 60 x kill%  +  constitution x level x combat%
```

With an average constitution of about 10 that is roughly `level x k`, where
`k` depends on the class.

## Choosing the cost of a class

Set `cost` so that every class needs about the same kills: a class that earns
less per kill gets a lower cost. With `cost = 30 x k`, a character needs about
30 kills of its own level from level 5 to 6, whatever its class.

Example, three classes with these percentages:

| class | kill | armed combat | other | k | cost |
|---|---|---|---|---|---|
| warrior | 75% | 100% | | 55 | 1650 |
| explorer | 70% | 70% | skills 100% | 49 | 1470 |
| scholar | 50% | 50% | magic 100% | 35 | 1050 |

The scholar's cost is set from what it earns by fighting alone. Once it casts
spells it also earns magic xp, so it levels faster than the others; how much
faster is tuned with its `MAGIC_XP` percentage, without touching its cost.

## The numbers

xp to the next level and kills of creatures of the character's own level, with
the example classes above (`growth` 1.3):

| level | warrior xp | explorer xp | scholar xp | kills (any class) |
|---|---|---|---|---|
| 5 -> 6 | 8,250 | 7,350 | 5,250 | 30 |
| 6 -> 7 | 12,870 | 11,466 | 8,190 | 39 |
| 7 -> 8 | 19,519 | 17,390 | 12,421 | 51 |
| 8 -> 9 | 29,000 | 25,836 | 18,454 | 66 |
| 9 -> 10 | 42,413 | 37,786 | 26,990 | 86 |
| 10 -> 11 | 61,263 | 54,580 | 38,985 | 111 |
| 11 -> 12 | 87,606 | 78,049 | 55,749 | 145 |
| 12 -> 13 | 124,242 | 110,688 | 79,063 | 188 |
| 13 -> 14 | 174,974 | 155,886 | 111,347 | 245 |
| 14 -> 15 | 244,963 | 218,240 | 155,886 | 318 |
| 15 -> 16 | 341,199 | 303,977 | 217,127 | 414 |
| 16 -> 17 | 473,130 | 421,516 | 301,082 | 538 |
| 17 -> 18 | 653,511 | 582,219 | 415,870 | 699 |
| 18 -> 19 | 899,539 | 801,407 | 572,433 | 909 |
| 19 -> 20 | 1,234,367 | 1,099,709 | 785,506 | 1,181 |
| total 5 -> 20 | 4,406,846 | 3,926,099 | 2,804,353 | ~5,000 |

Against creatures one or two levels below, the kills go up: they give less kill
xp and no combat xp. In a starting area whose creatures are levels 2 to 5 and a
few up to 10, a level 5 character needs about 40-50 kills of the common
creatures (or about 20 of the strongest) to reach level 6, and about 90-140 (or
about 30) to reach level 7, as the weakest stop giving xp.

How long that is depends on how long a fight lasts. At 40 to 60 kills an hour,
counting rests, levels 5 to 20 by killing alone take about 85 to 125 hours;
quests and other sources shorten it.

For reference, an established MMORPG with a level cap of 60 asks for about 600
kills of the player's level for its last level, and a first-time player takes
150 to 240 hours to reach the cap.

## Tuning

- More or fewer kills at every level: change `cost` in proportion.
- A steeper or flatter climb: `growth`. With 1.25 the last level asks for ~680
  kills and the whole climb ~3,300; with 1.35, ~2,000 and ~7,600.
- One kind of xp worth more or less to a class: its percentage in
  `set_xp_types`.
