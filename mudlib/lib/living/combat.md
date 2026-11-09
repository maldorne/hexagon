# Combat

Every attack of a weapon (`weapon_attack` in `weapon.c`; unarmed combat in
`living/unarmed_combat.c` follows the same rolls) is resolved on its own.

## The rolls

1. **Fumble**: `1d200 < 10` (9 in 200) ends the attack with no damage.
2. **To hit**: the attack hits if `WC + 1d10 - AC >= 1`.
   - `WC` (`query_total_wc`): dex bonus + `query_tohit_bonus` (strength bonus + temporary bonus) +
     the class `combat_bonus` + for each wielded weapon its enchant minus its
     `difficulty` + mastery / 10 (unarmed: unarmed ability / 10) + level / 5.
   - `AC` (`query_total_ac(type)`): 10 (5 if passed out) + equipment ac for that type
     (held + worn) + body ac + constitution bonus + 1 for small races, plus the
     armour skill malus (a negative number, `query_skill_malus`).
   - Outnumbering: if the defender has 2 or more attackers than the attacker,
     the AC is divided by `(difference + 2) / 2`.
3. **Damage** (on a hit): `dice + weapon enchant + query_damage_bonus + strength
   bonus`, at least 1. A critical (`1d200 < 10`) multiplies it by the weapon's
   `multiplier`. The location hit scales it (`recalc_damage`).
   The hit message ends with the wound it made (`lib/handlers/weapons.c`):
   always on a critical, a killing blow or a quarter or more of the victim's
   hit points; on smaller hits only one time in three.
4. **Armour absorption**: the piece worn on the hit location absorbs a
   percentage of the damage and loses condition.
   - Physical: `query_ac_against(type) * 10` percent (cap 100), where the value is
     the piece's ac plus its bonus against slashing, blunt or piercing.
   - Elemental: the piece's base ac * 10 percent, plus the piece's own resistance
     property for that element (`query_property(<damage type id>)`), clamped to
     [-100, 100]. A negative resistance is a weakness: the damage grows.

Equipment ac counts to hit for every type. For an elemental type it is the sum
of the base ac of what is worn and held, without any per-type bonus (those only
exist for slashing, blunt and piercing).

## Damage types

A damage type is an int for the rolls (`SLASHING` 0 to `CHAOS` 19) and an
English id string for everything that is stored or looked up by name
(`DAMAGE_SLASHING`, `DAMAGE_FIRE`...), both in `include/living/damage-types.h`.
`DAMAGE_PHYSICAL_TYPES` and `DAMAGE_ELEMENTAL_TYPES` list the ids in the order
of the ints (`DAMAGE_ELEMENTAL_TYPES[type - sizeof(AC_TYPES)]`).

The ids are the keys of the resistances of a living (`set_resistance`,
`query_resistance`, `adjust_tmp_resistance`), of the resistance properties of
an item, of the type passed to `spell_damage` and `skill_damage`, and of the
`resistances` field of an NPC template. They are never translated. The name a
player reads is `table("damage-types")->query_damage_type_name(id)`
(`query_damage_type_id(name)` goes the other way, for what a player types); the
names live in `include/translations/combat.<lang>.h`, and a weapon gives the
name of one of its attacks with `query_attack_type_name`
(`query_attack_type_id` is the id).

A savefile written when the keys were the translated names is migrated when it
is restored (`migrate_legacy_keys` of the table): the properties of an item in
`init_auto_load_attributes`, the resistances of a living in
`migrate_resistance_keys`, which the restore of a player and of an NPC call.

## Dodge and parry

A miss is shown as a dodge, a parry or a plain miss, but this is narrative only:
the attack was already decided by the to-hit roll. A real dodge or parry roll
does not exist yet.

## Elemental damage: attacks and extra damage

- `add_attack(name, type, dice, faces, bonus)` with an elemental type is a
  separate attack: it rolls to hit by itself, against the elemental AC.
- `add_extra_damage(attack, type, dice, faces, bonus)`
  (`remove_extra_damage(attack)`, `query_extra_damage`) adds elemental damage to
  every hit of one physical attack of the weapon, named as in `add_attack`
  (`BASIC_ATTACK_NAME` for the one it comes with); an attack may carry several.
  It does not roll to hit: it lands only with that attack's hit, is not
  multiplied by the critical or the location, is absorbed by the same armour
  piece with the elemental rule, and adds one line to the hit message. The line
  names the weapon by its short and has its own verb for each element. A last
  optional argument gives the weapon its own lines, `({ attacker, defender, room })`,
  with the tokens `$weapon$` (already "Tu <short>" or "<article> <short> de <owner>"),
  `$attacker$` and `$defender$`.
  A spell can give a weapon extra damage for good (`enchant-weapon`, see
  the readme of the spells of the game).
  An optional seventh argument is the sentence detect magic reads for that extra
  damage (without it, the line of the element with the dice, from the weapons
  handler).
  A weapon that holds any magic (extra damage, an enchant or a mark) adds one
  generic line to its look and never says what (`extra_look` of `weapon.c`).
  What a spell puts on a weapon is recorded with
  `add_magic_mark(id, who, level, data)` (`data`: `enchant` points to give back,
  `attack` whose extra damage goes, `property` to remove, `text` for detect
  magic); `dispel_magic(level)` undoes every mark of a lower level and returns
  the ids, and `query_magic_descriptions()` lists the exact sentences.
  A weapon that adds it only against some targets redefines
  `has_extra_damage_against(target)` (default 1): when it answers 0 the extra
  damage of that hit is not rolled nor shown.

## Natural attacks

A creature with no weapon fights with its body, in an unarmed style. The damage
of any unarmed blow is `xDy+z`: the dice and bonus of the style, then
`query_damage_bonus` and the strength bonus (`workout_attack` of
`living/unarmed_combat.c`). Every style, in the table or in a race, is
`({ name, num_dice, dice_size, bonus, message_lists })`; the living keeps the
dice it fights with (`set_damage_dice(num, size, bonus)`, `query_damage_dice`).

The styles of `lib/tables/unarmed-combat.c` are the ones anybody could learn, and
the natural ones that races and templates name
(`NATURAL_ATTACK_*` of `include/living/combat.h`):

| style | dice | style | dice |
|---|---|---|---|
| teeth | 1d4 | horns | 1d8 |
| fangs | 1d6 | hooves | 1d6 |
| claws | 1d6 | tail | 1d4 |
| paws | 1d6 | legs | 1d3 |
| beak | 1d4 | a punch | 1d3 |

**Racial styles** live in the race object (`lib/race.c`): `add_unarmed_style(id,
name, num, size, bonus, message_lists)` adds one that only that race has (a
dragon mixing claws, fangs and tail), with the same query functions as the table
(`style_exists`, `query_unarmed_style_info`, `query_messages`, `set_damage_dice`).
`set_natural_style(id)` says the style the creatures of the race fight with, one
of its own or an id of the table. When a living gets the race (`set_race_ob` of
`living/social.c`) it knows that style at 100 and uses it; when it loses the race
it goes back to its fists. Player races have none. A living asks where a style
lives through `_style_source` of `living/unarmed_combat.c` (its race, then the
table); learning and improving (`adjust_unarmed_ability`) only look at the table,
so a racial style can never be learned.

A template whose race is shared by different beasts (animal, bird, reptile) names
the style of each in the field `unarmed_style` (a table id, or an id of its race).

## Hit locations

Aiming at a body part is an API of the defender (`living/wear.c`), shared by
weapons, unarmed combat, spells and skills:

- `pick_hit_location()` -> `({ multiplier, location, piece })`: the living's
  `obtain_location()` (random location, damage multiplier) plus the worn piece
  covering it, or nil. The locations are always the race's: every monster and
  NPC has a race object that defines its body. `lib/living/body.c` answers for
  the living from its race: `obtain_location()`, `query_locations()` (the
  location names, for debugging) and `query_body_size()`. A living without
  locations gives `({ 1.0, "", nil })`.
- `query_armour_at(location)`: the first worn piece whose `query_localization()`
  is that location, or nil.
- `absorb_hit(piece, type, damage)`: how much of `damage` of that type the piece
  absorbs (physical or elemental rule, see Armour absorption above; elemental can
  be negative for a weakness). It only computes: the caller wears the piece down
  with `piece->hit_item(n)` when it wants to, after deciding the final damage.

A caller scales its damage by the multiplier, subtracts `absorb_hit`, and passes
the location name to its message. Weapons and unarmed combat get the triple
through `recalc_damage`, which returns `({ damage, location, piece })`.

## Tuning

- **Hit more**: `adjust_tmp_tohit_bon(n)` (temporary), the class
  `set_combat_bonus(n)`, a weapon with lower `difficulty` or higher
  `set_enchant(n)`, more strength/dexterity.
- **Be hit less**: `adjust_tmp_ac_bon(n)` (temporary), `adjust_body_ac_bon(n)` or
  `set_body_ac(n)`, armour with more ac (`armour_ac`, `set_enchant`), a lower
  skill malus.
- **Take less damage**: armour on every location, with a high ac against the type
  (`set_ac_bon_against(type, n)` for the physical ones).
- **Deal more damage**: `adjust_tmp_damage_bon(n)`, weapon `set_enchant(n)`, more
  dice in `add_attack`, more strength, a higher weapon `multiplier`.
- **Resist an element**: put the element's id as a property on the armour
  piece (`add_property(DAMAGE_FIRE, n)` with `n` up to 100; negative for a
  weakness), and give the piece base ac.

## Who fights whom, and for how long

Every living keeps two lists in `lib/living/combat.c`: `attacker_list`, the
rivals in its place, and `call_outed`, the rivals who left it and are still
chased (they rejoin the fight when the two meet again). A rival who stays away
for `CHASE_EXPIRY_SECONDS` (`include/living/combat.h`, 5 minutes) is dropped
with the "no longer fighting" message; the check runs on every combat round
and whenever a new blow starts (`attack_ob`, `attack_by`), so a monster whose
heart_beat stopped forgets too. Neither list is saved.

A monster of `GRUDGE_MIN_MONSTER_LEVEL` or more also holds grudges
(`lib/monster/memory.c`): the names of the players of `GRUDGE_MIN_ATTACKER_LEVEL`
or more who fought it, with the time of the last blow. They survive logouts and,
for a persisted NPC, restarts, and the monster attacks such a player on sight.
A grudge runs out `GRUDGE_EXPIRY_SECONDS` (5 minutes) after the last blow, and
ends at once when the monster stops fighting that player (`stop_fight`: death,
peace, the law, an expired chase). The constants are in `include/npc/npc.h`.

## Protecting

A living protected by another (`set_protector`, set by the protect skill,
`lib/obj/skills/protect.c`) may have each blow aimed at it fall on the
protector, when both are in the same place: `query_protect_valid` of the
protector rolls 20 plus its protect ability, 20 less when it is not already
fighting the attacker, at most 95. Every player knows the skill from 50; an NPC
that does not know it rolls with that same starting value. The ability does not
climb with use. A protector who attacks the one it protects stops protecting.

## Fights and the law

`attack_ob` (the attacker's side) calls `report_attack(attacker, victim)` of the
crimes handler the first time the victim enters the attacker's list, so one
report per fight start, not per round. `do_death` calls `report_death`. Both
are in a `catch`; see `lib/handlers/crimes.md` for what counts as assault,
guard assault and murder, and for self-defence.

## Non-lethal guards

`adjust_hp` (`living/health.c`) asks `spare_blow(guard, victim)` of the crimes
handler when a blow leaves a living at 0 hp or less and its dealer is an NPC that
is not in safe combat. Only a guard or responder of a citizenship whose guards do
not kill (`lethal` 0) takes the answer 1, and only against a player or an NPC
citizen that is not its enemy in war. Then the victim stays at 1 hp, the fight
stops on both sides with every guard fighting it, and the victim is passed out
and arrested instead of dying. Every other attacker, and every other victim,
goes through the usual death. See `lib/handlers/crimes.md`.

## Monsters using effects in a fight

`lib/monster/combat.c` gives every monster and NPC a default: each heart beat of
a fight (`combat_effects_heart_beat`), and never while it is casting or under
`noskill`, it tries one of its combat effects (`query_combat_effects`: the spells
and non-passive skills it knows, `query_known_effects`, plus its attack effects).
Attack effects are effect object paths added with `add_attack_effect(path)` (and
`remove_attack_effect`, `query_attack_effects`): what a creature does without
knowing it as a spell or skill, like a bite or a breath, or any spell or skill
given to that creature alone. They need neither guild nor job (`is_attack_effect_of`
in `lib/effects/base_effect.c`: usable, neutral power, cast at the class level of
the creature); a creature-only effect does not need `set_common_effect(1)`.

An effect that should not announce itself when it starts (a bite, a breath) calls `set_silent_start(1)` (`lib/effects/base_effect.c`, off by default): the "begins to cast" lines to the caster and the room and the chant of the spell bases are dropped, and what the effect prints is the first thing read. The creature bases (`breath-spell.c`, `venom-spell.c` of rl-aeternum) set it.

Chances: an effect with a chance of its own (`set_effect_chance(key, percent)`,
or `add_attack_effect(path, percent)`; the key is the full path with `.c` of a
spell or attack effect, or the id of a skill; the template fills them, see
`npc-template`) is tried that percent of the heart beats, rolled by itself. All
the effects without one share a single roll of `NPC_CAST_CHANCE` percent
(`include/npc/npc.h`) and one of them is picked at random, so a monster with no
chances behaves as always. The rolls come first, then `choose_effect()` is offered
only the effects that rolled in favour (`cast_roll`); a direct call to
`choose_effect()` outside the heart beat sees them all. Only effects it can
use now are drawn (`can_use_effect`: an intent other than none, common, one of its attack effects or allowed by its guild or job,
gp enough, none of the effect's lockout properties set). `choose_effect()` puts them
in a context (`attackers`, `usable`, `choice`) and hands it to `combat_decision()`:
a plain monster gives it back untouched, an NPC runs its components' `choose_effect`
chain over it (a healer takes its heals out of `usable`; a component may also set
`choice`). When nobody has chosen, it picks one of `usable` at random and
`aim_effect()` gives its targets by intent and target type: a harmful `one`/`touch` goes at
an attacker, a harmful `many` at all of them, a helpful
`one`/`touch` at itself, a helpful `many` at itself and all the allies fighting the same
attackers; `all`, `self` and `none` are cast as they are. `cast_chosen_effect()`
runs it through `cast_effect_on` (chosen targets) or `cast_effect`, so the effect
makes the same refusals as for a player. The template option `guild_abilities` adds everything the
NPC's guild teaches up to its guild level to what it knows.

## Running away

Each round of a fight, a living with attackers asks `query_wants_to_flee()` and
runs (`run_away()`, a random exit; none, it stays) when it answers yes. The
default is its wimpy: health under `wimpy` percent of its maximum. An NPC asks
its components first through the `flee` chain: each votes `FLEE_VETO`,
`FLEE_ABSTAIN` or `FLEE_FORCE` (`include/npc/npc.h`), a veto wins over
everything and a force over the wimpy, whatever the order they answer in. A
protector vetoes while somebody it protects is fighting there; a responder or an
escort vetoes while on its errand.
