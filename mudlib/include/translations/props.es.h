
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

#define _LANG_PROP_CHAIR_SIT_VERBS          ({ "sentar", "sentarse", "sentarme" })
#define _LANG_PROP_CHAIR_STAND_VERBS        ({ "levantar", "levantarse", "levantarme" })
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

#define _LANG_PROP_TABLE_LEAN_VERBS     ({ "apoyarse", "apoyarme", "apoyar" })
#define _LANG_PROP_TABLE_CLIMB_VERBS    ({ "subir", "trepar" })

#define _LANG_PROP_TABLE_SMELL          "La mesa huele a cera y madera vieja."
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
//  estatua abandonada
// ************************************************************
#define _LANG_PROP_RUINED_STATUE_ID          "estatua"
#define _LANG_PROP_RUINED_STATUE_ID_ALIAS_1  "placa"

#define _LANG_PROP_RUINED_STATUE_NOUN        "estatua abandonada"
#define _LANG_PROP_RUINED_STATUE_NOUN_PLURAL "estatuas abandonadas"

#define _LANG_PROP_RUINED_STATUE_SHORT       "una estatua $material_phrase$ abandonada"
#define _LANG_PROP_RUINED_STATUE_LONG        "Una vieja estatua $material_phrase$ se alza aquí, abandonada en mitad de la nada. El tiempo ha borrado las facciones de la figura y le ha quebrado un brazo; está agrietada y cubierta de líquenes, y las plantas y las enredaderas crecen a su alrededor y trepan por ella hasta los hombros. En el pedestal, medio oculta por la maleza, hay una placa."

#define _LANG_PROP_RUINED_STATUE_SMELL       "Huele a musgo y a tierra húmeda."
#define _LANG_PROP_RUINED_STATUE_FEEL        "Está fría y áspera, y las enredaderas se te enganchan en los dedos."

#define _LANG_PROP_RUINED_STATUE_READ_VERBS  ({ "leer" })
#define _LANG_PROP_RUINED_STATUE_READ_ME     "Apartas las enredaderas que cubren el pedestal y descubres la placa."
#define _LANG_PROP_RUINED_STATUE_READ_OTHERS "$mcname$ aparta las enredaderas del pie de la estatua y lee algo."
#define _LANG_PROP_RUINED_STATUE_READ_NOTHING "Si alguna vez hubo algo escrito en la placa, el tiempo lo ha borrado."

#define _LANG_PROP_RUINED_STATUE_PRAY_VERBS  ({ "rezar", "orar" })
#define _LANG_PROP_RUINED_STATUE_PRAY_ME     "Te arrodillas ante la estatua abandonada. Nadie parece haberlo hecho en mucho tiempo."
#define _LANG_PROP_RUINED_STATUE_PRAY_OTHERS "$mcname$ se arrodilla ante la estatua abandonada y reza en silencio."

// ************************************************************
//  charco
// ************************************************************
#define _LANG_PROP_PUDDLE_ID            "charco"
#define _LANG_PROP_PUDDLE_ID_ALIAS_1    "poza"

#define _LANG_PROP_PUDDLE_NOUN          "charco"
#define _LANG_PROP_PUDDLE_NOUN_PLURAL   "charcos"

#define _LANG_PROP_PUDDLE_SHORT         "un charco"
#define _LANG_PROP_PUDDLE_LONG          "Un charco de agua turbia se ha quedado estancado en un hueco del suelo, con los bordes de barro."
#define _LANG_PROP_PUDDLE_SMELL         "Huele a barro y a agua estancada."

#define _LANG_PROP_PUDDLE_STEP_VERBS    ({ "pisar", "chapotear" })
#define _LANG_PROP_PUDDLE_STEP_ME       "Metes un pie en el charco y el agua fría te empapa la bota."
#define _LANG_PROP_PUDDLE_STEP_OTHERS   "$mcname$ mete un pie en el charco y salpica barro alrededor."

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

#define _LANG_PROP_FOUNTAIN_DRINK_ME    "Bebes un poco de agua fresca de la fuente."
#define _LANG_PROP_FOUNTAIN_DRINK_OTHERS "$mcname$ bebe un poco de agua fresca de la fuente."
#define _LANG_PROP_FOUNTAIN_SMELL       "El agua huele levemente a musgo y a piedra fría."

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

#define _LANG_PROP_BAR_LEAN_VERBS       ({ "apoyarse", "apoyarme", "apoyar" })

#define _LANG_PROP_BAR_LEAN_ME          "Te apoyas en la barra."
#define _LANG_PROP_BAR_LEAN_OTHERS      "$mcname$ se apoya en la barra."
#define _LANG_PROP_BAR_SMELL            "La barra huele a cerveza rancia y madera vieja."

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

#define _LANG_PROP_BUNK_LIE_VERBS       ({ "tumbarse", "tumbarme", "acostarse", "acostarme", "echarse", "echarme" })
#define _LANG_PROP_BUNK_RISE_VERBS      ({ "levantarse", "levantarme", "incorporarse", "incorporarme" })

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
#define _LANG_PROP_RACK_SEARCH_ME       "Rebuscas entre las capas: nada que no sea lana vieja y correas gastadas."
#define _LANG_PROP_RACK_SEARCH_OTHERS   "$mcname$ rebusca entre las capas colgadas."
#define _LANG_PROP_RACK_SMELL           "Las capas huelen a sudor, cuero y humo de hoguera."

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

// yunque
#define _LANG_PROP_ANVIL_ID             "yunque"
#define _LANG_PROP_ANVIL_ID_ALIAS_1     "bigornia"
#define _LANG_PROP_ANVIL_NOUN           "yunque"
#define _LANG_PROP_ANVIL_NOUN_PLURAL    "yunques"
#define _LANG_PROP_ANVIL_SHORT          "un yunque"
#define _LANG_PROP_ANVIL_LONG           "Un yunque $material_phrase$, pesado y negro, sobre un tocón de roble. La cara está pulida por años de golpes y el cuerno, gastado de doblar hierro."

#define _LANG_PROP_ANVIL_STRIKE_VERBS   ({ "golpear", "tocar" })
#define _LANG_PROP_ANVIL_STRIKE_ME      "Golpeas el yunque. Suena hondo y largo, como una campana sorda."
#define _LANG_PROP_ANVIL_STRIKE_OTHERS  "$mcname$ golpea el yunque, que suena como una campana sorda."

// fuelle
#define _LANG_PROP_BELLOWS_ID           "fuelle"
#define _LANG_PROP_BELLOWS_ID_ALIAS_1   "fuelles"
#define _LANG_PROP_BELLOWS_NOUN         "fuelle"
#define _LANG_PROP_BELLOWS_NOUN_PLURAL  "fuelles"
#define _LANG_PROP_BELLOWS_SHORT        "un fuelle"
#define _LANG_PROP_BELLOWS_LONG         "Un fuelle grande $material_phrase$, de tapas de madera, colgado junto a la fragua y unido a ella por una tobera de hierro. Se acciona con una palanca larga."

#define _LANG_PROP_BELLOWS_PUMP_VERBS   ({ "accionar", "soplar", "bombear" })
#define _LANG_PROP_BELLOWS_PUMP_ME      "Bajas la palanca un par de veces. El fuelle resopla y el aire silba por la tobera hacia la fragua."
#define _LANG_PROP_BELLOWS_PUMP_OTHERS  "$mcname$ acciona el fuelle, que resopla con fuerza hacia la fragua."

// fragua
#define _LANG_PROP_FORGE_ID             "fragua"
#define _LANG_PROP_FORGE_ID_ALIAS_1     "forja"
#define _LANG_PROP_FORGE_NOUN           "fragua"
#define _LANG_PROP_FORGE_NOUN_PLURAL    "fraguas"
#define _LANG_PROP_FORGE_SHORT          "una fragua"
#define _LANG_PROP_FORGE_LONG           "Una fragua $material_phrase$, ancha y baja, con el hogar abierto bajo una campana ennegrecida por el humo."
#define _LANG_PROP_FORGE_SUFFIX_LIT     " (encendida)"
#define _LANG_PROP_FORGE_LONG_LIT       " En ella arde un fuego vivo, y el calor se nota desde la puerta."
#define _LANG_PROP_FORGE_LONG_UNLIT     " Está apagada y las brasas, frías; nadie trabaja hoy el yunque."
#define _LANG_PROP_FORGE_LONG_UNLIT_PLURAL " Están apagadas y las brasas, frías."

#define _LANG_PROP_FORGE_WARM_VERBS     ({ "calentarse", "calentarme", "calentar" })
#define _LANG_PROP_FORGE_WARM_ME        "Te acercas a la fragua y extiendes las manos hacia el fuego. El calor te llega hasta los huesos."
#define _LANG_PROP_FORGE_WARM_OTHERS    "$mcname$ se acerca a la fragua a calentarse las manos."
#define _LANG_PROP_FORGE_NOT_LIT        "La fragua está apagada; no da ningún calor."

// herramientas
#define _LANG_PROP_TOOLS_ID             "herramientas"
#define _LANG_PROP_TOOLS_ID_ALIAS_1     "herramienta"
#define _LANG_PROP_TOOLS_NOUN           "juego de herramientas"
#define _LANG_PROP_TOOLS_NOUN_PLURAL    "juegos de herramientas"
#define _LANG_PROP_TOOLS_SHORT          "un juego de herramientas"
#define _LANG_PROP_TOOLS_LONG           "De unas clavijas en la pared cuelgan tenazas de varios tamaños, martillos, punzones y limas, todos ennegrecidos por el uso y cada uno en su sitio."

#define _LANG_PROP_TOOLS_HANDLE_VERBS   ({ "descolgar", "coger", "tocar" })
#define _LANG_PROP_TOOLS_HANDLE_ME      "Descuelgas unas tenazas y las sopesas; pesan más de lo que parece. Las vuelves a dejar en su clavija."
#define _LANG_PROP_TOOLS_HANDLE_OTHERS  "$mcname$ descuelga unas tenazas, las sopesa y las vuelve a colgar."

// estantería
#define _LANG_PROP_SHELF_ID             "estantería"
#define _LANG_PROP_SHELF_ID_ALIAS_1     "estanteria"
#define _LANG_PROP_SHELF_NOUN           "estantería"
#define _LANG_PROP_SHELF_NOUN_PLURAL    "estanterías"
#define _LANG_PROP_SHELF_SHORT          "una estantería"
#define _LANG_PROP_SHELF_LONG           "Una estantería $material_phrase$ junto a la puerta, con lo que se vende colocado a la vista en sus baldas."

#define _LANG_PROP_SHELF_BROWSE_VERBS   ({ "rebuscar", "ojear" })
#define _LANG_PROP_SHELF_BROWSE_ME      "Echas un vistazo a las baldas. Lo que hay a la venta, hay que pedírselo a quien atiende."
#define _LANG_PROP_SHELF_BROWSE_OTHERS  "$mcname$ echa un vistazo a las baldas de la estantería."

// pozo
#define _LANG_PROP_WELL_ID              "pozo"
#define _LANG_PROP_WELL_ID_ALIAS_1      "brocal"
#define _LANG_PROP_WELL_NOUN            "pozo"
#define _LANG_PROP_WELL_NOUN_PLURAL     "pozos"
#define _LANG_PROP_WELL_SHORT           "un pozo"
#define _LANG_PROP_WELL_LONG            "Un pozo de brocal $material_phrase$, gastado por el roce de las cuerdas. Un cubo de madera cuelga de una polea sobre la boca."

#define _LANG_PROP_WELL_LEAN_VERBS      ({ "asomarse", "asomarme", "asomar" })
#define _LANG_PROP_WELL_LEAN_ME         "Te asomas al brocal. Abajo, muy hondo, brilla el agua, y tu voz vuelve de ella un poco más grave."
#define _LANG_PROP_WELL_LEAN_OTHERS     "$mcname$ se asoma al pozo."

#define _LANG_PROP_WELL_DRINK_VERBS     ({ "beber" })
#define _LANG_PROP_WELL_DRINK_ME        "Subes el cubo del pozo y bebes un trago. El agua está fría y sabe a piedra."
#define _LANG_PROP_WELL_DRINK_OTHERS    "$mcname$ sube el cubo del pozo y bebe un trago."

// abrevadero
#define _LANG_PROP_TROUGH_ID            "abrevadero"
#define _LANG_PROP_TROUGH_ID_ALIAS_1    "pilón"
#define _LANG_PROP_TROUGH_NOUN          "abrevadero"
#define _LANG_PROP_TROUGH_NOUN_PLURAL   "abrevaderos"
#define _LANG_PROP_TROUGH_SHORT         "un abrevadero"
#define _LANG_PROP_TROUGH_LONG          "Un abrevadero $material_phrase$, largo y bajo, lleno de agua hasta el borde. Tiene el fondo verde de verdín y la orilla gastada por los animales."

#define _LANG_PROP_TROUGH_DRINK_VERBS   ({ "beber" })
#define _LANG_PROP_TROUGH_DRINK_ME      "Haces cuenco con las manos y bebes del abrevadero. El agua está tibia y sabe a hierba."
#define _LANG_PROP_TROUGH_DRINK_OTHERS  "$mcname$ se inclina sobre el abrevadero y bebe del agua de los animales."

// comedero
#define _LANG_PROP_MANGER_ID            "comedero"
#define _LANG_PROP_MANGER_ID_ALIAS_1    "pesebre"
#define _LANG_PROP_MANGER_NOUN          "comedero"
#define _LANG_PROP_MANGER_NOUN_PLURAL   "comederos"
#define _LANG_PROP_MANGER_SHORT         "un comedero"
#define _LANG_PROP_MANGER_LONG          "Un comedero $material_phrase$, bajo y alargado, con restos de grano y paja en el fondo y la madera alisada por el uso."

#define _LANG_PROP_MANGER_SMELL         "Hueles el comedero. Huele a grano, a paja húmeda y, sobre todo, a animal."

// Palabras que un jugador antepone al nombre del objeto y que no forman parte
// de él: "tumbarse EN EL camastro". Se descartan por delante del argumento
// antes de buscar a qué prop se refiere.
#define _LANG_PROPS_FILLER_WORDS ({ "en", "sobre", "encima", "dentro", "a", \
                                    "al", "de", "del", "la", "el", "los", \
                                    "las", "un", "una", "unos", "unas" })

#endif // _LANG_PROPS_H_ES
