
// base.c

#define _LANG_ITEMS_CHOOSE_VERBS ({ "escoger" })
#define _LANG_ITEMS_CHOOSE_FAIL "¿Coger el qué?\n"
#define _LANG_ITEMS_FIGHTER_MSG "Desde este día tu viaje por los reinos estará " + \
      "marcado por tu honor y por tu destreza con la espada.\n\n" + \
      "Que los Dioses te acompañen en tu viaje...\n\n"
#define _LANG_ITEMS_MIXED_MSG "Desde este día tu deambular por el mundo estará marcado " + \
    "por tu capacidad para la negociación, tu habilidad para no " + \
    "llamar la atención y, en los peores casos, " + \
    "tu maestría con la espada, el engaño o incluso la huída.\n\n" + \
    "Que los Dioses te acompañen en tu viaje...\n\n"
#define _LANG_ITEMS_SCHOLAR_MSG "Desde este día tu deambular por el mundo estará marcado " + \
    "por tu capacidad para el estudio, tu inteligencia y la sabiduría que obtengas en tus viajes.\n\n" + \
    "Que los Dioses te acompañen en tu viaje...\n\n"

// sword.c

#define _LANG_SWORD_NAME "espada"
#define _LANG_SWORD_DESC "Una espada brillante y pulida, de un extraño metal que no " + \
    "sabes reconocer. Parece contener en su interior toda la " + \
    "fuerza y la energía de los guerreros de antaño. " + \
    "Con una espada como ésta cualquiera podría marcar su " + \
    "propio destino, viajando hasta lugares donde sólo " + \
    "la ley del acero puede tenerse en cuenta...\n" + \
    "\n%^BOLD%^Si coges la espada te convertirás en un guerrero.%^RESET%^\n"

// key.c

#define _LANG_KEY_NAME "llave"
#define _LANG_KEY_DESC "Una pequeña llave de aspecto común, con una pequeña arandela " + \
    "colgando de un extremo para poder unirla a otras. Está fabricada " + \
    "en un metal bastante oscuro y aparentemente bastante resistente. " + \
    "Tiene una extraña forma que parece hacerla capaz de abrir hasta la " + \
    "más compleja cerradura. Sin duda esta llave podría abrir puertas " + \
    "que ocultan increíbles secretos o grandes fortunas, y promete "
    "aventuras sin fin...\n" + \
    "\n%^BOLD%^Si coges la llave te convertirás en la clase mixta, el explorador.%^RESET%^\n"

// book.c

#define _LANG_BOOK_NAME "libro"
#define _LANG_BOOK_DESC "Este libro polvoriento encuadernado en cuero tiene un " + \
    "emblema dorado descolorido en su portada que representa un búho posado " + \
    "sobre un libro abierto. Sus páginas están amarillentas y gastadas, " + \
    "con diminutas letras escritas a mano que cubren cada centímetro. " + \
    "Su peso es notable, como si sostenerlo en tus manos te otorgara acceso " + \
    "a todo el conocimiento contenido en su interior. Sin duda, " + \
    "este tomo encierra los secretos para desbloquear los misterios " + \
    "del universo y promete transformarte en un gran erudito, cambiando " + \
    "para siempre el curso de tu vida...\n" + \
    "\n%^BOLD%^Si coges el libro te convertirás en un erudito.%^RESET%^\n"

// orb.c

#define _LANG_ORB_NAME "orbe"
#define _LANG_ORB_DESC "Un orbe de color rojizo, con sutiles formas vaporosas " + \
    "moviéndose dentro, dándole una apariencia mágica y " + \
    "misteriosa que te atrae. Parece que el orbe puede comunicarse " + \
    "contigo de alguna forma primitiva y desconocida, directamente " + \
    "a tu mente... te dice que quizá deberías 'ajustar'.\n"
#define _LANG_ADJUST_VERBS ({ "ajustar" })
#define _LANG_ORB_ALREADY_ADJUSTED "Ya has ajustado la ficha con anterioridad... " + \
      "¿Cómo has llegado hasta aquí?\n"
#define _LANG_ORB_RACIAL_BONUSES "\n    Fue: %-+2d  Des: %-+2d  Con: %-+2d  Int: %-+2d\n" + \
    "    Sab: %-+2d  Car: %-+2d  Per: %-+2d  Vol: %-+2d\n\n"
#define _LANG_ORB_ADJUST_TXT "\nVas a utilizar un método de asignación de puntos predefinidos para terminar de " + \
    "crear tu personaje. %^BOLD%^RED%^Lo único que tienes que seleccionar%^RESET%^ es el orden " + \
    "de todos sus atributos de mayor a menor importancia para " + this_player()->query_objective() + \
    ". Estos atributos son:\n\n" + \
    "    %^BOLD%^CYAN%^Fue%^RESET%^rza\n    %^BOLD%^CYAN%^Des%^RESET%^treza\n    %^BOLD%^CYAN%^Con%^RESET%^stitución\n    %^BOLD%^CYAN%^Int%^RESET%^eligencia\n" + \
    "    %^BOLD%^CYAN%^Sab%^RESET%^iduría\n    %^BOLD%^CYAN%^Car%^RESET%^isma\n    %^BOLD%^CYAN%^Per%^RESET%^cepción\n    %^BOLD%^CYAN%^Vol%^RESET%^untad\n\n" + \
    "La importancia de cada uno de estos atributos dependerá de cómo quieras que sea " + \
    "tu personaje en el futuro, e influirán de un modo muy importante en muchos " + \
    "aspectos del juego. " + \
    "Tus bonificadores por la raza que has escogido son:\n " + racial_bonuses + \
    "Por favor, introduce el orden de importancia (de mayor a menor).\n" + \
    "Por ejemplo: %^BOLD%^fue con des per int car vol sab%^RESET%^\n\n" + \
    "Escribe 'c' o 'cancelar' en cualquier momento para abandonar el ajuste\n" + \
    "de las características."
#define _LANG_ORB_ADJUST_ORDER "\nOrden: "
#define _LANG_ORB_ADJUST_INVALID "Ese orden no es válido.\n"
#define _LANG_ORB_ADJUST_TRY_AGAIN "Prueba otra vez con " + \
      "un patrón de la forma: %^BOLD%^fue con des per int car vol sab%^RESET%^\n"
#define _LANG_ORB_ADJUST_CANCELLED "Ajustar cancelado.\n"
#define _LANG_ORB_ADJUST_EIGHT "Debes incluir los ocho atributos.\n"
#define _LANG_ORB_ADJUST_CONFIRM "Has seleccionado: "+spew+"\n\n¿Es correcto? " + \
    "(sí/no/cancelar) "
#define _LANG_ATTRIBUTES ({ "fue", "des", "con", "int", "sab", "car", "per", "vol" })
#define _LANG_ORB_ADJUST_OK "Ok, tus atributos serán:\n"
#define _LANG_ORB_ADJUST_CONGRATS "¡Enhorabuena! La generación de atributos " + \
    "ha sido completada. Tu personaje ya está completo, puedes comenzar a jugar."