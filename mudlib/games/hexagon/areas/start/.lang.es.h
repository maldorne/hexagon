
// begin.c

#define _LANG_START_SHORT "%^BOLD%^Luz en las %^BLACK%^Tinieblas%^RESET%^"
#define _LANG_START_LONG "Te encuentras en una pequeña habitación con poca iluminación, " + \
          "frente a un viejo terminal de ordenador con un teclado y un ratón polvorientos. " + \
          "En la pantalla del monitor se despliega una lista de juegos diversos, " + \
          "cada uno con una breve descripción y un botón de 'jugar'. Mientras " + \
          "contemplas las posibilidades, la habitación parece cobrar vida con un " + \
          "suave zumbido de energía, y comienza a cargar el juego que has elegido. " + \
          "Sientes una sensación de anticipación mientras esperas que la aventura comience."
#define _LANG_START_HINTS ({ "Escribe 'juegos' para ver la lista de juegos " + \
          "disponibles en los que puedes jugar.", "Escribe 'jugar <número>' para " + \
          "empezar a jugar ese juego." })
#define _LANG_PLAY_VERBS ({ "jugar", "play" })
#define _LANG_PLAY_FAIL "¿Jugar qué?\n"
#define _LANG_PLAY_GAME_DO_NOT_LOAD "El juego elegido parece que no funciona.\n"
// why the chosen game cannot be played, by the reason the game gives
#define _LANG_PLAY_CLOSED ([ \
    GAME_CLOSED: "Ese juego no está abierto a los jugadores.\n", \
    GAME_NEEDS_DEMO: "Hasta que tu cuenta no termine una demostración, sólo " + \
      "puedes crear personajes en ellas.\n", \
    GAME_DEMO_TAKEN: "Ya tienes otro personaje en esa demostración: sólo " + \
      "puede haber uno de cada cuenta en cada demostración.\n" ])
#define _LANG_PLAY_START "Ok, has elegido jugar a " + games[i]->query_game_name() + \
          ". Tu personaje da su primer paso en lo que podría ser un largo viaje. ¡Buena suerte!\n"
