/* 
 * Guldan'98, Iolo'02, neverbot'03 completely redone for CcMud
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
 *
 */

#include <item/armour.h>

mixed * lookup_armour_data(string name) 
{
  switch (name)
  {
    // type  1:
    // leather
    case SHIRT:
      return ({  10,  1000,  10, 6,      0,-1, 0, 0,      0, 0, 0,   1, 4, "el pecho" });

    case TUNIC:
      return ({  20,  3000,  50, 20,    0,-1, 0, 0,      0, 0, 1,   1, 4, "el pecho" });

    case LEATHER: // Cuero
      return ({  70,  3000, 150, 15,   2, 6, 0, 10,    0, 1, 0,   1, 3, "el pecho" });

    case PADDED_LEATHER: // Acolchada
      return ({  100,  3000, 100, 15,   1, 8, 0, 5,      0, 2,-1,   1, 3, "el pecho" });

    case FUR: // Piel
      return ({  100,  4000, 300, 15,   3, 4,-3, 20,    0, 0, 0,   1, 3, "el pecho" });

    case SPLINT_MAIL: // 
      return ({  500,  6000, 400, 15,   2, 5, 0, 10,    0, 1, 0,   1, 3, "el pecho" });

    // metal
    case RING_MAIL: // Anillas
      return ({  500,  4000, 300, 17,   4, 4,-2, 20,    2, 0, -1,   1, 2, "el pecho" });

    case STUDDED_LEATHER: // Tachonado
      return ({  300,  4000, 250, 15,   3, 5,-1, 15,    0, 1, 0,   1, 2, "el pecho"});

    case SCALE_MAIL: // Escamas
      return ({  500,  4000, 400, 17,   4, 3,-4, 25,    1, 0, 0,   1, 2, "el pecho"});

    case BRIGANDINE: // Brigantina
      return ({  600,  4000, 350, 17,   4, 3,-4, 25,    1, 0, 0,   1, 2, "el pecho" });

    case CHAIN_MAIL: // Mallas
      return ({  750,  6000, 400, 17,   5, 2,-5, 30,    3, 0, -1,   1, 2, "el pecho" });

    //case BRONZE_PLATE: // placas bronce
    //  return ({ 3000,  7000, 450, 20,   5, 3,-4, 35,    1, -1, 0,   1, 2, "el pecho" });

    case BANDED_MAIL: // Bandas
      return ({ 2000,  7000, 350, 18,   5, 3,-4, 35,    1, -1, 0,   1, 2, "el pecho" });

    case PLATE_MAIL: // placas
      return ({ 5000,  8000, 500, 20,   7, 0,-7, 40,    1, -1, 0,   1, 2, "el pecho"});

    case FIELD_PLATE: // campaña
      return ({ 5000,  9000, 600, 20,   7, 0,-7, 40,    2, -1, 1,   1, 2, "el pecho" });

    case FULL_PLATE: // completa
      return ({ 8000, 10000, 700, 20,   8, 1,-6, 35,    2, -1, 1,   1, 2, "el pecho" });

    // type  3:
    case COWL: // Capucha, Gorro
      return ({  100,   500,  15, 2,     0,-1, 0, 0,      0, 0, 0,   3, 4, "la cabeza" });

    case BONNET: // Cofia
      return ({  100,  1000,  40, 4,    1,-1, 0, 5,      1, 1, -1,   3, 3, "la cabeza" });

    case HELMET: // Yelmo abierto al rostro
      return ({  500,  1500,  50, 5,    2,-1, 0, 10,    1, -1, 0,   3, 2, "la cabeza" });

    case BASINET: // Bacinete, Yelmo cerrado al rostro
      return ({  500,  1500,  60, 5,    3,-1,-1, 15,    1, -1, 0,   3, 2, "la cabeza"});

    case GREAT_HELMET: // Gran yelmo
      return ({  1500,  2000, 100, 6,    4,-1,-1, 20,    1, -1, 0,   3, 2, "la cabeza" });

    // type  4:
    case SHOES:
      return ({  20,  1000,   5, 2,   1,-1, 0, 0,      0, 0, 0,   4, 3, "un pie" });
    case BOOTS:
      return ({  50,  1000,  10, 3,    2,-1, 0, 0,      0, 0, 0,   4, 3, "un pie" });
    case SLIPPERS:
      return ({  20,  1000,   5, 2,   1,-1, 0, 0,      0, 0, 0,   4, 4, "un pie" });

    // type  5:
    case AMULET:
    case NECKLACE:
      return ({  10,  1000,   5, 1,    0,-1, 0, 0,      0, 0, 0,   5, 2, "nada" });

    // type  6:
    case CAPE:
      return ({  50,  1000,  10, 10,   0,-1,-1, 5,      0, 0, 0,   6, 4, "nada" });
    case CLOAK:
      return ({  50,  1000,  15, 11,   1,-1,-1, 5,      0, 0, 0,   6, 3, "nada" });

    // type  7: 
    case RING:
      return ({  10,  1000, 5, 1,     0,-1, 0, 0,      0, 0, 0,   7, 2, "nada" });

    // type  8: 
    case GLOVES:
      return ({  30,  1000, 5, 2,     0,-1, 0, 0,      0, 0, 0,   8, 3, "una mano" });

    case GAUNTLETS:
      return ({  100,  1000,  15, 3,   1,-1, 0, 20,    0, 0, 0,   8, 2, "una mano"});

    // type  9:
    case BRACERS:
      return ({  100,  1000,  10, 2,   1,-1, 0, 0,      0, 0, 0,  9, 3, "un brazo" });

    // type  10:
    case BELT:
      return ({  20,  1000,   5, 2,    0,-1, 0, 0,      0, 0, 0,   10, 3, "nada" });

    // type 11:
    case TROUSERS:
      return ({  20,  1000,  10, 6,   1,-1, 0, 0,      0, 0, 0,   11, 4, "una pierna" });

    // type 12:
    case PENDANT:
      return ({  10,  1000,   5, 1,   0,-1, 0, 0,      0, 0, 0,   12, 2, "nada" });
      
    // type 13: containers
    case BAGPACK:
      return ({  100, 1000,  10, 6,     0,-1, 0, 0,     0, 0, 0,  13, 3, "nada" });
    case QUIVER:
      return ({  100, 1000,  10, 6,     0,-1, 0, 0,     0, 0, 0,  13, 3, "nada" });
  }
  return ({});
}

// aux functions from /obj/armour.c, neverbot 6/03
string query_armour_type_name(int armour_type) 
  {
  switch (armour_type)
    {
    case 1:
      return "armadura corporal";
    case 2:
      return "escudo";
    case 3:
      return "casco";
    case 4:
      return "botas";
    case 5:
      return "amuleto"; 
    case 6:
      return "capa"; 
    case 7:
      return "anillo"; 
    case 8:
      return "guante"; 
    case 9: 
      return "brazalete";
    case 10: 
      return "cinturón";
    case 11:
      return "pantalón";
    case 12:
      return "pendiente";
    case 13:
      return "equipo";
      
    default:
      return "desconocido";
    }
}

