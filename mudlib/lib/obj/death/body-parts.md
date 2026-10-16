# Corpses and body parts

A living that dies leaves a corpse (`corpse.c`); with the right skill and a knife or a dagger, parts can be taken from it (`body-part.c`). Both can be sold and offered to a deity (see `/lib/room/temple/temple.md`). Ids and spec keys are in `<living/body-parts.h>`.

## The corpse

`make_corpse(killer)` (`lib/living/death.c`) keeps on the corpse who the dead was:

| query | what |
|---|---|
| `query_owner()`, `query_given_name()` | the name the world saw, and the one they had |
| `query_race_ob()`, `query_race_id()` | the race, path with `.c` and id |
| `query_base_race_ob()`, `query_base_race_id()` | the base race of a lineage, `""` for a race |
| `query_dead_level()`, `query_dead_align()` | level and real alignment |
| `query_was_player()` | whether it was a player |
| `query_killer_name()`, `query_killer_file()` | who killed them, `""` when nobody did |
| `query_died_at()` | time of death |
| `query_parts_taken()`, `query_parts_left(id)` | what was taken (or ruined), what is still on it |

`start_decay()` starts its heart beat: the corpse goes through remains and rotten remains and may rise as an undead. `query_freshness()` (100 down to 0) falls a tenth with each stage. A corpse has no value; `query_offering_value()` is its race's body size times `CORPSE_WORTH_PER_SIZE`, grown with the level (`BODY_LEVEL_WORTH`: five times at level 20), cut by its freshness and by a tenth for every part taken (never below `CORPSE_LEAST_WORTH` percent).

## The parts of a race

Each race says the kind of body it has (`set_body_kind(BODY_*)`), or it follows from its race id: animal is a beast, bird, reptile, insect, critter and undead are their own kind, unknown is `BODY_NONE`, any other race is humanoid. Each kind has its default parts (`query_kind_body_parts` in `lib/race.c`):

| kind | parts (skill) |
|---|---|
| humanoid | head, two hands (dismembering), heart (gutting), teeth (pulling) |
| beast | skin (skinning, ability 10), head (dismembering), heart (gutting), fangs, claws (pulling) |
| bird | plumage (skinning, 20), head, heart, claws |
| reptile | scales (skinning, 30), head, heart, fangs |
| insect | mandibles (pulling) |
| critter | skin (skinning, 5), tail (dismembering) |
| undead | head (dismembering), teeth (pulling) |
| none | nothing |

A part's spec: `BODY_PART_SKILL`, `BODY_PART_MIN` (least ability), `BODY_PART_WEIGHT`, `BODY_PART_VALUE` (base value), `BODY_PART_COUNT` (1 by default), `BODY_PART_PERISHABLE`, and optionally `BODY_PART_FILE` (the object to clone, one inheriting `body-part.c`) and its own names (`BODY_PART_NAME`, `_PLURAL`, `_ALIASES`, `_FEMININE`, `_PLURAL_NOUN`) for a part the translations (`<translations/body-parts.h>`) do not name.

A race changes them in its setup: `add_body_part(id, spec)` adds a part or merges into the one it has, `remove_body_part(id)` drops it. `query_body_parts()` gives the result. A lineage (`lib/lineage.c`) starts from its base race's parts, the base race's changes included, unless it sets a kind of its own.

## The skills

One skill per kind of part, any of them percentage, all on `lib/obj/skills/base-extraction.c`: `skinning` (despellejar / skin), `gutting` (destripar / gut), `dismembering` (desmembrar / dismember), `pulling` (arrancar / pull). Nothing asks where the skill came from.

`<verb> <corpse>` takes the first of the skill's parts still on the body, `<verb> <part> de <corpse>` (`from` in English) a given one. The corpse may be carried or in the room; a knife or a dagger must be wielded; the race must be known; the ability must reach the part's minimum. The chance is 50% plus what the ability passes the minimum (10-95%); a failure ruins the part, and it counts as taken all the same. Not in combat.

## The body part

`body-part.c` (`BODY_PART_OB`): `set_part(id, spec)`, `set_source(corpse)`, `set_freshness(percent)`; it keeps the part id, its names, base value and weight, and the source (name, race and base race with path and id, level, alignment, whether a player, file) with `taken_at`, across saves (`query_source_*`, `query_part_id`). Short: «Corazón de Lobo».

A perishable part (heads, hands, hearts, tails) rots in `BODY_PART_FRESH_TIME` seconds of play, charged when its state is read, like food: offline hours do not count. A part off a rotting corpse starts as fresh as the corpse was. `query_freshness()`, `query_rotten()`; the long says fresh, smelling or rotten. Rotten parts stay. `query_value()` is the base value grown with the level of the dead, cut by freshness (never below `BODY_PART_ROTTEN_WORTH` percent, never below 1). Skins, teeth, fangs, claws, horns and mandibles never rot.
