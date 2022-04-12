
// common parts of descriptions

#define __ROAD_LONG "Te encuentras en una carretera de adoquines para facilitar " + \
        "el trasporte de tiro."
#define __ROAD_MOUNTAINS "Miras al horizonte y admiras el paisaje " + \
        "montañoso que te rodea."
#define __ROAD_NO_TREES "Esta parte esta despoblada de árboles " + \
        "y lo único que ves son algunos matorrales y hierbajos."
#define __ROAD_NEAR_FOREST "Miras al horizonte y admiras el paisaje " + \
        "boscoso que se acerca."
#define __ROAD_FOREST "A tu alrededor puedes observar una gran " + \
        "cantidad de árboles de distintos tamaños y formas."
#define __ROAD_FAR_FOREST "En la distancia, no demasiado lejos, puedes ver un denso bosque."

// rooms

#define _LANG_ROAD_SHORT "Carretera"
#define _LANG_ROAD_LONG __ROAD_LONG + " " + __ROAD_MOUNTAINS + " " + __ROAD_NO_TREES + "\n"
#define _LANG_ROAD_TEMPLE "Al este puedes ver una construcción semejante a un templo.\n"
#define _LANG_ROAD_NEAR_FOREST_SHORT _LANG_ROAD_SHORT + " en el bosque"
#define _LANG_ROAD_NEAR_FOREST_LONG __ROAD_LONG + " " + __ROAD_NEAR_FOREST + " " + __ROAD_NO_TREES + "\n"
#define _LANG_ROAD_FOREST_LONG __ROAD_LONG + " " + __ROAD_FOREST + "\n"
#define _LANG_ROAD_FAR_FOREST_LONG __ROAD_LONG + " " + __ROAD_FAR_FOREST + "\n"

#define _LANG_RAISEROOM_SHORT "%^CYAN%^Habitación de Resurrección%^RESET%^"
#define _LANG_RAISEROOM_LONG "Este lugar te recuerda enormemente a un templo, ya " + \
    "que puedes ver que en su interior hay un enorme altar, y detrás " + \
    "de este ves una estatua que representa al " + \
    "Dios Lummen. Si tu devoción al Dios Lummen es pura te estará " + \
    "permitido recuperar tu forma mortal.\n"
#define _LANG_RAISEROOM_STATUE ({ "estatua" })
#define _LANG_RAISEROOM_STATUE_DESC "Forjada en metal, representa al Dios Lummen, " + \
    "portador de las almas y benefactor de todos los clérigos del reino.\n"
#define _LANG_RAISEROOM_ALTAR ({ "altar" })
#define _LANG_RAISEROOM_ALTAR_DESC "Un sobrio altar de piedra pulida.\n"

#define _LANG_GUILD_SHORT "Casa del Aventurero"
#define _LANG_GUILD_LONG "En la casa del aventurero los jóvenes aprendices de cualquier " + \
  "lugar pueden compartir sus pertenencias para ayudarse unos a otros, " + \
  "darse consejos, entrenar, y apoyarse en general. Al fondo de la " + \
  "estancia puedes ver un gran arcón en el que los aventureros " + \
  "pueden dejar sus pertenencias, con una única condición: aquí " + \
  "todo pertenece a todos, por lo que nada tiene un dueño oficial. Por " + \
  "lo tanto, si te llevas algo que necesites, cuando puedas devuélvelo, " + \
  "por el bien común. Ante tí tienes un gran tablón de notas " + \
  "donde los aventureros pueden dejarse mensajes unos a otros.\n"

#define _LANG_VAULT_SHORT "Casa del Aventurero: Baúl"
#define _LANG_VAULT_LONG "En la casa del aventurero los jóvenes aprendices de cualquier " + \
  "lugar pueden compartir sus pertenencias para ayudarse unos a otros, " + \
  "darse consejos, entrenar, y apoyarse en general. " + \
  "Puedes ver un gran arcón en el que los aventureros " + \
  "pueden dejar sus pertenencias, con una única condición: aquí " + \
  "todo pertenece a todos, por lo que nada tiene un dueño oficial. Por " + \
  "lo tanto, si te llevas algo que necesites, cuando puedas devuélvelo, " + \
  "por el bien común.\n"

#define _LANG_CROSSING_SHORT "Intersección"
#define _LANG_CROSSING_LONG "Te encuentras en una intersección por la que puedes tomar tres " + \
        "caminos diferentes. El camino del este y suroeste se internan " + \
        "en un bosque, pero el camino del noroeste te lleva hacia las montañas.\n"
#define _LANG_CROSSING_SIGN_LONG "Es una placa de metal clavada en el suelo a modo de " + \
    "tablero de informacion.\n"
#define _LANG_CROSSING_SIGN_MESS "  Este     : Naduk     \n" + \
                                 "  Suroeste : Erken     \n" + \
                                 "  Noroeste : Montañas  \n"
#define _LANG_CROSSING_SIGN_SHORT "Placa de metal"
#define _LANG_CROSSING_SIGN_NAME "placa"




