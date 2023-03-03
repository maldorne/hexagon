/*
 * neverbot 06/03 completely redone for CcMud
 *  
 * The returned value is:
 *
 * 0: cost in basic value coins (always positive)
 * 1: cost in basic value coins for enchanted +1
 *    costs are always for standar size
 *
 * 2: weight in standard units (tenths of pounds), 
 *    weights are always for standar size
 * 3: size (int), 1 for xxs (like a ring) and 20 for XXL (full armour)
 *
 * 4: attack type, 1 slashing, 2 blunt and 3 pearcing
 * 5: number of damage dice
 * 6: type of damage dice
 * 7: bonus to the dice (xd + y)
 *
 * 8: default material: 1 = wood, 2 = metal, 3 = leather, 4 = cloth
 * 9: base difficulty
 * 10: max dex bonus while using this weapon (a negative number means no max)
 * 11: damage multiplier on critical hit
 * 12: weapon type (to check mastery)
 *
 * More needed info (I don't how these did not exist before)
 * 13: needed hands (1 or 2)
 * 14: default number of attacks (usually 1, two in the case of some exotic two-handed weapons
 *     (two-handed staff, etc)
 */

#include <translations/weapon.h>
#include <translations/combat.h>
#include <living/combat.h>
// #include <resistances.h>
// #include <attacks.h>
// #include <ranged.h>

// return every mastery type
// If it's not here, you cannot learn it
// Corresponds with the 12th column in the lookup table (query_weapon_family to the object)
string * query_weapon_masteries() { return WEAPON_MASTERIES; }

mixed * lookup_weapon_data(string name)
{  
  mixed * weapon_data;
  weapon_data = ({ });

  switch(name)
  {
    //          cost, cost, weight, size 
    //                                  damage,  x D y + z
    //                                                    material, difficulty, max dex, multiplier, family
    //                                                                              hands, attacks

    // very small simple weapons
    case KNIFE: // cuchillo
      weapon_data = ({ 10, 100, 10,2,       SLASHING, 1, 3, 0,   2, 0,-1, 2, DAGGER_MASTERY,     1 ,1 }); break;
    case DAGGER: // daga
      weapon_data = ({ 20, 200, 10,2,       PIERCING, 1, 4, 0,   2, 0,-1, 2, DAGGER_MASTERY,     1 ,1 }); break;

    // small simple weapons
    case CLUB: // porra
      weapon_data = ({ 50, 500, 60,3,       BLUNT,    1, 6, 0,   2, 0,-1, 2, MACE_MASTERY,       1 ,1 }); break;
    case LIGHT_MACE: // maza
      weapon_data = ({ 50, 500, 60,3,       BLUNT,    1, 6, 0,   2, 0,-1, 2, MACE_MASTERY,       1 ,1 }); break;
    case PICK: // pico
      weapon_data = ({ 40, 400, 40,8,       PIERCING, 1, 6, 0,   2, 2, 4, 4, PICK_MASTERY,       2 ,1 }); break;
    case SHOVEL: // pala
      weapon_data = ({ 40, 400, 40,8,       BLUNT,    1, 6, 0,   2, 1, 4, 2, MACE_MASTERY,       2 ,1 }); break;

    // medium simple weapons
    case HEAVY_MACE:
      weapon_data = ({ 120,1200,120,5,      BLUNT,    1, 8, 0,   2, 0,-1, 2, MACE_MASTERY,       2 ,1 }); break;
    case JAVELIN: // Media lanza o jabalina
      weapon_data = ({ 10, 100, 30,6,       PIERCING, 1, 6, 0,   1, 1, 6, 3, SPEAR_MASTERY,      1 ,1 }); break;
    case STAFF:
      weapon_data = ({ 5, 50, 30,7,         BLUNT,    1, 6, 0,   1, 0,-1, 2, STAFF_MASTERY,      2 ,1 }); break;

    // big simple weapons
    case LIGHT_SPEAR: // lanza ligera
      weapon_data = ({ 20, 200, 30,9,       PIERCING, 1, 8, 0,   2, 1, 6, 3, SPEAR_MASTERY,      2 ,1 }); break;

    // small martial weapons
    case SHORT_SWORD:
      weapon_data = ({ 30, 300,30,6,        PIERCING, 1, 6, 0,   2, 0,-1, 2, SHORT_SWORD_MASTERY, 1 ,1 }); break;
    case THROWING_AXE:
      weapon_data = ({ 80, 800, 40,6,       SLASHING, 1, 6, 0,   2, 1, 6, 2, AXE_MASTERY,        1 ,1 }); break;
    case HAND_AXE:
      weapon_data = ({ 60, 600, 50,6,       SLASHING, 1, 6, 0,   2, 0,-1, 3, AXE_MASTERY,        1 ,1 }); break;
    case LIGHT_HAMMER:
      weapon_data = ({ 10, 100, 20,9,       BLUNT,    1, 4, 0,   2, 0,-1, 2, HAMMER_MASTERY,     1 ,1 }); break;

    // medium martial weapons
    case SCIMITAR:
      weapon_data = ({ 150, 1500, 40, 10,   SLASHING, 1, 6, 0,   2, 1,-1, 2, LONG_SWORD_MASTERY, 1 ,1 }); break;
    case LONG_SWORD:
      weapon_data = ({ 150, 1500, 40, 10,   SLASHING, 1, 8, 0,   2, 0,-1, 2, LONG_SWORD_MASTERY, 1 ,1 }); break;
    case RAPIER:
      weapon_data = ({ 200, 2000, 30, 10,   PIERCING, 1, 6, 0,   2, 1, 5, 2, LONG_SWORD_MASTERY, 1 ,1 }); break;
    case BATTLE_AXE: // hacha de batalla
      weapon_data = ({ 100, 1000, 70, 12,   SLASHING, 1, 8, 0,   2, 0,-1, 3, AXE_MASTERY,        1 ,1 }); break;
    case LONG_SPEAR: // lanza larga
      weapon_data = ({ 100, 1000, 100, 15,  PIERCING, 1, 8, 0,   2, 2, 4, 3, SPEAR_MASTERY,      2 ,1 }); break;
    case WAR_HAMMER: // martillo de guerra
      weapon_data = ({ 120, 1200, 80, 10,   BLUNT,    1, 8, 0,   2, 0,-1, 3, HAMMER_MASTERY,     2 ,1 }); break;
    case HEAVY_PICK:
      weapon_data = ({ 80, 800, 60, 8,      PIERCING, 1, 6, 0,   2, 1, 5, 4, PICK_MASTERY,       1 ,1 }); break;
    case TRIDENT:
      weapon_data = ({ 15, 1500, 50, 10,    PIERCING, 1, 8, 0,   2, 1, 6, 2, SPEAR_MASTERY,      2 ,1 }); break;

    // big martial weapons
    case HALBERD:
      weapon_data = ({ 100, 1000, 150, 15,  SLASHING, 1, 10, 0,  2, 1, 6, 3, SPEAR_MASTERY,      2 ,1 }); break;
    case CUTLASS:  // alfanje, cimitarra grande
      weapon_data = ({ 200, 2000, 160, 13,  SLASHING, 2, 4, 0,   2, 1, 6, 2, TWO_HANDED_SWORD_MASTERY, 2 ,1 }); break;
    case BROADSWORD:
      weapon_data = ({ 50, 500, 150, 13,    SLASHING, 2, 6, 0,   2, 0,-1, 2, TWO_HANDED_SWORD_MASTERY, 2 ,1 }); break;
    case HEAVY_AXE:
      weapon_data = ({ 20, 200, 200, 13,    SLASHING, 1, 12, 0,  2, 1, 6, 3, AXE_MASTERY,        2 ,1 }); break;
    case HEAVY_SPEAR:  // lanza larga
      weapon_data = ({ 50, 500, 90, 17,     PIERCING, 1, 8, 0,   2, 2, 4, 3, SPEAR_MASTERY,      2 ,1 }); break;
    case PIKE:  // pica
      weapon_data = ({ 50, 500, 90, 17,     PIERCING, 1, 8, 0,   2, 2, 4, 3, SPEAR_MASTERY,      2 ,1 }); break;
  
    // small exotic weapons
    case SICKLE: // hoz
      weapon_data = ({ 60, 600, 30,2,       SLASHING, 1, 6, 0,   2, 1, 6, 2, SCYTHE_MASTERY,     1 ,1 }); break;
    case LIGHT_FLAIL: // mangual ligero
      weapon_data = ({ 80, 800, 50, 10,     BLUNT,    1, 8, 0,   2, 2, 4, 2, FLAIL_MASTERY,      1 ,1 }); break;

    // medium exotic weapons
    case TWO_HANDED_STAFF: // baston a dos manos
      weapon_data = ({ 50, 500, 50,  16,    BLUNT,    1, 8, 0,   1, 1, 6, 2, STAFF_MASTERY,      2 ,2 }); break;
    case BASTARD_SWORD: // espada bastarda
      weapon_data = ({ 350, 3500, 10, 12,   SLASHING, 1, 10, 0,  2, 0,-1, 2, TWO_HANDED_SWORD_MASTERY, 2 ,1 }); break;
    case HOOKED_HAMMER: // martillo ganchudo 
      weapon_data = ({ 200, 2000, 60, 9,    BLUNT,    1, 4, 0,   2, 1, 5, 3, HAMMER_MASTERY,     1 ,2 }); break;

    // big exotic weapons
    case WHIP: // latigo
      weapon_data = ({ 50, 500, 20, 9,      SLASHING, 1, 2, 2,   3, 2, 4, 2, WHIP_MASTERY,       1 ,1 }); break;
    case CHAIN: // cadena 
      weapon_data = ({ 250, 2500, 150, 15,  PIERCING, 2, 4, 0,   2, 2, 4, 2, WHIP_MASTERY,       2 ,2 }); break;
    case TWO_HANDED_SWORD: // espada a dos manos
      weapon_data = ({ 1000,10000,150, 16,  SLASHING, 1, 8, 0,   2, 2, 4, 2, TWO_HANDED_SWORD_MASTERY, 2 ,2 }); break;
    case SCYTHE:
      weapon_data = ({ 18, 180, 120, 13,    PIERCING, 2, 4, 0,   2, 2, 4, 4, SCYTHE_MASTERY,     2 ,1 }); break;
    case HEAVY_FLAIL: // mangual pesado
      weapon_data = ({ 150, 1500, 200, 13,  BLUNT,    1, 10, 0,  2, 2, 4, 2, FLAIL_MASTERY,      2 ,1 }); break;
  }

  return weapon_data;
}
