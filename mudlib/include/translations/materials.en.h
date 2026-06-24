
// materials.en.h — English strings for the materials catalogue.
//
// Two forms per material:
//   _LANG_MATERIAL_<X>_PHRASE  — adjectival form attached to a noun
//                                ("wooden chair", "stone altar").
//   _LANG_MATERIAL_<X>_NAME    — bare noun form ("wood", "stone").
//
// Consumed by /lib/tables/materials.c::query_material_{phrase,name}.

#ifndef _LANG_MATERIALS_H_EN
#define _LANG_MATERIALS_H_EN

#define _LANG_MATERIAL_UNKNOWN_PHRASE   "unknown"
#define _LANG_MATERIAL_UNKNOWN_NAME     "unknown"

#define _LANG_MATERIAL_WOOD_PHRASE      "wooden"
#define _LANG_MATERIAL_WOOD_NAME        "wood"

#define _LANG_MATERIAL_METAL_PHRASE     "metal"
#define _LANG_MATERIAL_METAL_NAME       "metal"

#define _LANG_MATERIAL_STONE_PHRASE     "stone"
#define _LANG_MATERIAL_STONE_NAME       "stone"

#define _LANG_MATERIAL_LEATHER_PHRASE   "leather"
#define _LANG_MATERIAL_LEATHER_NAME     "leather"

#define _LANG_MATERIAL_CLOTH_PHRASE     "cloth"
#define _LANG_MATERIAL_CLOTH_NAME       "cloth"

#define _LANG_MATERIAL_CRYSTAL_PHRASE   "crystal"
#define _LANG_MATERIAL_CRYSTAL_NAME     "crystal"

#endif // _LANG_MATERIALS_H_EN
