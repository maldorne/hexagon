
#define MATERIAL_TABLE "/lib/tables/materials.c"

// Material identifiers.
//
// Concatenable, ":"-separated hierarchy. The base is a coarse
// material; deeper segments specify the subtype:
//
//   "wood"
//   "wood:oak"
//   "metal:steel:damascus"
//
// Consumers ask the table for strings keyed by the full id; if no
// entry exists for "wood:oak" the table walks up — "wood:oak" →
// "wood" → UNKNOWN — and returns the first hit. This keeps the table
// sparse: only define entries for the subtypes that need their own
// localised name.

// broad categories
#define ANIMAL "animal"
#define VEGETAL "vegetal"
#define MINERAL "mineral"

// base materials
#define WOOD "wood"
#define METAL "metal"
#define STONE "stone"
#define LEATHER "leather"
#define CLOTH "cloth"
#define CRYSTAL "crystal"

#define UNKNOWN "unknown"
