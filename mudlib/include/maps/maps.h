// Worldmap coordinate convention (fixed for every game, every map):
//
//   +x = east        -x = west
//   +y = north       -y = south
//   +z = up          -z = down
//
// Diagonals compose the two component axes (e.g. northeast = +x +y).
// This is the standard cartesian convention. It is NOT per-game
// configurable — the renderer is free to flip an axis on the way out
// (e.g. an ASCII map drawing y growing downward on screen), but the
// underlying world coordinates always follow the table above.

// Short name — resolved by handler() at /lib/handlers/worldmap (with
// per-game override at /games/<game>/handlers/worldmap if present).
#define WORLDMAP_HANDLER "worldmap"
