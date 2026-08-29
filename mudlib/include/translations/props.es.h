
// props.es.h — cadenas en castellano para el subsistema de location-props.
//
// Agrupado por tipo de prop. Las cadenas independientes de tipo
// (cabecera, joiners, mensajes del despachador) viven al inicio. Cada
// sección de un tipo agrupa ids, sustantivo, descripciones, arrays
// de verbos, mensajes de acción y suffixes de estado — todo lo que
// el prop necesita.
//
// El código de juego usa `#include <translations/props.h>` y el
// driver reescribe el include a .en.h / .es.h según GLOBAL_COMPILE_LANG.
// Ver /lib/core/driver.c::include_file.

#ifndef _LANG_PROPS_H_ES
#define _LANG_PROPS_H_ES

// ************************************************************
//  Cadenas independientes del tipo (despachador, joiners).
// ************************************************************

// Cabecera de la sección, renderizada al inicio de la línea de props
// en la salida de look.
#define _LANG_PROPS_SECTION_HEADER    "Ves"

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

// Plantillas de frase nominal usadas por hook_long del componente
// para describir las instancias en el cuerpo de la location.
// Variables en scope en el call site: count_word, noun,
// material_phrase. El orden difiere por idioma; ver props.en.h.
#define _LANG_PROPS_NOUN_PHRASE             count_word + " " + noun + " " + material_phrase
#define _LANG_PROPS_NOUN_PHRASE_NO_MATERIAL count_word + " " + noun

// Plantilla de id compuesto con material usada por el componente
// para enumerar sus alias en el parser. Variables en scope: id,
// material_phrase. En castellano la frase de material va detrás
// ("silla de madera"); en inglés delante ("wooden chair"). Mismo
// orden de variables, distinta concatenación por idioma.
#define _LANG_PROPS_ID_WITH_MATERIAL        id + " " + material_phrase

// ************************************************************
//  silla
// ************************************************************
#define _LANG_PROP_CHAIR_ID             "silla"
#define _LANG_PROP_CHAIR_ID_ALIAS_1     "asiento"

#define _LANG_PROP_CHAIR_NOUN           "silla"
#define _LANG_PROP_CHAIR_NOUN_PLURAL    "sillas"

#define _LANG_PROP_CHAIR_SHORT          "una silla $material_phrase$"
#define _LANG_PROP_CHAIR_LONG           "Una silla robusta $material_phrase$, para una persona."

#define _LANG_PROP_CHAIR_SUFFIX_TIPPED      " (tirada en el suelo)"
#define _LANG_PROP_CHAIR_SUFFIX_OCCUPIED    " (%s está sentado en ella)"
#define _LANG_PROP_CHAIR_SUFFIX_TIPPED_PLURAL   " (tiradas en el suelo)"

#define _LANG_PROP_CHAIR_LONG_TIPPED        " Está tirada en el suelo."
#define _LANG_PROP_CHAIR_LONG_OCCUPIED      " %s está sentado en ella."
#define _LANG_PROP_CHAIR_LONG_TIPPED_PLURAL " Están tiradas en el suelo."

#define _LANG_PROP_CHAIR_SIT_VERBS          ({ "sentar", "sentarse" })
#define _LANG_PROP_CHAIR_STAND_VERBS        ({ "levantar", "levantarse" })
#define _LANG_PROP_CHAIR_TIP_VERBS          ({ "tirar", "volcar" })
#define _LANG_PROP_CHAIR_RIGHT_VERBS        ({ "enderezar" })

#define _LANG_PROP_CHAIR_SIT_ME             "Te sientas en la silla."
#define _LANG_PROP_CHAIR_SIT_OTHERS         "$mcname$ se sienta en la silla."
#define _LANG_PROP_CHAIR_STAND_ME           "Te levantas."
#define _LANG_PROP_CHAIR_STAND_OTHERS       "$mcname$ se levanta."
#define _LANG_PROP_CHAIR_TIP_ME             "Tiras la silla al suelo."
#define _LANG_PROP_CHAIR_TIP_OTHERS         "$mcname$ tira la silla al suelo."
#define _LANG_PROP_CHAIR_RIGHT_ME           "Levantas la silla."
#define _LANG_PROP_CHAIR_RIGHT_OTHERS       "$mcname$ levanta la silla."

#define _LANG_PROP_CHAIR_CANT_SIT           "No puedes sentarte en esta silla ahora mismo."
#define _LANG_PROP_CHAIR_ALREADY_SITTING "Ya estás sentado en esa silla."
#define _LANG_PROP_CHAIR_OCCUPIED_BY        "$lastarg$ ya está sentado en ella."
#define _LANG_PROP_CHAIR_NOT_SITTING        "No estás sentado en esta silla."
#define _LANG_PROP_CHAIR_NAILED_OR_TIPPED   "No puedes tirar esta silla."
#define _LANG_PROP_CHAIR_NOT_TIPPED         "La silla no está tirada en el suelo."

// ************************************************************
//  mesa
// ************************************************************
#define _LANG_PROP_TABLE_ID             "mesa"

#define _LANG_PROP_TABLE_NOUN           "mesa"
#define _LANG_PROP_TABLE_NOUN_PLURAL    "mesas"

#define _LANG_PROP_TABLE_SHORT          "una mesa $material_phrase$"
#define _LANG_PROP_TABLE_LONG           "Una larga mesa $material_phrase$, marcada por años de uso."

#define _LANG_PROP_TABLE_SMELL_VERBS    ({ "oler", "olfatear" })
#define _LANG_PROP_TABLE_LEAN_VERBS     ({ "apoyarse", "apoyar" })
#define _LANG_PROP_TABLE_CLIMB_VERBS    ({ "subir", "trepar" })

#define _LANG_PROP_TABLE_SMELL_ME       "La mesa huele a cera y madera vieja."
#define _LANG_PROP_TABLE_LEAN_ME        "Te apoyas en la mesa."
#define _LANG_PROP_TABLE_LEAN_OTHERS    "$mcname$ se apoya en la mesa."
#define _LANG_PROP_TABLE_CLIMB_ME       "Te subes a la mesa."
#define _LANG_PROP_TABLE_CLIMB_OTHERS   "$mcname$ se sube a la mesa."

// ************************************************************
//  estatua
// ************************************************************
#define _LANG_PROP_STATUE_ID            "estatua"
#define _LANG_PROP_STATUE_ID_ALIAS_1    "idolo"

#define _LANG_PROP_STATUE_NOUN          "estatua"
#define _LANG_PROP_STATUE_NOUN_PLURAL   "estatuas"

#define _LANG_PROP_STATUE_SHORT         "una estatua $material_phrase$"
#define _LANG_PROP_STATUE_LONG          "Una estatua $material_phrase$ se alza en este lugar, representando a una figura desconocida."

#define _LANG_PROP_STATUE_PRAY_VERBS    ({ "rezar", "orar" })

#define _LANG_PROP_STATUE_PRAY_ME       "Te arrodillas ante la estatua y ofreces una oración en silencio."
#define _LANG_PROP_STATUE_PRAY_OTHERS   "$mcname$ se arrodilla ante la estatua y reza en silencio."

// ************************************************************
//  altar
// ************************************************************
#define _LANG_PROP_ALTAR_ID             "altar"

#define _LANG_PROP_ALTAR_NOUN           "altar"
#define _LANG_PROP_ALTAR_NOUN_PLURAL    "altares"

#define _LANG_PROP_ALTAR_SHORT          "un altar de piedra"
#define _LANG_PROP_ALTAR_LONG           "Un modesto altar de piedra pulida."

#define _LANG_PROP_ALTAR_PRAY_VERBS     ({ "rezar", "orar" })

#define _LANG_PROP_ALTAR_PRAY_ME        "Inclinas la cabeza ante el altar."
#define _LANG_PROP_ALTAR_PRAY_OTHERS    "$mcname$ inclina la cabeza ante el altar."

// ************************************************************
//  fuente
// ************************************************************
#define _LANG_PROP_FOUNTAIN_ID          "fuente"
#define _LANG_PROP_FOUNTAIN_ID_ALIAS_1  "fuente de agua"

#define _LANG_PROP_FOUNTAIN_NOUN        "fuente"
#define _LANG_PROP_FOUNTAIN_NOUN_PLURAL "fuentes"

#define _LANG_PROP_FOUNTAIN_SHORT       "una fuente $material_phrase$"
#define _LANG_PROP_FOUNTAIN_LONG        "Una fuente $material_phrase$. Agua clara burbujea suavemente en su pilón."

#define _LANG_PROP_FOUNTAIN_DRINK_VERBS ({ "beber" })
#define _LANG_PROP_FOUNTAIN_SMELL_VERBS ({ "oler", "olfatear" })

#define _LANG_PROP_FOUNTAIN_DRINK_ME    "Bebes un poco de agua fresca de la fuente."
#define _LANG_PROP_FOUNTAIN_DRINK_OTHERS "$mcname$ bebe un poco de agua fresca de la fuente."
#define _LANG_PROP_FOUNTAIN_SMELL_ME    "El agua huele levemente a musgo y a piedra fría."

// ************************************************************
//  chimenea
// ************************************************************
#define _LANG_PROP_FIREPLACE_ID         "chimenea"
#define _LANG_PROP_FIREPLACE_ID_ALIAS_1 "hogar"

#define _LANG_PROP_FIREPLACE_NOUN       "chimenea"
#define _LANG_PROP_FIREPLACE_NOUN_PLURAL "chimeneas"

#define _LANG_PROP_FIREPLACE_SHORT      "una pequeña chimenea"
#define _LANG_PROP_FIREPLACE_LONG       "Una pequeña chimenea $material_phrase$."

#define _LANG_PROP_FIREPLACE_SUFFIX_LIT " (encendida)"

#define _LANG_PROP_FIREPLACE_LONG_LIT   " Las llamas crepitan en su interior, bañando la sala de luz cálida."
#define _LANG_PROP_FIREPLACE_LONG_UNLIT " Las cenizas están frías; nadie la ha encendido en bastante tiempo."
#define _LANG_PROP_FIREPLACE_LONG_UNLIT_PLURAL " Las cenizas están frías; nadie las ha encendido en bastante tiempo."

#define _LANG_PROP_FIREPLACE_LIGHT_VERBS ({ "encender", "prender" })
#define _LANG_PROP_FIREPLACE_EXT_VERBS  ({ "apagar", "extinguir" })

#define _LANG_PROP_FIREPLACE_LIGHT_ME       "Enciendes la chimenea. Las llamas cobran vida."
#define _LANG_PROP_FIREPLACE_LIGHT_OTHERS   "$mcname$ enciende la chimenea."
#define _LANG_PROP_FIREPLACE_EXT_ME         "Apagas el fuego. El hogar se oscurece."
#define _LANG_PROP_FIREPLACE_EXT_OTHERS     "$mcname$ apaga el fuego."
#define _LANG_PROP_FIREPLACE_ALREADY_LIT    "La chimenea ya está encendida."
#define _LANG_PROP_FIREPLACE_NOT_LIT        "La chimenea no está encendida."

// ************************************************************
//  barra (mostrador de taberna)
// ************************************************************
#define _LANG_PROP_BAR_ID               "barra"
#define _LANG_PROP_BAR_ID_ALIAS_1       "mostrador"

#define _LANG_PROP_BAR_NOUN             "barra"
#define _LANG_PROP_BAR_NOUN_PLURAL      "barras"

#define _LANG_PROP_BAR_SHORT            "una larga barra $material_phrase$"
#define _LANG_PROP_BAR_LONG             "Una larga barra $material_phrase$ recorre la pared, su superficie marcada por años de jarras y codos. No está muy limpia."

#define _LANG_PROP_BAR_LEAN_VERBS       ({ "apoyarse", "apoyar" })
#define _LANG_PROP_BAR_SMELL_VERBS      ({ "oler", "olfatear" })

#define _LANG_PROP_BAR_LEAN_ME          "Te apoyas en la barra."
#define _LANG_PROP_BAR_LEAN_OTHERS      "$mcname$ se apoya en la barra."
#define _LANG_PROP_BAR_SMELL_ME         "La barra huele a cerveza rancia y madera vieja."

// ------------------------------------------------------------
//  camastro
// ------------------------------------------------------------
#define _LANG_PROP_BUNK_ID              "camastro"
#define _LANG_PROP_BUNK_ID_ALIAS_1      "catre"
#define _LANG_PROP_BUNK_NOUN            "camastro"
#define _LANG_PROP_BUNK_NOUN_PLURAL     "camastros"
#define _LANG_PROP_BUNK_SHORT           "un camastro $material_phrase$"
#define _LANG_PROP_BUNK_LONG            "Un camastro estrecho $material_phrase$, con la manta doblada a los pies."

#define _LANG_PROP_BUNK_SUFFIX_OCCUPIED " (%s está tumbado en él)"
#define _LANG_PROP_BUNK_LONG_OCCUPIED   " %s descansa en él."

#define _LANG_PROP_BUNK_LIE_VERBS       ({ "tumbarse", "acostarse", "echarse" })
#define _LANG_PROP_BUNK_RISE_VERBS      ({ "levantarse", "incorporarse" })

#define _LANG_PROP_BUNK_OCCUPIED_BY     "Ya hay alguien tumbado ahí."
#define _LANG_PROP_BUNK_ALREADY_LYING   "Ya estás tumbado en ese camastro."
#define _LANG_PROP_BUNK_NOT_LYING       "No estás tumbado en el camastro."
#define _LANG_PROP_BUNK_LIE_ME          "Te tumbas en el camastro."
#define _LANG_PROP_BUNK_LIE_OTHERS      "$mcname$ se tumba en el camastro."
#define _LANG_PROP_BUNK_RISE_ME         "Te levantas del camastro."
#define _LANG_PROP_BUNK_RISE_OTHERS     "$mcname$ se levanta del camastro."

// ------------------------------------------------------------
//  perchero
// ------------------------------------------------------------
#define _LANG_PROP_RACK_ID              "perchero"
#define _LANG_PROP_RACK_ID_ALIAS_1      "clavijas"
#define _LANG_PROP_RACK_NOUN            "perchero"
#define _LANG_PROP_RACK_NOUN_PLURAL     "percheros"
#define _LANG_PROP_RACK_SHORT           "un perchero"
#define _LANG_PROP_RACK_LONG            "Una hilera de clavijas $material_phrase$ fijadas a la pared. De ellas cuelgan capas y cinturones."

#define _LANG_PROP_RACK_SEARCH_VERBS    ({ "registrar", "rebuscar" })
#define _LANG_PROP_RACK_SMELL_VERBS     ({ "oler", "olfatear" })
#define _LANG_PROP_RACK_SEARCH_ME       "Rebuscas entre las capas: nada que no sea lana vieja y correas gastadas."
#define _LANG_PROP_RACK_SEARCH_OTHERS   "$mcname$ rebusca entre las capas colgadas."
#define _LANG_PROP_RACK_SMELL_ME        "Las capas huelen a sudor, cuero y humo de hoguera."

// ------------------------------------------------------------
//  armero
// ------------------------------------------------------------
#define _LANG_PROP_ARMS_ID              "armero"
#define _LANG_PROP_ARMS_ID_ALIAS_1      "bastidor"
#define _LANG_PROP_ARMS_NOUN            "armero"
#define _LANG_PROP_ARMS_NOUN_PLURAL     "armeros"
#define _LANG_PROP_ARMS_SHORT           "un armero con lanzas"
#define _LANG_PROP_ARMS_LONG            "Un bastidor $material_phrase$ apoyado junto a la puerta, con media docena de lanzas encajadas por el astil y las puntas hacia arriba. Una cadena pasa por todos los astiles y se cierra con candado al bastidor: de aquí no sale ninguna."

#define _LANG_PROP_ARMS_COUNT_VERBS     ({ "contar" })
#define _LANG_PROP_ARMS_COUNT_ME        "Cuentas seis lanzas, todas con la punta limpia y engrasada, y todas encadenadas al bastidor."
#define _LANG_PROP_ARMS_COUNT_OTHERS    "$mcname$ cuenta las lanzas del armero."

// telarañas
#define _LANG_PROP_COBWEB_ID            "telaraña"
#define _LANG_PROP_COBWEB_ID_ALIAS_1    "telarana"
#define _LANG_PROP_COBWEB_NOUN          "telaraña"
#define _LANG_PROP_COBWEB_NOUN_PLURAL   "telarañas"
#define _LANG_PROP_COBWEB_SHORT         "unas telarañas espesas"
#define _LANG_PROP_COBWEB_LONG          "Telarañas más robustas de lo normal cuelgan de rama en rama, tan tupidas que apenas dejan pasar la luz."

#define _LANG_PROP_COBWEB_TOUCH_VERBS   ({ "tocar", "palpar" })
#define _LANG_PROP_COBWEB_TOUCH_ME      "Rozas una hebra con los dedos. Se tensa, vibra, y en algún lugar de la espesura algo responde moviéndose."
#define _LANG_PROP_COBWEB_TOUCH_OTHERS  "$mcname$ roza una hebra de las telarañas y toda la maraña tiembla."

// Palabras que un jugador antepone al nombre del objeto y que no forman parte
// de él: "tumbarse EN EL camastro". Se descartan por delante del argumento
// antes de buscar a qué prop se refiere.
#define _LANG_PROPS_FILLER_WORDS ({ "en", "sobre", "encima", "dentro", "a", \
                                    "al", "de", "del", "la", "el", "los", \
                                    "las", "un", "una", "unos", "unas" })

#endif // _LANG_PROPS_H_ES
