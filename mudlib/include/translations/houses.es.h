
// houses.es.h — cadenas en castellano para los componentes de solar y casa.
//
// El código usa `#include <translations/houses.h>` y el driver reescribe el
// include a .en.h / .es.h según GLOBAL_COMPILE_LANG. Ver
// /lib/core/driver.c::include_file.

#define _LANG_PLOT_SHORT "Solar vacío"
#define _LANG_PLOT_LONG \
    "Un solar vacío, sin construir. El terreno está despejado y nivelado, " + \
    "listo para levantar sobre él una edificación.\n"

#define _LANG_HOME_SHORT "Casa"
#define _LANG_HOME_LONG \
    "El interior de una vivienda modesta. Las paredes desnudas y el suelo de " + \
    "tierra apisonada aguardan los muebles y enseres de quienes la habiten.\n"
