
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
// entry exists for "wood:oak" the table walks up — "wood:oak" ->
// "wood" -> UNKNOWN — and returns the first hit. This keeps the table
// sparse: only define entries for the subtypes that need their own
// localised name.

// broad categories
#define MAT_ANIMAL   "animal"
#define MAT_VEGETAL  "vegetal"
#define MAT_MINERAL  "mineral"

// base materials
#define MAT_WOOD     "wood"
#define MAT_METAL    "metal"
#define MAT_STONE    "stone"
#define MAT_LEATHER  "leather"
#define MAT_CLOTH    "cloth"
#define MAT_CRYSTAL  "crystal"
#define MAT_WEB      "web"

// subtypes
#define MAT_SILK     "cloth:silk"

#define MAT_UNKNOWN  "unknown"
