/* 
 * Guldan'98
 * Iolo'02
 * neverbot'03 completely redone for CcMud
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
 * New statistics, similar to D&D 3rd ed, neverbot 6/03
 * 4: armour bonus (AC) 
 * 5: max dex bonus (a negative number means no max)
 * 6: armour malus (for skills)
 * 7: spell malus (fail chance (%))
 *
 * 8:  bonus against slashing weapons
 * 9:  bonus against blunt weapons
 * 10: bonus against piercing weapons
 *     these values can be negative!!
 *
 * 11: type: 1: body armour, 2: shield, 3: helmet, 4: shoes, 5: amulet 
 *           6: cape, 7: ring, 8: gloves, 9: bracelets, 10: belt,
 *          11: trousers, 12: pendants, 13: containers (bag, quiver, etc)
 * 12: material: 1 = wood, 2 = metal, 3 = leather, 4 = cloth
 * 13: body localization (like the localization system by Iolo@Rl)
 * 14: default single/plural: 0 single, 1 plural (shoes, trousers, gloves, etc)
 *
 */

#include <translations/armour.h>
#include <translations/races.h>

mixed * lookup_armour_data(string name) 
{
  switch (name)
  {
    //   cost, cost enchant, weight, size
    //                    ac, max dex bon, skill malus, spell malus
    //                                         ac against (slash, blunt, pierce)
    //                                                            type, material, body localization

    // type 1: chest
    // leather
    case SHIRT:
      return ({ 10, 1000, 10, 6,       0,-1, 0, 0,     0, 0, 0,   1, 4, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case TUNIC:
      return ({ 20, 3000, 50, 20,      0,-1, 0, 0,     0, 0, 1,   1, 4, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case LEATHER: // cuero
      return ({ 70, 3000, 150, 15,     2, 6, 0, 10,    0, 1, 0,   1, 3, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case PADDED_LEATHER: // acolchada
      return ({ 100, 3000, 100, 15,    1, 8, 0, 5,     0, 2,-1,   1, 3, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case FUR: // piel
      return ({ 100, 4000, 300, 15,    3, 4,-3, 20,    0, 0, 0,   1, 3, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case SPLINT_MAIL: // bandas
      return ({ 500, 6000, 400, 15,    2, 5, 0, 10,    0, 1, 0,   1, 3, _LANG_RACES_LOCATIONS_CHEST, 0 });

    // metal
    case RING_MAIL: // anillas
      return ({ 500, 4000, 300, 17,    4, 4,-2, 20,    2, 0, -1,  1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case STUDDED_LEATHER: // cuero tachonado
      return ({ 300, 4000, 250, 15,    3, 5,-1, 15,    0, 1, 0,   1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case SCALE_MAIL: // escamas
      return ({ 500, 4000, 400, 17,    4, 3,-4, 25,    1, 0, 0,   1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case BRIGANDINE: // brigantina
      return ({ 600, 4000, 350, 17,    4, 3,-4, 25,    1, 0, 0,   1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case CHAIN_MAIL: // mallas
      return ({ 750, 6000, 400, 17,    5, 2,-5, 30,    3, 0, -1,  1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case BANDED_MAIL: // bandas y anillas
      return ({ 2000, 7000, 350, 18,   5, 3,-4, 35,    1, -1, 0,  1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case PLATE_MAIL: // placas
      return ({ 5000, 8000, 500, 20,   7, 0,-7, 40,    1, -1, 0,  1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case FIELD_PLATE: // campaña
      return ({ 5000, 9000, 600, 20,   7, 0,-7, 40,    2, -1, 1,  1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });
    case FULL_PLATE: // completa
      return ({ 8000, 10000, 700, 20,  8, 1,-6, 35,    2, -1, 1,  1, 2, _LANG_RACES_LOCATIONS_CHEST, 0 });

    // type 3: head
    case COWL: // capucha, gorro
      return ({ 100, 500, 15, 2,       0,-1, 0, 0,     0, 0, 0,   3, 4, _LANG_RACES_LOCATIONS_HEAD, 0 });
    case BONNET: // cofia
      return ({ 100, 1000, 40, 4,      1,-1, 0, 5,     1, 1, -1,  3, 3, _LANG_RACES_LOCATIONS_HEAD, 0 });
    case HELMET: // yelmo abierto al rostro
      return ({ 500, 1500, 50, 5,      2,-1, 0, 10,    1, -1, 0,  3, 2, _LANG_RACES_LOCATIONS_HEAD, 0 });
    case BASINET: // bacinete, yelmo cerrado al rostro
      return ({ 500, 1500, 60, 5,      3,-1,-1, 15,    1, -1, 0,  3, 2, _LANG_RACES_LOCATIONS_HEAD, 0 });
    case GREAT_HELMET: // gran yelmo
      return ({ 1500, 2000, 100, 6,    4,-1,-1, 20,    1, -1, 0,  3, 2, _LANG_RACES_LOCATIONS_HEAD, 0 });

    // type 4:
    case SHOES:
      return ({ 20, 1000, 5, 2,        1,-1, 0, 0,     0, 0, 0,   4, 3, _LANG_RACES_LOCATIONS_FOOT, 1 });
    case BOOTS:
      return ({ 50, 1000, 10, 3,       2,-1, 0, 0,     0, 0, 0,   4, 3, _LANG_RACES_LOCATIONS_FOOT, 1 });
    case SLIPPERS:
      return ({ 20, 1000, 5, 2,        1,-1, 0, 0,     0, 0, 0,   4, 4, _LANG_RACES_LOCATIONS_FOOT, 1 });

    // type 5:
    case AMULET:
    case NECKLACE:
      return ({ 10, 1000, 5, 1,        0,-1, 0, 0,     0, 0, 0,   5, 2, _LANG_RACES_LOCATIONS_NOTHING, 0 });

    // type 6:
    case CAPE:
      return ({ 50, 1000, 10, 10,      0,-1,-1, 5,     0, 0, 0,   6, 4, _LANG_RACES_LOCATIONS_NOTHING, 0 });
    case CLOAK:
      return ({ 50, 1000, 15, 11,      1,-1,-1, 5,     0, 0, 0,   6, 3, _LANG_RACES_LOCATIONS_NOTHING, 0 });

    // type 7: 
    case RING:
      return ({ 10, 1000, 5, 1,        0,-1, 0, 0,     0, 0, 0,   7, 2, _LANG_RACES_LOCATIONS_NOTHING, 0 });

    // type 8: 
    case GLOVES:
      return ({ 30, 1000, 5, 2,        0,-1, 0, 0,     0, 0, 0,   8, 3, _LANG_RACES_LOCATIONS_HAND, 1 });
    case GAUNTLETS:
      return ({ 100, 1000, 15, 3,      1,-1, 0, 20,    0, 0, 0,   8, 2, _LANG_RACES_LOCATIONS_HAND, 1 });

    // type 9:
    case BRACERS:
      return ({ 100, 1000, 10, 2,      1,-1, 0, 0,     0, 0, 0,   9, 3, _LANG_RACES_LOCATIONS_ARM, 1 });

    // type 10:
    case BELT:
      return ({ 20, 1000, 5, 2,        0,-1, 0, 0,     0, 0, 0,   10, 3, _LANG_RACES_LOCATIONS_NOTHING, 0 });

    // type 11:
    case TROUSERS:
      return ({ 20, 1000, 10, 6,       1,-1, 0, 0,     0, 0, 0,   11, 4, _LANG_RACES_LOCATIONS_LEG, 1 });

    // type 12:
    case PENDANT:
      return ({ 10, 1000, 5, 1,        0,-1, 0, 0,     0, 0, 0,   12, 2, _LANG_RACES_LOCATIONS_NOTHING, 0 });
      
    // type 13: containers
    case BAGPACK:
      return ({ 100, 1000, 10, 6,      0,-1, 0, 0,     0, 0, 0,   13, 3, _LANG_RACES_LOCATIONS_NOTHING, 0 });
    case QUIVER:
      return ({ 100, 1000, 10, 6,      0,-1, 0, 0,     0, 0, 0,   13, 3, _LANG_RACES_LOCATIONS_NOTHING, 0 });
  }
  return ({});
}

// aux functions from /obj/armour.c, neverbot 6/03
string query_armour_type_name(int armour_type) 
{
  switch (armour_type)
  {
    case 1:
      return ARMOUR_TYPE_BODY;
    case 2:
      return ARMOUR_TYPE_SHIELD;
    case 3:
      return ARMOUR_TYPE_HELM;
    case 4:
      return ARMOUR_TYPE_BOOT;
    case 5:
      return ARMOUR_TYPE_AMULET;
    case 6:
      return ARMOUR_TYPE_CAPE;
    case 7:
      return ARMOUR_TYPE_RING;
    case 8:
      return ARMOUR_TYPE_GLOVE;
    case 9: 
      return ARMOUR_TYPE_BRACER;
    case 10: 
      return ARMOUR_TYPE_BELT;
    case 11:
      return ARMOUR_TYPE_TROUSER;
    case 12:
      return ARMOUR_TYPE_PENDANT;
    case 13:
      return ARMOUR_TYPE_MISC;
      
    default:
      return ARMOUR_TYPE_UNKNOWN;
  }
}

// returned in order of armour_type (1 to 13)
string * query_armour_types() {
  return ({
    ARMOUR_TYPE_BODY, ARMOUR_TYPE_SHIELD, ARMOUR_TYPE_HELM, ARMOUR_TYPE_BOOT, 
    ARMOUR_TYPE_AMULET, ARMOUR_TYPE_CAPE, ARMOUR_TYPE_RING, ARMOUR_TYPE_GLOVE, 
    ARMOUR_TYPE_BRACER, ARMOUR_TYPE_BELT, ARMOUR_TYPE_TROUSER, ARMOUR_TYPE_PENDANT, 
    ARMOUR_TYPE_MISC, 
  });
}

// used from /lib/living/equip.c and maybe other places, to sort
// lists of armour pieces in order
int sort_armours(object s1, object s2)
{
  if (s1->query_armour_type() > s2->query_armour_type())
    return 1;
  if (s1->query_armour_type() < s2->query_armour_type())
    return -1;
  return 0;
}
