#ifndef EXPLORATION_H
#define EXPLORATION_H

// Per-player exploration state: the areas a player has explored, the travel
// anchors they have discovered, and the encyclopedia articles they have
// collected. Stored per game in its own savefile (see lib/player/exploration.c
// and lib/obj/player/exploration.c), separate from player.o.

#define EXPLORATION_OB    "/lib/obj/player/exploration.c"
#define EXPLORATION_SAVE  "exploration.o"

// Keys of the per-game sub-mapping.
#define EXP_AREAS    "areas"
#define EXP_ANCHORS  "anchors"
#define EXP_ARTICLES "articles"

#endif
