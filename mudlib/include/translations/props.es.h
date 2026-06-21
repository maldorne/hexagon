
// props.es.h — cadenas en castellano para el subsistema de location-props.
//
// Solo viven aquí cadenas independientes de tipo: la cabecera de la
// sección, los separadores de lista y los mensajes genéricos del
// despachador. Las cadenas por tipo (_LANG_PROP_CHAIR_*,
// _LANG_PROP_TABLE_*, ...) viajan con sus entradas / blueprints.
//
// El código de juego usa `#include <translations/props.h>` y el
// driver reescribe el include a .en.h / .es.h según GLOBAL_COMPILE_LANG.
// Ver /lib/core/driver.c::include_file.

#ifndef _LANG_PROPS_H_ES
#define _LANG_PROPS_H_ES

// Cabecera de la sección, renderizada al inicio de la línea de props
// en la salida de look.
#define _LANG_PROPS_SECTION_HEADER    "Ves:"

// Separadores de lista — componen la línea de la sección cuando hay
// varias instancias. Forma final:
// "<cabecera> <a><sep><b><sep_final><c>."
#define _LANG_PROPS_LIST_SEPARATOR    ", "
#define _LANG_PROPS_LIST_AND          " y "
#define _LANG_PROPS_LIST_TERMINATOR   "."

// Mensajes del despachador — usados en
// /lib/location/components/props.c cuando do_prop_action no puede
// satisfacer el verbo.
#define _LANG_PROPS_NO_TARGET         "No hay nada parecido aquí."
#define _LANG_PROPS_AMBIGUOUS_TARGET  "¿A cuál te refieres?"
#define _LANG_PROPS_REMOVED_ACTION    "No puedes hacer eso con esto."
#define _LANG_PROPS_NO_HANDLER        "No ocurre nada."

// Se añade bajo la descripción larga en `look <prop>`. Seguido por
// la lista de verbos separada por comas. "Puedes: encender, apagar."
#define _LANG_PROPS_YOU_CAN           "Puedes: "
#define _LANG_PROPS_NO_ACTIONS        "No hay nada que hacer con esto."

// ************************************************************
//  Arrays de verbos por acción.
//
//  Cada acción del catálogo tiene una lista de sinónimos por
//  idioma. Acciones con un solo verbo usan un array de un elemento.
//  La clave en props_table.c sigue siendo el id canónico (en
//  inglés); estos arrays controlan qué puede teclear el JUGADOR
//  para disparar la acción.
// ************************************************************
#define _LANG_PROP_CHAIR_SIT_VERBS         ({ "sentar", "sentarse" })
#define _LANG_PROP_CHAIR_STAND_VERBS       ({ "levantar", "levantarse" })
#define _LANG_PROP_CHAIR_TIP_VERBS         ({ "tirar", "volcar" })
#define _LANG_PROP_CHAIR_RIGHT_VERBS       ({ "enderezar" })

#define _LANG_PROP_TABLE_SMELL_VERBS       ({ "oler", "olfatear" })
#define _LANG_PROP_TABLE_LEAN_VERBS        ({ "apoyar", "apoyarse" })
#define _LANG_PROP_TABLE_CLIMB_VERBS       ({ "subir", "trepar" })

#define _LANG_PROP_STATUE_PRAY_VERBS       ({ "rezar", "orar" })

#define _LANG_PROP_ALTAR_PRAY_VERBS        ({ "rezar", "orar" })

#define _LANG_PROP_FOUNTAIN_DRINK_VERBS    ({ "beber" })
#define _LANG_PROP_FOUNTAIN_SMELL_VERBS    ({ "oler", "olfatear" })

#define _LANG_PROP_FIREPLACE_LIGHT_VERBS   ({ "encender", "prender" })
#define _LANG_PROP_FIREPLACE_EXT_VERBS     ({ "apagar", "extinguir" })

// ************************************************************
//  Cadenas por tipo — silla
// ************************************************************
#define _LANG_PROP_CHAIR_ID             "silla"
#define _LANG_PROP_CHAIR_ID_ALIAS_1     "asiento"
#define _LANG_PROP_CHAIR_ID_ALIAS_2     "silla de madera"
#define _LANG_PROP_CHAIR_SHORT          "una silla de madera"
#define _LANG_PROP_CHAIR_LONG           "Una silla robusta de madera, para una persona."

#define _LANG_PROP_CHAIR_SUFFIX_TIPPED      " (tirada en el suelo)"
#define _LANG_PROP_CHAIR_SUFFIX_OCCUPIED    " (%s está sentado en ella)"

#define _LANG_PROP_CHAIR_LONG_TIPPED        " Está tirada en el suelo."
#define _LANG_PROP_CHAIR_LONG_OCCUPIED      " %s está sentado en ella."

#define _LANG_PROP_CHAIR_SIT_ME             "Te sientas en la silla."
#define _LANG_PROP_CHAIR_SIT_OTHERS         "%s se sienta en la silla."
#define _LANG_PROP_CHAIR_STAND_ME           "Te levantas."
#define _LANG_PROP_CHAIR_STAND_OTHERS       "%s se levanta."
#define _LANG_PROP_CHAIR_TIP_ME             "Tiras la silla al suelo."
#define _LANG_PROP_CHAIR_TIP_OTHERS         "%s tira la silla al suelo."
#define _LANG_PROP_CHAIR_RIGHT_ME           "Levantas la silla."
#define _LANG_PROP_CHAIR_RIGHT_OTHERS       "%s levanta la silla."

#define _LANG_PROP_CHAIR_CANT_SIT           "No puedes sentarte en esta silla ahora mismo."
#define _LANG_PROP_CHAIR_OCCUPIED_BY        "%s ya está sentado en ella."
#define _LANG_PROP_CHAIR_NOT_SITTING        "No estás sentado en esta silla."
#define _LANG_PROP_CHAIR_NAILED_OR_TIPPED   "No puedes tirar esta silla."
#define _LANG_PROP_CHAIR_NOT_TIPPED         "La silla no está tirada en el suelo."

// ************************************************************
//  Cadenas por tipo — mesa
// ************************************************************
#define _LANG_PROP_TABLE_ID             "mesa"
#define _LANG_PROP_TABLE_ID_ALIAS_1     "mesa de madera"
#define _LANG_PROP_TABLE_SHORT          "una mesa de madera"
#define _LANG_PROP_TABLE_LONG           "Una larga mesa de madera, marcada por años de uso."

#define _LANG_PROP_TABLE_SMELL_ME       "La mesa huele a cera y madera vieja."
#define _LANG_PROP_TABLE_LEAN_ME        "Te apoyas en la mesa."
#define _LANG_PROP_TABLE_LEAN_OTHERS    "%s se apoya en la mesa."
#define _LANG_PROP_TABLE_CLIMB_ME       "Te subes a la mesa."
#define _LANG_PROP_TABLE_CLIMB_OTHERS   "%s se sube a la mesa."

// ************************************************************
//  Cadenas por tipo — estatua
// ************************************************************
#define _LANG_PROP_STATUE_ID            "estatua"
#define _LANG_PROP_STATUE_ID_ALIAS_1    "idolo"
#define _LANG_PROP_STATUE_ID_ALIAS_2    "estatua de metal"
#define _LANG_PROP_STATUE_SHORT         "una estatua de metal"
#define _LANG_PROP_STATUE_LONG          "Forjada en metal, representa al Dios Lummen, portador de las almas y benefactor de los sacerdotes."

#define _LANG_PROP_STATUE_PRAY_ME       "Te arrodillas ante la estatua y ofreces una oración en silencio."
#define _LANG_PROP_STATUE_PRAY_OTHERS   "%s se arrodilla ante la estatua y reza en silencio."

// ************************************************************
//  Cadenas por tipo — altar
// ************************************************************
#define _LANG_PROP_ALTAR_ID             "altar"
#define _LANG_PROP_ALTAR_ID_ALIAS_1     "altar de piedra"
#define _LANG_PROP_ALTAR_SHORT          "un altar de piedra"
#define _LANG_PROP_ALTAR_LONG           "Un modesto altar de piedra pulida."

#define _LANG_PROP_ALTAR_PRAY_ME        "Inclinas la cabeza ante el altar."
#define _LANG_PROP_ALTAR_PRAY_OTHERS    "%s inclina la cabeza ante el altar."

// ************************************************************
//  Cadenas por tipo — fuente
// ************************************************************
#define _LANG_PROP_FOUNTAIN_ID          "fuente"
#define _LANG_PROP_FOUNTAIN_ID_ALIAS_1  "fuente de piedra"
#define _LANG_PROP_FOUNTAIN_ID_ALIAS_2  "fuente de agua"
#define _LANG_PROP_FOUNTAIN_SHORT       "una fuente de piedra"
#define _LANG_PROP_FOUNTAIN_LONG        "Una fuente de piedra. Agua clara burbujea suavemente en su pilón."

#define _LANG_PROP_FOUNTAIN_DRINK_ME        "Bebes un poco de agua fresca de la fuente."
#define _LANG_PROP_FOUNTAIN_DRINK_OTHERS    "%s bebe un poco de agua fresca de la fuente."
#define _LANG_PROP_FOUNTAIN_SMELL_ME        "El agua huele levemente a musgo y a piedra fría."

// ************************************************************
//  Cadenas por tipo — chimenea
// ************************************************************
#define _LANG_PROP_FIREPLACE_ID          "chimenea"
#define _LANG_PROP_FIREPLACE_ID_ALIAS_1  "hogar"
#define _LANG_PROP_FIREPLACE_ID_ALIAS_2  "chimenea pequeña"
#define _LANG_PROP_FIREPLACE_SHORT       "una pequeña chimenea"
#define _LANG_PROP_FIREPLACE_LONG        "Una pequeña chimenea de piedra."

#define _LANG_PROP_FIREPLACE_SUFFIX_LIT  " (encendida)"

#define _LANG_PROP_FIREPLACE_LONG_LIT    " Las llamas crepitan en su interior, bañando la sala de luz cálida."
#define _LANG_PROP_FIREPLACE_LONG_UNLIT  " Las cenizas están frías; nadie la ha encendido en bastante tiempo."

#define _LANG_PROP_FIREPLACE_LIGHT_ME       "Enciendes la chimenea. Las llamas cobran vida."
#define _LANG_PROP_FIREPLACE_LIGHT_OTHERS   "%s enciende la chimenea."
#define _LANG_PROP_FIREPLACE_EXT_ME         "Apagas el fuego. El hogar se oscurece."
#define _LANG_PROP_FIREPLACE_EXT_OTHERS     "%s apaga el fuego."
#define _LANG_PROP_FIREPLACE_ALREADY_LIT    "La chimenea ya está encendida."
#define _LANG_PROP_FIREPLACE_NOT_LIT        "La chimenea no está encendida."

#endif // _LANG_PROPS_H_ES
