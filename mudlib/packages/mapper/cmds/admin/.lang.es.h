// mapper

#define _LANG_CMD_MAPPER_ALIAS  ({ "mapper" })
#define _LANG_CMD_MAPPER_SYNTAX "mapper update <carpeta del área> [all] | mapper list [<juego>]"
#define _LANG_CMD_MAPPER_HELP   "Vuelca a JSON un mapa de un área apto para jugadores, para un visor público.\n" + \
  "\n" + \
  "  mapper update <carpeta>        solo las localizaciones de ese área\n" + \
  "  mapper update <carpeta> all    con todas las subáreas que cuelgan de ella\n" + \
  "  mapper list [<juego>]          los mapas escritos y cuándo\n" + \
  "\n" + \
  "<carpeta> es como /games/<juego>/areas/<area>. El mapa se escribe en\n" + \
  "/games/<juego>/docs/maps/<ruta del área con - en lugar de />.json y se lee de\n" + \
  "las localizaciones guardadas, sin cargar ninguna.\n"
#define _LANG_CMD_MAPPER_USAGE  "Sintaxis: mapper update <carpeta del área> [all] | mapper list [<juego>]\n"
