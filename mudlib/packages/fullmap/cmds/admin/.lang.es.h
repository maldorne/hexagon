// fullmap

#define _LANG_CMD_FULLMAP_ALIAS  ({ "fullmap", "mapacompleto" })
#define _LANG_CMD_FULLMAP_SYNTAX "fullmap [<juego>] [<z>] [html] [json]"
#define _LANG_CMD_FULLMAP_HELP   "Dibuja el mapa completo de un nivel de un juego: todos los sectores, con los " + \
  "nombres de las ciudades, sin marco, leyenda ni marca del jugador.\n" + \
  "\n" + \
  "  fullmap                 el nivel donde estás, de tu juego\n" + \
  "  fullmap <juego> <z>     otro juego y nivel de sectores\n" + \
  "  fullmap html            además lo escribe como página con colores en\n" + \
  "                          /games/<juego>/docs/maps/world-<z>.html\n" + \
  "                          (un nivel negativo se escribe como m<z>)\n" + \
  "  fullmap json            además lo escribe como datos para la web en\n" + \
  "                          /games/<juego>/docs/maps/world-<z>.json\n"
#define _LANG_CMD_FULLMAP_USAGE  "Sintaxis: fullmap [<juego>] [<z>] [html] [json]\n"
#define _LANG_CMD_FULLMAP_NOMAP  "Ese nivel del juego no tiene sectores.\n"
#define _LANG_CMD_FULLMAP_WRITTEN "Mapa escrito en %s.\n"
#define _LANG_CMD_FULLMAP_SIZE   "%d x %d sectores.\n"
#define _LANG_CMD_FULLMAP_NOPACKAGE "El paquete fullmap no está disponible.\n"
