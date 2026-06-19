
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

#endif // _LANG_PROPS_H_ES
