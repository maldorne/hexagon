
#define DIR_SOME_PLACE "algún sitio"

#define DIR_DOWN      "abajo"
#define DIR_UP        "arriba"
#define DIR_IN        "dentro"
#define DIR_OUT       "fuera"
#define DIR_NORTH     "norte"
#define DIR_SOUTH     "sur"
#define DIR_EAST      "este"
#define DIR_WEST      "oeste"
#define DIR_SOUTHWEST "sudoeste"
#define DIR_SOUTHEAST "sudeste"
#define DIR_NORTHEAST "noreste"
#define DIR_NORTHWEST "noroeste"
#define DIR_COMMON    "comun"
#define DIR_STAIRS    "escaleras"

#define DIR_DOWN_SHORTEN      "ab"
#define DIR_UP_SHORTEN        "ar"
#define DIR_IN_SHORTEN        "de"
#define DIR_OUT_SHORTEN       "fu"
#define DIR_NORTH_SHORTEN     "n"
#define DIR_SOUTH_SHORTEN     "s"
#define DIR_EAST_SHORTEN      "e"
#define DIR_WEST_SHORTEN      "o"
#define DIR_SOUTHWEST_SHORTEN "so"
#define DIR_SOUTHEAST_SHORTEN "se"
#define DIR_NORTHEAST_SHORTEN "ne"
#define DIR_NORTHWEST_SHORTEN "no"
#define DIR_STAIRS_SHORTEN    "es"

/* Some common aliases */
#define EXPANSION ([ \
   DIR_DOWN_SHORTEN      : DIR_DOWN, \
   DIR_UP_SHORTEN        : DIR_UP, \
   DIR_IN_SHORTEN        : DIR_IN, \
   DIR_OUT_SHORTEN       : DIR_OUT, \
   DIR_NORTH_SHORTEN     : DIR_NORTH, \
   DIR_SOUTH_SHORTEN     : DIR_SOUTH, \
   DIR_EAST_SHORTEN      : DIR_EAST, \
   DIR_WEST_SHORTEN      : DIR_WEST, \
   DIR_SOUTHWEST_SHORTEN : DIR_SOUTHWEST, \
   DIR_SOUTHEAST_SHORTEN : DIR_SOUTHEAST, \
   DIR_NORTHEAST_SHORTEN : DIR_NORTHEAST, \
   DIR_NORTHWEST_SHORTEN : DIR_NORTHWEST, \
   DIR_STAIRS_SHORTEN    : DIR_STAIRS, \
])

#define OPPOSITES ([ \
   DIR_NORTH     : DIR_SOUTH, \
   DIR_SOUTH     : DIR_NORTH, \
   DIR_WEST      : DIR_EAST, \
   DIR_EAST      : DIR_WEST, \
   DIR_NORTHWEST : DIR_SOUTHEAST, \
   DIR_SOUTHEAST : DIR_NORTHWEST, \
   DIR_NORTHEAST : DIR_SOUTHWEST, \
   DIR_SOUTHWEST : DIR_NORTHEAST, \
   DIR_UP        : DIR_DOWN, \
   DIR_DOWN      : DIR_UP, \
   DIR_IN        : DIR_OUT, \
   DIR_OUT       : DIR_IN, \
   DIR_STAIRS    : DIR_STAIRS, \
])

#define EXIT_ARTICLES ([ \
   DIR_NORTH     : "el", \
   DIR_SOUTH     : "el", \
   DIR_WEST      : "el", \
   DIR_EAST      : "el", \
   DIR_NORTHWEST : "el", \
   DIR_SOUTHEAST : "el", \
   DIR_NORTHEAST : "el", \
   DIR_SOUTHWEST : "el", \
   DIR_STAIRS    : "las", \
   "entrada"     : "la", \
   "salida"      : "la", \
])

// ============================================================
//  Canonicalización de direcciones agnóstica al idioma
//
//  Todo lo que se escribe en /save debe usar las palabras de dirección
//  canónicas en inglés para que el estado de un juego siga siendo
//  migrable entre los idiomas en que se compila el mudlib.
//  DIR_TO_CANONICAL pliega cualquier palabra de dirección que este
//  idioma acepta (nombre completo o abreviatura) — más las inglesas, para
//  que los datos heredados en idioma mezclado se normalicen — a su forma
//  canónica inglesa. DIR_FROM_CANONICAL es el reverso, para mostrar.
//  Ver room_handler::canonical_dir / localize_dir.
// ============================================================

#define DIR_TO_CANONICAL ([ \
   "norte":"north", "sur":"south", "este":"east", "oeste":"west", \
   "noreste":"northeast", "noroeste":"northwest", \
   "sudeste":"southeast", "sudoeste":"southwest", \
   "arriba":"up", "abajo":"down", "dentro":"inside", "fuera":"outside", \
   "escaleras":"stairs", \
   "n":"north", "s":"south", "e":"east", "o":"west", \
   "ne":"northeast", "no":"northwest", "se":"southeast", "so":"southwest", \
   "ar":"up", "ab":"down", "de":"inside", "fu":"outside", "es":"stairs", \
   "north":"north", "south":"south", "east":"east", "west":"west", \
   "northeast":"northeast", "northwest":"northwest", \
   "southeast":"southeast", "southwest":"southwest", \
   "up":"up", "down":"down", "inside":"inside", "outside":"outside", \
   "stairs":"stairs", "in":"inside", "out":"outside", \
])

#define DIR_FROM_CANONICAL ([ \
   "north":"norte", "south":"sur", "east":"este", "west":"oeste", \
   "northeast":"noreste", "northwest":"noroeste", \
   "southeast":"sudeste", "southwest":"sudoeste", \
   "up":"arriba", "down":"abajo", "inside":"dentro", "outside":"fuera", \
   "stairs":"escaleras", \
])
