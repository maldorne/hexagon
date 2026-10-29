// fullmap

#define _LANG_CMD_FULLMAP_ALIAS  ({ "fullmap" })
#define _LANG_CMD_FULLMAP_SYNTAX "fullmap [<game>] [<z>] [html]"
#define _LANG_CMD_FULLMAP_HELP   "Draws the whole world map of a game level: every sector, with city names, " + \
  "no frame, legend or player marker.\n" + \
  "\n" + \
  "  fullmap                 the level you stand on, of your game\n" + \
  "  fullmap <game> <z>      another game and sector level\n" + \
  "  fullmap html            also writes it as a coloured page to\n" + \
  "                          /games/<game>/docs/maps/world-<z>.html\n" + \
  "                          (a negative level is written as m<z>)\n"
#define _LANG_CMD_FULLMAP_USAGE  "Syntax: fullmap [<game>] [<z>] [html]\n"
#define _LANG_CMD_FULLMAP_NOMAP  "That game level has no sectors.\n"
#define _LANG_CMD_FULLMAP_WRITTEN "Map written to %s.\n"
#define _LANG_CMD_FULLMAP_SIZE   "%d x %d sectors.\n"
#define _LANG_CMD_FULLMAP_NOPACKAGE "The fullmap package is not available.\n"
