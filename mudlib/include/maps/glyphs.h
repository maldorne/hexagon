#ifndef MAPS_GLYPHS_H
#define MAPS_GLYPHS_H

// Box-drawing and map glyphs, defined as chr() byte sequences so this
// header (and its consumers) stay pure ASCII on disk -- no literal
// multi-byte characters that a re-encoding editor or a different OS could
// corrupt. Same convention as the chr() usage in cartography.c.
//
// GLYPH_BOX_<n><s><e><w>: one box char per arm-weight combination, where
// each of N,S,E,W is 0 (no arm), 1 (light/thin) or 2 (heavy/thick).
// Comments are ASCII-only: the Unicode codepoint plus the arm weights.

#define GLYPH_BOX_0000   " "                                            // space  N0S0E0W0
#define GLYPH_BOX_0001   (chr(226) + chr(149) + chr(180))               // U+2574 N0S0E0W1
#define GLYPH_BOX_0002   (chr(226) + chr(149) + chr(184))               // U+2578 N0S0E0W2
#define GLYPH_BOX_0010   (chr(226) + chr(149) + chr(182))               // U+2576 N0S0E1W0
#define GLYPH_BOX_0011   (chr(226) + chr(148) + chr(128))               // U+2500 N0S0E1W1
#define GLYPH_BOX_0012   (chr(226) + chr(149) + chr(190))               // U+257E N0S0E1W2
#define GLYPH_BOX_0020   (chr(226) + chr(149) + chr(186))               // U+257A N0S0E2W0
#define GLYPH_BOX_0021   (chr(226) + chr(149) + chr(188))               // U+257C N0S0E2W1
#define GLYPH_BOX_0022   (chr(226) + chr(148) + chr(129))               // U+2501 N0S0E2W2
#define GLYPH_BOX_0100   (chr(226) + chr(149) + chr(183))               // U+2577 N0S1E0W0
#define GLYPH_BOX_0101   (chr(226) + chr(148) + chr(144))               // U+2510 N0S1E0W1
#define GLYPH_BOX_0102   (chr(226) + chr(148) + chr(145))               // U+2511 N0S1E0W2
#define GLYPH_BOX_0110   (chr(226) + chr(148) + chr(140))               // U+250C N0S1E1W0
#define GLYPH_BOX_0111   (chr(226) + chr(148) + chr(172))               // U+252C N0S1E1W1
#define GLYPH_BOX_0112   (chr(226) + chr(148) + chr(173))               // U+252D N0S1E1W2
#define GLYPH_BOX_0120   (chr(226) + chr(148) + chr(141))               // U+250D N0S1E2W0
#define GLYPH_BOX_0121   (chr(226) + chr(148) + chr(174))               // U+252E N0S1E2W1
#define GLYPH_BOX_0122   (chr(226) + chr(148) + chr(175))               // U+252F N0S1E2W2
#define GLYPH_BOX_0200   (chr(226) + chr(149) + chr(187))               // U+257B N0S2E0W0
#define GLYPH_BOX_0201   (chr(226) + chr(148) + chr(146))               // U+2512 N0S2E0W1
#define GLYPH_BOX_0202   (chr(226) + chr(148) + chr(147))               // U+2513 N0S2E0W2
#define GLYPH_BOX_0210   (chr(226) + chr(148) + chr(142))               // U+250E N0S2E1W0
#define GLYPH_BOX_0211   (chr(226) + chr(148) + chr(176))               // U+2530 N0S2E1W1
#define GLYPH_BOX_0212   (chr(226) + chr(148) + chr(177))               // U+2531 N0S2E1W2
#define GLYPH_BOX_0220   (chr(226) + chr(148) + chr(143))               // U+250F N0S2E2W0
#define GLYPH_BOX_0221   (chr(226) + chr(148) + chr(178))               // U+2532 N0S2E2W1
#define GLYPH_BOX_0222   (chr(226) + chr(148) + chr(179))               // U+2533 N0S2E2W2
#define GLYPH_BOX_1000   (chr(226) + chr(149) + chr(181))               // U+2575 N1S0E0W0
#define GLYPH_BOX_1001   (chr(226) + chr(148) + chr(152))               // U+2518 N1S0E0W1
#define GLYPH_BOX_1002   (chr(226) + chr(148) + chr(153))               // U+2519 N1S0E0W2
#define GLYPH_BOX_1010   (chr(226) + chr(148) + chr(148))               // U+2514 N1S0E1W0
#define GLYPH_BOX_1011   (chr(226) + chr(148) + chr(180))               // U+2534 N1S0E1W1
#define GLYPH_BOX_1012   (chr(226) + chr(148) + chr(181))               // U+2535 N1S0E1W2
#define GLYPH_BOX_1020   (chr(226) + chr(148) + chr(149))               // U+2515 N1S0E2W0
#define GLYPH_BOX_1021   (chr(226) + chr(148) + chr(182))               // U+2536 N1S0E2W1
#define GLYPH_BOX_1022   (chr(226) + chr(148) + chr(183))               // U+2537 N1S0E2W2
#define GLYPH_BOX_1100   (chr(226) + chr(148) + chr(130))               // U+2502 N1S1E0W0
#define GLYPH_BOX_1101   (chr(226) + chr(148) + chr(164))               // U+2524 N1S1E0W1
#define GLYPH_BOX_1102   (chr(226) + chr(148) + chr(165))               // U+2525 N1S1E0W2
#define GLYPH_BOX_1110   (chr(226) + chr(148) + chr(156))               // U+251C N1S1E1W0
#define GLYPH_BOX_1111   (chr(226) + chr(148) + chr(188))               // U+253C N1S1E1W1
#define GLYPH_BOX_1112   (chr(226) + chr(148) + chr(189))               // U+253D N1S1E1W2
#define GLYPH_BOX_1120   (chr(226) + chr(148) + chr(157))               // U+251D N1S1E2W0
#define GLYPH_BOX_1121   (chr(226) + chr(148) + chr(190))               // U+253E N1S1E2W1
#define GLYPH_BOX_1122   (chr(226) + chr(148) + chr(191))               // U+253F N1S1E2W2
#define GLYPH_BOX_1200   (chr(226) + chr(149) + chr(189))               // U+257D N1S2E0W0
#define GLYPH_BOX_1201   (chr(226) + chr(148) + chr(167))               // U+2527 N1S2E0W1
#define GLYPH_BOX_1202   (chr(226) + chr(148) + chr(170))               // U+252A N1S2E0W2
#define GLYPH_BOX_1210   (chr(226) + chr(148) + chr(159))               // U+251F N1S2E1W0
#define GLYPH_BOX_1211   (chr(226) + chr(149) + chr(129))               // U+2541 N1S2E1W1
#define GLYPH_BOX_1212   (chr(226) + chr(149) + chr(133))               // U+2545 N1S2E1W2
#define GLYPH_BOX_1220   (chr(226) + chr(148) + chr(162))               // U+2522 N1S2E2W0
#define GLYPH_BOX_1221   (chr(226) + chr(149) + chr(134))               // U+2546 N1S2E2W1
#define GLYPH_BOX_1222   (chr(226) + chr(149) + chr(136))               // U+2548 N1S2E2W2
#define GLYPH_BOX_2000   (chr(226) + chr(149) + chr(185))               // U+2579 N2S0E0W0
#define GLYPH_BOX_2001   (chr(226) + chr(148) + chr(154))               // U+251A N2S0E0W1
#define GLYPH_BOX_2002   (chr(226) + chr(148) + chr(155))               // U+251B N2S0E0W2
#define GLYPH_BOX_2010   (chr(226) + chr(148) + chr(150))               // U+2516 N2S0E1W0
#define GLYPH_BOX_2011   (chr(226) + chr(148) + chr(184))               // U+2538 N2S0E1W1
#define GLYPH_BOX_2012   (chr(226) + chr(148) + chr(185))               // U+2539 N2S0E1W2
#define GLYPH_BOX_2020   (chr(226) + chr(148) + chr(151))               // U+2517 N2S0E2W0
#define GLYPH_BOX_2021   (chr(226) + chr(148) + chr(186))               // U+253A N2S0E2W1
#define GLYPH_BOX_2022   (chr(226) + chr(148) + chr(187))               // U+253B N2S0E2W2
#define GLYPH_BOX_2100   (chr(226) + chr(149) + chr(191))               // U+257F N2S1E0W0
#define GLYPH_BOX_2101   (chr(226) + chr(148) + chr(166))               // U+2526 N2S1E0W1
#define GLYPH_BOX_2102   (chr(226) + chr(148) + chr(169))               // U+2529 N2S1E0W2
#define GLYPH_BOX_2110   (chr(226) + chr(148) + chr(158))               // U+251E N2S1E1W0
#define GLYPH_BOX_2111   (chr(226) + chr(149) + chr(128))               // U+2540 N2S1E1W1
#define GLYPH_BOX_2112   (chr(226) + chr(149) + chr(131))               // U+2543 N2S1E1W2
#define GLYPH_BOX_2120   (chr(226) + chr(148) + chr(161))               // U+2521 N2S1E2W0
#define GLYPH_BOX_2121   (chr(226) + chr(149) + chr(132))               // U+2544 N2S1E2W1
#define GLYPH_BOX_2122   (chr(226) + chr(149) + chr(135))               // U+2547 N2S1E2W2
#define GLYPH_BOX_2200   (chr(226) + chr(148) + chr(131))               // U+2503 N2S2E0W0
#define GLYPH_BOX_2201   (chr(226) + chr(148) + chr(168))               // U+2528 N2S2E0W1
#define GLYPH_BOX_2202   (chr(226) + chr(148) + chr(171))               // U+252B N2S2E0W2
#define GLYPH_BOX_2210   (chr(226) + chr(148) + chr(160))               // U+2520 N2S2E1W0
#define GLYPH_BOX_2211   (chr(226) + chr(149) + chr(130))               // U+2542 N2S2E1W1
#define GLYPH_BOX_2212   (chr(226) + chr(149) + chr(137))               // U+2549 N2S2E1W2
#define GLYPH_BOX_2220   (chr(226) + chr(148) + chr(163))               // U+2523 N2S2E2W0
#define GLYPH_BOX_2221   (chr(226) + chr(149) + chr(138))               // U+254A N2S2E2W1
#define GLYPH_BOX_2222   (chr(226) + chr(149) + chr(139))               // U+254B N2S2E2W2

// Indexed table: BOX_GLYPHS_INDEXED[n*27 + s*9 + e*3 + w] is the glyph
// for that arm-weight combination. Assign once (e.g. in create()) and
// index it per cell.
#define BOX_GLYPHS_INDEXED ({ \
  GLYPH_BOX_0000, \
  GLYPH_BOX_0001, \
  GLYPH_BOX_0002, \
  GLYPH_BOX_0010, \
  GLYPH_BOX_0011, \
  GLYPH_BOX_0012, \
  GLYPH_BOX_0020, \
  GLYPH_BOX_0021, \
  GLYPH_BOX_0022, \
  GLYPH_BOX_0100, \
  GLYPH_BOX_0101, \
  GLYPH_BOX_0102, \
  GLYPH_BOX_0110, \
  GLYPH_BOX_0111, \
  GLYPH_BOX_0112, \
  GLYPH_BOX_0120, \
  GLYPH_BOX_0121, \
  GLYPH_BOX_0122, \
  GLYPH_BOX_0200, \
  GLYPH_BOX_0201, \
  GLYPH_BOX_0202, \
  GLYPH_BOX_0210, \
  GLYPH_BOX_0211, \
  GLYPH_BOX_0212, \
  GLYPH_BOX_0220, \
  GLYPH_BOX_0221, \
  GLYPH_BOX_0222, \
  GLYPH_BOX_1000, \
  GLYPH_BOX_1001, \
  GLYPH_BOX_1002, \
  GLYPH_BOX_1010, \
  GLYPH_BOX_1011, \
  GLYPH_BOX_1012, \
  GLYPH_BOX_1020, \
  GLYPH_BOX_1021, \
  GLYPH_BOX_1022, \
  GLYPH_BOX_1100, \
  GLYPH_BOX_1101, \
  GLYPH_BOX_1102, \
  GLYPH_BOX_1110, \
  GLYPH_BOX_1111, \
  GLYPH_BOX_1112, \
  GLYPH_BOX_1120, \
  GLYPH_BOX_1121, \
  GLYPH_BOX_1122, \
  GLYPH_BOX_1200, \
  GLYPH_BOX_1201, \
  GLYPH_BOX_1202, \
  GLYPH_BOX_1210, \
  GLYPH_BOX_1211, \
  GLYPH_BOX_1212, \
  GLYPH_BOX_1220, \
  GLYPH_BOX_1221, \
  GLYPH_BOX_1222, \
  GLYPH_BOX_2000, \
  GLYPH_BOX_2001, \
  GLYPH_BOX_2002, \
  GLYPH_BOX_2010, \
  GLYPH_BOX_2011, \
  GLYPH_BOX_2012, \
  GLYPH_BOX_2020, \
  GLYPH_BOX_2021, \
  GLYPH_BOX_2022, \
  GLYPH_BOX_2100, \
  GLYPH_BOX_2101, \
  GLYPH_BOX_2102, \
  GLYPH_BOX_2110, \
  GLYPH_BOX_2111, \
  GLYPH_BOX_2112, \
  GLYPH_BOX_2120, \
  GLYPH_BOX_2121, \
  GLYPH_BOX_2122, \
  GLYPH_BOX_2200, \
  GLYPH_BOX_2201, \
  GLYPH_BOX_2202, \
  GLYPH_BOX_2210, \
  GLYPH_BOX_2211, \
  GLYPH_BOX_2212, \
  GLYPH_BOX_2220, \
  GLYPH_BOX_2221, \
  GLYPH_BOX_2222 \
})

// Non-box map glyphs (also non-ASCII, kept here for the same reason).
#define GLYPH_MAP_FOREST   (chr(226) + chr(153) + chr(163))         // U+2663 black club suit
#define GLYPH_MAP_CITY     (chr(226) + chr(150) + chr(147))         // U+2593 dark shade block
#define GLYPH_MAP_FARM     (chr(226) + chr(137) + chr(161))         // U+2261 identical-to: plowed crop rows
// A city whose locations are mostly forest: the woods glyph in bold green.
// BOLD goes after GREEN so a plain forest cell ("%^GREEN%^" + glyph) never
// matches inside it when the legend looks for one.
#define GLYPH_FOREST_CITY  ("%^GREEN%^%^BOLD%^" + GLYPH_MAP_FOREST + "%^RESET%^")
#define GLYPH_MAP_COAST       "~"
#define GLYPH_MAP_UNDERGROUND "."

// What a sector shows on the world map is one of these codes (a sector keeps
// its code, see /lib/location/sector.c). The fixed glyphs come first, in
// MAP_GLYPHS order; then the two bridge crossings, the sixteen ways and the
// sixteen rivers, which /lib/handlers/worldmap.c builds from the box tables.
// A way or river code adds n*8 + s*4 + e*2 + w for the arms it draws.
#define MAP_GLYPH_EMPTY        0
#define MAP_GLYPH_CITY         1
#define MAP_GLYPH_FOREST_CITY  2
#define MAP_GLYPH_FOREST       3
#define MAP_GLYPH_FARM         4
#define MAP_GLYPH_COAST        5
#define MAP_GLYPH_UNDERGROUND  6
#define MAP_GLYPH_BRIDGE_V     7   // a road east-west over a river north-south
#define MAP_GLYPH_BRIDGE_H     8   // a road north-south over a river east-west
#define MAP_GLYPH_WAYS         9
#define MAP_GLYPH_RIVERS       25
#define MAP_GLYPH_COUNT        41

// The fixed glyphs as they are drawn, colour included.
#define MAP_GLYPHS ({ \
  " ", \
  GLYPH_MAP_CITY, \
  GLYPH_FOREST_CITY, \
  "%^GREEN%^" + GLYPH_MAP_FOREST + "%^RESET%^", \
  "%^YELLOW%^" + GLYPH_MAP_FARM + "%^RESET%^", \
  GLYPH_MAP_COAST, \
  GLYPH_MAP_UNDERGROUND, \
})
#define GLYPH_WALL_N_ROAD  (chr(226) + chr(149) + chr(168))         // U+2568 up double, horiz single
#define GLYPH_WALL_E_ROAD  (chr(226) + chr(149) + chr(158))         // U+255E vert single, right double
#define GLYPH_WALL_S_ROAD  (chr(226) + chr(149) + chr(165))         // U+2565 down double, horiz single
#define GLYPH_WALL_W_ROAD  (chr(226) + chr(149) + chr(161))         // U+2561 vert single, left double

// Double-line box glyphs, for rivers. Indexed by the arms present as a mask,
// N=8 S=4 E=2 W=1 (BOX_GLYPHS_DOUBLE[n*8 + s*4 + e*2 + w]); a single arm has
// no half-line in the double set, so it uses the full line of its axis.
#define GLYPH_DBL_H        (chr(226) + chr(149) + chr(144))         // U+2550 horizontal
#define GLYPH_DBL_V        (chr(226) + chr(149) + chr(145))         // U+2551 vertical
#define GLYPH_DBL_SE       (chr(226) + chr(149) + chr(148))         // U+2554 down and right
#define GLYPH_DBL_SW       (chr(226) + chr(149) + chr(151))         // U+2557 down and left
#define GLYPH_DBL_NE       (chr(226) + chr(149) + chr(154))         // U+255A up and right
#define GLYPH_DBL_NW       (chr(226) + chr(149) + chr(157))         // U+255D up and left
#define GLYPH_DBL_NSE      (chr(226) + chr(149) + chr(160))         // U+2560 vertical and right
#define GLYPH_DBL_NSW      (chr(226) + chr(149) + chr(163))         // U+2563 vertical and left
#define GLYPH_DBL_SEW      (chr(226) + chr(149) + chr(166))         // U+2566 down and horizontal
#define GLYPH_DBL_NEW      (chr(226) + chr(149) + chr(169))         // U+2569 up and horizontal
#define GLYPH_DBL_NSEW     (chr(226) + chr(149) + chr(172))         // U+256C vertical and horizontal

#define BOX_GLYPHS_DOUBLE ({ \
  " ", GLYPH_DBL_H, GLYPH_DBL_H, GLYPH_DBL_H, \
  GLYPH_DBL_V, GLYPH_DBL_SW, GLYPH_DBL_SE, GLYPH_DBL_SEW, \
  GLYPH_DBL_V, GLYPH_DBL_NW, GLYPH_DBL_NE, GLYPH_DBL_NEW, \
  GLYPH_DBL_V, GLYPH_DBL_NSW, GLYPH_DBL_NSE, GLYPH_DBL_NSEW \
})

// A road crossing a river in one sector.
#define GLYPH_CROSS_DBL_V  (chr(226) + chr(149) + chr(171))         // U+256B vertical double, horizontal single
#define GLYPH_CROSS_DBL_H  (chr(226) + chr(149) + chr(170))         // U+256A horizontal double, vertical single

#endif
