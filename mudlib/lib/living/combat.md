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
