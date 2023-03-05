
#define _LANG_NPCS_SNAKE_NAME "serpiente"
#define _LANG_NPCS_SNAKE_LONG "Es una gran serpiente de tierra bastante agresiva.\n"
#define _LANG_NPCS_SNAKE_JOIN_FIGHT "La serpiente muestra sus colmillos y ataca.\n"

#define _LANG_NPCS_PILGRIM_NAME "peregrino"
#define _LANG_NPCS_PILGRIM_LONG "Es un hombre de las carreteras que viaja en peregrinación " + \
    "hacia algún templo. Parece un hombre inofensivo.\n" 
#define _LANG_NPCS_PILGRIM_CHAT ({ "'Si deseas visitar el Templo del Dios Lummen ve al norte.", \
                                   ":sonríe amablemente." })

#define _LANG_NPCS_HEALER_NAME "thran"
#define _LANG_NPCS_HEALER_SHORT "Thran, el Clérigo"
#define _LANG_NPCS_HEALER_ALIASES ({ "Thran", "clérigo", "clerigo" })
#define _LANG_NPCS_HEALER_PLURAL "Clérigos"
#define _LANG_NPCS_HEALER_PLURAL_ALIASES ({ "clérigos", "clérigos" })
#define _LANG_NPCS_HEALER_LONG "Es un clérigo humano, orador del Dios Lummen. Está dotado de poder " + \
    "para devolver las almas a sus cuerpos originales. Tiene un aspecto " + \
    "misterioso y mágico. Escribe 'info' para ver en qué puede ayudarte.\n"
#define _LANG_NPCS_HEALER_RAISE_COMMANDS ({ "resucitar", "resucitarme" })
#define _LANG_NPCS_HEALER_INFO_COMMANDS ({ "info", "información", "informacion" })
#define _LANG_NPCS_HEALER_INFO "Uhmm, si tu devoción hacia el Dios Lummen es pura " + \
    "podrás resucitar. Escribe 'resucitar' y hallarás respuesta.\n"
#define _LANG_NPCS_HEALER_CHAT ({ "'Si deseas saber como volver a tu forma mortal escribe 'info'.", \
    ":realiza una oracion al dios Lummen.", \
    "'Debes ser un buen devoto." })
#define _LANG_NPCS_HEALER_RAISE_NO "¿Por qué quieres resucitar si no lo necesitas?\n"
#define _LANG_NPCS_HEALER_RAISE_TOO_SOON "Resucitaste hace poco, tendrás que esperar un poco más.\n"
#define _LANG_NPCS_HEALER_RAISE_YES_ME "Thran levanta sus manos invocando los poderes de los Dioses, y apareces de nuevo en tu forma mortal.\n"
#define _LANG_NPCS_HEALER_RAISE_YES_ROOM this_player()->query_cap_name()+" aparece de nuevo en su forma mortal.\n"

#define _LANG_NPCS_FARMER_NAME "granjero"
#define _LANG_NPCS_FARMER_LONG "Un granjero de la región. Es un hombre común " + \
    "y bastante agradable.\n" 
#define _LANG_NPCS_FARMER_CHAT ({ ":te saluda cordialmente.", \
                                  ":comenta algo sobre la cosecha de maíz de este año." })
