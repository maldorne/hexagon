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
     property for that element (`query_property(<element name>)`), clamped to
     [-100, 100]. A negative resistance is a weakness: the damage grows.

Equipment ac counts to hit for every type. For an elemental type it is the sum
of the base ac of what is worn and held, without any per-type bonus (those only
exist for slashing, blunt and piercing).

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

## Hit locations

Aiming at a body part is an API of the defender (`living/wear.c`), shared by
weapons, unarmed combat, spells and skills:

- `pick_hit_location()` -> `({ multiplier, location, piece })`: the race's
  `obtain_location()` (random location, damage multiplier) plus the worn piece
  covering it, or nil. A living without locations gives `({ 1.0, "", nil })`.
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
- **Resist an element**: put the element's name as a property on the armour
  piece (`add_property(<element name>, n)` with `n` up to 100; negative for a
  weakness), and give the piece base ac.

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

## Monsters casting in a fight

`lib/monster/casting.c` gives every monster and NPC a default: each heart beat of
a fight, with `NPC_CAST_CHANCE` percent (`include/npc/npc.h`), and never while it
is casting or under `noskill`, it tries one spell or skill it knows
(`query_known_effects`: its spells and its non-passive skills). Only effects it can
use now are drawn (`can_use_effect`: an intent other than none, taught by its guild,
gp enough, none of the effect's lockout properties set). `choose_effect()` picks one
at random and its targets by intent and target type: a harmful `one`/`touch` goes at
an attacker, a harmful `many` at all of them, a helpful
`one`/`touch` at itself, a helpful `many` at itself and all the allies fighting the same
attackers; `all`, `self` and `none` are cast as they are. `cast_chosen_effect()`
runs it through `cast_effect_on` (chosen targets) or `cast_effect`, so the effect
makes the same refusals as for a player. An AI component replaces `choose_effect()`
and leaves the rest. The template option `guild_abilities` adds everything the
NPC's guild teaches up to its guild level to what it knows.
