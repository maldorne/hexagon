
// begin.c

#define _LANG_START_SHORT "%^BOLD%^Arboleda %^GREEN%^esmeralda%^RESET%^"
#define _LANG_START_LONG "Pasas a través de un portal brillante y te encuentras " + \
          "en un mundo vibrante y fantástico lleno del dulce aroma de las flores " + \
          "y el sonido lejano de los pájaros. Ante ti se extiende un vasto " + \
          "paisaje de colinas ondulantes, montañas imponentes y vegetación exuberante, " + \
          "habitado por criaturas que nunca antes habías visto. Mientras " + \
          "admiras los paisajes y sonidos de este nuevo mundo, una sensación " + \
          "de emoción y anticipación crece dentro de ti, ansioso por " + \
          "emprender tu viaje y descubrir qué maravillas y peligros te esperan."
#define _LANG_START_HINTS ({ "Echa un vistazo a los objetos que hay aquí. " + \
          "Solo puedes coger uno de ellos.", \
          "Escribe 'escoger <objeto>' para elegir tu clase de personaje." })

// begin2.c

#define _LANG_START2_SHORT "%^BOLD%^Dosel %^GREEN%^esmeralda%^RESET%^"
#define _LANG_START2_LONG "Al caminar un poco más, te encuentras en el borde " + \
          "de un denso bosque que se yergue ante ti, con su dosel esmeralda " + \
          "elevándose por encima de tu cabeza. El canto de los pájaros se hace " + \
          "más fuerte, acompañado del susurro de las hojas y el sonido " + \
          "distante del agua que fluye. El aire aquí es más fresco y el dulce " + \
          "aroma de las flores en flor es reemplazado por el aroma terroso " + \
          "de la tierra húmeda y la vegetación fresca. Sientes una " + \
          "sensación de intriga y temor mientras observas hacia el " + \
          "corazón del bosque, preguntándote qué secretos y desafíos " + \
          "se encuentran dentro. A tu alrededor puedes ver " + \
          number_as_string(num_races) + " %^BOLD%^cofre" + ((num_races==1) ? "" : "s") + \
          "%^RESET%^, colocado" + ((num_races == 1) ? "" : "s") + " " \
          "como si fueses el centro de toda la existencia."
#define _LANG_START2_CHESTS_NAME "cofre"
#define _LANG_START2_CHESTS ({ "cofres", "cofre", "efigie", "efigies" })
#define _LANG_START2_CHESTS_DESC "Puedes ver a tu alrededor " + number_as_string(num_races) + " pequeños cofres con las " + \
    "efigies de seres de las más diversas razas de " + game_pretty_name(this_object()) + ". Concretamente " + \
    "observas dibujos de "+query_multiple_short(keys(races)) + ". " + \
    "%^BOLD%^Puedes mirar a cada uno de ellos por separado " + \
    "para observarlos detenidamente (ej: <mirar cofre " + keys(races)[random(sizeof(races))] + ">).%^RESET%^\n"
#define _LANG_START2_HINTS ({ "Echa un vistazo a los cofres que hay aquí. " + \
          "Solo puedes abrir uno de ellos.", \
          "Escribe 'abrir cofre " + r + "' para elegir la raza de tu personaje." })
#define _LANG_START2_LOOK_VERBS ({ "mirar" })
#define _LANG_START2_OPEN_VERBS ({ "abrir" })
#define _LANG_START2_CHEST_RACE "Cofre con la efigie de un "+r+"."
#define _LANG_START2_CHEST_NOT_FOUND "¿Abrir qué? Quizá quieras 'abrir cofre <raza>'.\n"
#define _LANG_START2_OPEN_MSG "Te acercas al cofre con la efigie dorada de " + \
    "un "+r+" y lo abres con cuidado, notando como comienza " + \
    "a atraerte hacia su interior antes de perder la consciencia.\n"