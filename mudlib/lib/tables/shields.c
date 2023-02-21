/*
 * shield_table.c, reviewed for CcMud, neverbot 6/03
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
 * 4: material: 1 = wood, 2 = metal, 3 = leather, 4 = cloth
 * 5: armour bonus (AC) 
 * 6: spell malus (fail chance (%))
 * 7: skill malus
 * 8: two-handed?
 *
 */

#include <translations/shield.h>

int * lookup_shield_data (string name)
{
  switch (name)
  {	
    case ROUNDEL:	
    case BUCKLER:	
      return ({ 150, 1500, 50, 6,     2, 1, 5, -1, 0 });

    case SMALL_WOODEN_SHIELD: // Escudo pequeño de madera
      return ({ 30, 300, 50, 8,      	1, 1, 5, -1, 0 });
    case SMALL_METAL_SHIELD: // Escudo pequeño de acero	
      return ({ 90, 900, 60, 8,   	  2, 2, 5, -1, 0 });
    case HEAVY_WOODEN_SHIELD: // Escudo grande de madera
      return ({ 70, 700, 100, 11, 	  1, 3, 15, -2, 0 });
    case HEAVY_METAL_SHIELD: // Escudo grande de acero
      return ({ 200, 2000, 150, 11, 	2, 4, 15, -2, 0 });

    case PAVISE: // Escudo paves (corporal a dos manos)
      return ({ 300, 3000, 450, 20, 	2, 5, 50,-10, 1 });
  }
  
  return ({});
}
