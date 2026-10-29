# Temples and deities

A deity is a social object (`lib/deity.c`, a game's own under `/games/<game>/obj/deities/`). A player takes one in a temple, **once and forever**: there is no changing god and no renouncing. Deities are always named by their full path ending in `.c`, stored and compared as such (the player's `query_deity_ob()`, a temple's deity, a prop's `overrides.deity`, the handler's keys).

## Temples

A temple is a room built on `lib/room/temple/temple-room.c` (`set_temple_deity("<path>.c")` in its setup) or a location with the `temple` component (`locations temple <path>`). Both share `temple-actions.c`:

- a sign naming the deity and giving the instructions, put up on every load from the help document `help/world/temples` (editing it changes every temple) and again when the deity changes;
- `adorar` / `worship`: takes the temple's deity, after a yes/no confirmation; asked again on the answer;
- `ofrecer` / `offer <item | n coins | n <coin type>>`: an offering, followers only. What is offered is carried, except a corpse, which may lie in the temple; what the corpse holds stays on the floor.

Who is refused, in order: no deity in the temple; a closed temple (location only: the citizenship holding the ground does not accept the deity, see the component); a player who already has one (their own: "you already belong"; another: "your god would be furious"); then the deity's own rules. Offering at another god's temple gets the same "furious" answer.

`room2loc` turns a temple room into a `temple` component with the room's deity; the component puts its own sign up, so no `sign` component is captured.

## Requirements (`lib/deity.c`)

Set in the deity's `setup`; each failed rule has its own refusal (`query_worship_refusal(player)`, nil when allowed):

| rule | API | note |
|---|---|---|
| race | `set_races`, `set_forbidden_races` | race ids; a race admits all its lineages (the race id or the base race id is asked) |
| lineage | `set_lineages`, `set_forbidden_lineages` | lineage ids: a race object with a base race (`lib/lineage.c`) |
| alignment | `set_alignment_range(min, max)`, `set_alignment_band(DEITY_ALIGN_*)` | real alignment, both ends included; bands in `<deities.h>` |
| guild | `set_guilds` (any of them), `set_forbidden_guilds` | guild paths with `.c` |
| citizenship | (always) | the player's own citizenship must accept the deity (`accepts_deity` of the game's diplomacy) |

`query_legal_player` (asked by `set_deity_ob` and NPC templates) checks only race and lineage. A deity masks `query_worship_refusal` for a rule of its own and calls `::query_worship_refusal` for the rest.

Gender: `query_gender()` is male unless the deity's setup calls `set_gender(GENDER_FEMALE)`; `query_god_noun()` gives god / goddess. Texts are filled with `fill_text`: `$god$` the deity's name, `$noun$` god or goddess.

## Offerings

Rules, first match wins: `add_offering(([ DEITY_OFFER_KIND: ..., ... ]))`, `clear_offerings()`.

- `DEITY_OFFER_KIND`: `coins`, `item` (anything but corpses and body parts), `weapon`, `armour`, `shield`, `corpse`, `part` (a body part, `/lib/obj/death/body-parts.md`);
- `DEITY_OFFER_MATERIAL`: a material id, its subtypes included (`metal` takes `metal:steel`); the numbers weapons, armours and shields keep are read through the materials table (`query_material_id`);
- `DEITY_OFFER_BASE`: the base file of one item, with `.c`;
- `DEITY_OFFER_MIN`: the least value;
- `DEITY_OFFER_WORTH`: percent of the value it counts for (100 by default);
- `DEITY_OFFER_REPLY`: the answer, filled with `fill_text`.

For `corpse` and `part`, on whose body it was:

- `DEITY_OFFER_PART`: part ids (`({ PART_HEART })`), `part` only;
- `DEITY_OFFER_RACES`: race ids, a race takes its lineages;
- `DEITY_OFFER_ALIGN_MIN`, `DEITY_OFFER_ALIGN_MAX`: the real alignment of the dead, both ends included (a good god taking only evil bodies: `DEITY_OFFER_ALIGN_MAX : -1`);
- `DEITY_OFFER_LEVEL_MIN`: the least level of the dead;
- `DEITY_OFFER_PLAYERS`: 0 refuses players' bodies, taken otherwise.

A corpse is worth its `query_offering_value()` and a part its value: both grow with the level of the dead and fall as they rot.

The base takes coins and any item worth at least `DEITY_VALUABLE_MIN`. What no rule takes is refused and stays with the player; an item in use, a living or a pile of coins named as an item is refused too. An accepted item is destructed.

## Prayers

`rezar` / `orar` / `pray` is the shared fixed skill `lib/obj/skills/pray.c`, given to every worshipper (at worship, and on login by the deity's `start_player`). It works only where the caster's own deity is worshipped: a temple of it, or an `altar`, `statue` or `ruined_statue` prop with `overrides.deity` naming it (`props set <handle> overrides.deity <path>.c`); a prop without one is nobody's. Anywhere else, another god's place included, it refuses ("your god could take offence").

The skill only dispatches; the deity holds the prayers:

- `add_prayer(id, words [, luck])`, `remove_prayer(id)`, `set_prayer_luck(id, luck)`; a new id needs the deity to mask `grant_prayer(caster, id)` and call `::grant_prayer` for the rest;
- defaults: `heal` (a tenth of max hit points, 1/10..1/15), `power` (the same of energy), `blessing` (+2..6 constitution for a while), `strength` (+1..3), `wisdom` (+1..2 wisdom and intelligence);
- `answer_prayer(caster, id)`: heard when `random(base + wait left) <= luck` (base 500, luck 50); every prayer lengthens the wait; from 5000 the deity scolds, past 8000 it curses (hit points to 1, -1 to every stat, -2 charisma) and the caster cannot pray for a while. `query_pray_base` / `query_pray_annoyed` / `query_pray_cursed` change the marks.

A word that asks for a default prayer the deity removed gets "<god> shuns hearing a prayer like that".

The props' own `pray` action yields to the skill: its spec carries `PROP_SPEC_YIELDS_TO_SKILL: SKILL_PRAY`, so for a player who knows the skill the prop does nothing (the skill, on the player, answered the verb first and its message stands). Without a deity the prop keeps its flavour.

## The deities handler

`lib/handlers/deities.c` keeps, per deity, one number per player: a **devotion** score. Prayers raise it (`DEITY_DEVOTION_PRAYER`, 1 each) and offerings raise it by their worth (one point per `DEITY_DEVOTION_WORTH`, at least 1), up to `DEITY_DEVOTION_MAX`. It falls in real time, `DEITY_DEVOTION_DECAY` points every `DEITY_DEVOTION_PERIOD` seconds, computed lazily from a stored timestamp whenever it is read or written (the way `lib/handlers/crimes.c` decays points); an entry that reaches zero is deleted, so the book holds only the active devotees and a player who stops praying and offering drops out. A game's handler masks `query_devotion_prayer`, `query_devotion_worth`, `query_devotion_max`, `query_devotion_decay` and `query_devotion_period` to change the numbers.

Every game with deities has its own handler (`/games/<game>/handlers/deities.c`, inheriting the shared one, saving to `/save/games/<game>/deities.o`); the shared copy saves nothing. Reach it with `handler("deities", ob)`. An older save with per-follower accounts is ignored on load (a different variable) and replaced on the next save.

API: `record_worship`, `record_offering`, `record_prayer`, `record_curse`, `query_deities()` (deities with an active devotee), `query_devotion(name, deity)`, `query_devotees(deity)` (`({ ({ name, devotion }), ... })`, the most devoted first).

### Lifetime totals

The totals live in the player, not in the handler, and are saved with it (`deity_totals` in `lib/living/social.c`): worship date, prayers made, prayers heard, curses, offerings made and their total worth (`DEITY_TOT_*`). The `record_*` functions of the handler update them through `add_deity_total` / `set_deity_total`; `query_deity_totals()` reads them. They are only reachable while the player is connected, since a save file is never parsed to read them.

The admin command `deities` reads the book: `deities` lists the game's deities and their active devotees, `deities <deity>` shows the deity and its devotees by devotion, `deities player <name>` gives the player's devotion in each deity and, only if they are connected, their lifetime totals.
