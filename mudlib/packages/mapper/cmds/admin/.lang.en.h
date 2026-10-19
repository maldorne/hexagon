// mapper

#define _LANG_CMD_MAPPER_ALIAS  ({ "mapper" })
#define _LANG_CMD_MAPPER_SYNTAX "mapper update <area dir> [all] | mapper list [<game>]"
#define _LANG_CMD_MAPPER_HELP   "Dumps a player-safe map of an area to JSON, for a public map viewer.\n" + \
  "\n" + \
  "  mapper update <area dir>        the locations of that area only\n" + \
  "  mapper update <area dir> all    with every sub-area under it\n" + \
  "  mapper list [<game>]            the maps written and when\n" + \
  "\n" + \
  "<area dir> is like /games/<game>/areas/<area>. The map is written to\n" + \
  "/games/<game>/docs/maps/<area path with - for />.json and is read from the\n" + \
  "saved locations, without loading any of them.\n"
#define _LANG_CMD_MAPPER_USAGE  "Syntax: mapper update <area dir> [all] | mapper list [<game>]\n"
