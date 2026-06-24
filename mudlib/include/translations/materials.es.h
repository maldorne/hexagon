
// materials.es.h — cadenas en castellano para el catálogo de materiales.
//
// Dos formas por material:
//   _LANG_MATERIAL_<X>_PHRASE  — forma "de X" para pegarla a un sustantivo
//                                ("silla de madera", "altar de piedra").
//   _LANG_MATERIAL_<X>_NAME    — sustantivo puro ("madera", "piedra").
//
// Consumidos por /lib/tables/materials.c::query_material_{phrase,name}.

#ifndef _LANG_MATERIALS_H_ES
#define _LANG_MATERIALS_H_ES

#define _LANG_MATERIAL_UNKNOWN_PHRASE   "de material desconocido"
#define _LANG_MATERIAL_UNKNOWN_NAME     "desconocido"

#define _LANG_MATERIAL_WOOD_PHRASE      "de madera"
#define _LANG_MATERIAL_WOOD_NAME        "madera"

#define _LANG_MATERIAL_METAL_PHRASE     "de metal"
#define _LANG_MATERIAL_METAL_NAME       "metal"

#define _LANG_MATERIAL_STONE_PHRASE     "de piedra"
#define _LANG_MATERIAL_STONE_NAME       "piedra"

#define _LANG_MATERIAL_LEATHER_PHRASE   "de cuero"
#define _LANG_MATERIAL_LEATHER_NAME     "cuero"

#define _LANG_MATERIAL_CLOTH_PHRASE     "de tela"
#define _LANG_MATERIAL_CLOTH_NAME       "tela"

#define _LANG_MATERIAL_CRYSTAL_PHRASE   "de cristal"
#define _LANG_MATERIAL_CRYSTAL_NAME     "cristal"

#endif // _LANG_MATERIALS_H_ES
