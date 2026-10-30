
// Cadenas de display por habilidad (español). La clave de aprendizaje es el
// id en inglés de <living/skills.h>; esto es solo el nombre visible y la ayuda.

#define _LANG_SKILL_ORIENTATION_NAME "orientación"
#define _LANG_SKILL_ORIENTATION_ALIASES ({ "orientacion" })
#define _LANG_SKILL_ORIENTATION_HELP "Esta habilidad mide tu capacidad para " + \
    "orientarte y encontrar caminos, siendo capaz de llegar sano y salvo a " + \
    "tu destino.\n"

#define _LANG_SKILL_SEARCH_NAME "buscar"
#define _LANG_SKILL_SEARCH_ALIASES ({ })
#define _LANG_SKILL_SEARCH_HELP "Esta habilidad te permite registrar tu entorno " + \
    "en busca de cosas ocultas. Cuanto mejor se te dé, más cosas encontrarás.\n"
#define _LANG_SKILL_SEARCH_START "Buscas atentamente a tu alrededor, tratando " + \
    "de encontrar algo oculto.\n"
#define _LANG_SKILL_SEARCH_ROOM " busca algo por los alrededores.\n"

#define _LANG_SKILL_HIDE_NAME "esconderse"
#define _LANG_SKILL_HIDE_ALIASES ({ })
#define _LANG_SKILL_HIDE_HELP "Al utilizar esta habilidad intentarás echar un rápido " + \
    "vistazo a tu alrededor para ver si puedes aprovechar algo para esconderte. A " + \
    "mayor habilidad, más fácil te será encontrar dónde ocultarte, y la oscuridad " + \
    "ayuda. Si tienes éxito, el gasto de puntos es el doble.\n"
#define _LANG_SKILL_HIDE_START "Buscas rápidamente un lugar donde poder esconderte."
#define _LANG_SKILL_HIDE_ROUND1 "Tratas de buscar un lugar en el que ocultarte.\n"
#define _LANG_SKILL_HIDE_SUCCESS "Te escondes con sigilo.\n"
#define _LANG_SKILL_HIDE_FAIL "No consigues encontrar ningún lugar en el que esconderte.\n"

// hide shadow (placed on a hidden character)
#define _LANG_HIDE_REVEALED_ROOM_PRE "\n\t%^BOLD%^Notas a "
#define _LANG_HIDE_REVEALED_ROOM_POST " moverse a tu alrededor... ¡estaba escondid" + \
      player->query_vowel() + "!%^RESET%^\n"
#define _LANG_HIDE_REVEALED_YOU "¡Te han descubierto!\n"
#define _LANG_HIDE_SEARCH_FOUND_PRE "Buscas a tu alrededor y encuentras a "
#define _LANG_HIDE_SEARCH_FOUND_POST ", aunque aún no se ha dado cuenta.\n"

#define _LANG_SKILL_CLIMB_NAME "trepar"
#define _LANG_SKILL_CLIMB_ALIASES ({ "escalar" })
#define _LANG_SKILL_CLIMB_HELP "Con 'trepar <algo>' intentas subir por un árbol, " + \
      "una pared o cualquier otra cosa a la que puedas agarrarte. Cuanto mejor " + \
      "tengas la habilidad, menos probable es que resbales, y algunas cosas son " + \
      "más fáciles de subir que otras.\n"
#define _LANG_SKILL_CLIMB_START "Buscas por dónde empezar a subir."
#define _LANG_SKILL_CLIMB_NOT_CLIMBABLE "Ahí no hay por dónde trepar.\n"
#define _LANG_SKILL_CLIMB_ROUND1 "Te agarras bien y empiezas a subir.\n"
#define _LANG_SKILL_CLIMB_ROUND1_ROOM caster->query_cap_name() + \
      " empieza a trepar por " + ob->query_short() + ".\n"
#define _LANG_SKILL_CLIMB_SUCCESS "Llegas arriba sin problemas, echas un vistazo " + \
      "y vuelves a bajar.\n"
#define _LANG_SKILL_CLIMB_FAIL "Resbalas y vuelves al suelo sin haber llegado " + \
      "muy alto.\n"

#define _LANG_SKILL_REPAIR_NAME "reparar"
#define _LANG_SKILL_REPAIR_ALIASES ({ "arreglar" })
#define _LANG_SKILL_REPAIR_HELP "Con 'reparar <objeto>' trabajas en la fragua " + \
      "de una herrería un arma, una armadura o un escudo de metal que lleves " + \
      "encima, para quitarle el desgaste. Solo puede hacerse mientras la fragua " + \
      "está encendida, y hay que pagar el metal y el carbón que se gastan. " + \
      "Cuanto mejor tengas la habilidad, más desgaste quitas de una vez y menos " + \
      "te cuestan los materiales; si fallas, solo pierdes el esfuerzo.\n"
#define _LANG_SKILL_REPAIR_START "Preparas el yunque y avivas el fuego de la fragua."
#define _LANG_SKILL_REPAIR_NO_FORGE "Para reparar necesitas una fragua y un " + \
      "yunque: busca una herrería.\n"
#define _LANG_SKILL_REPAIR_FORGE_COLD "La fragua está apagada; sin nadie que la " + \
      "atienda no puedes trabajar el metal.\n"
#define _LANG_SKILL_REPAIR_NOT_CARRIED "Tienes que llevar encima lo que quieras reparar.\n"
#define _LANG_SKILL_REPAIR_NOT_GEAR "Solo se pueden reparar armas, armaduras y escudos.\n"
#define _LANG_SKILL_REPAIR_NOT_METAL "Eso no es de metal: en una fragua no se arregla.\n"
#define _LANG_SKILL_REPAIR_NOTHING_TO_DO "Está en perfecto estado; no hay nada que reparar.\n"
#define _LANG_SKILL_REPAIR_TOO_POOR "No llevas dinero suficiente para pagar los materiales.\n"
#define _LANG_SKILL_REPAIR_ROUND1 "Calientas " + target->query_short() + \
      " en las brasas hasta que el metal enrojece.\n"
#define _LANG_SKILL_REPAIR_ROUND1_ROOM caster->query_cap_name() + \
      " calienta " + target->query_short() + " en la fragua.\n"
#define _LANG_SKILL_REPAIR_SUCCESS "Golpeas el metal sobre el yunque hasta dejar " + \
      target->query_short() + " en mejor estado. Los materiales te cuestan " + \
      handler("money")->money_string(money) + ".\n"
#define _LANG_SKILL_REPAIR_SUCCESS_ROOM caster->query_cap_name() + \
      " martillea " + target->query_short() + " sobre el yunque y lo deja como nuevo.\n"
#define _LANG_SKILL_REPAIR_FAIL "El metal se enfría antes de tiempo y no consigues " + \
      "arreglar nada.\n"
#define _LANG_SKILL_REPAIR_FAIL_ROOM caster->query_cap_name() + \
      " golpea el yunque sin mucho acierto.\n"

#define _LANG_SKILL_SLICE_NAME "rebanar"
#define _LANG_SKILL_SLICE_ALIASES ({ })
#define _LANG_SKILL_SLICE_HELP "Con 'rebanar <alguien>' haces girar tus dos armas " + \
      "de filo y lanzas sobre tu rival una serie de cortes rápidos como el rayo. " + \
      "Necesitas empuñar dos armas cortantes. Cuantos más destreza y nivel tengas, " + \
      "más cortes das en cada serie; la destreza y lo bien que conozcas la " + \
      "habilidad mejoran la puntería y el daño, y la luz fuerte te estorba. Cada " + \
      "serie de cortes gasta energía.\n"
#define _LANG_SKILL_SLICE_START "Haces girar tus armas buscando un hueco en la guardia de tu rival."
#define _LANG_SKILL_SLICE_START_ROOM "hace girar sus armas."
#define _LANG_SKILL_SLICE_NO_WEAPONS "Necesitas empuñar dos armas de filo para rebanar.\n"
#define _LANG_SKILL_SLICE_LOST_WEAPONS "Te haces un lío con las armas y dejas de rebanar.\n"
#define _LANG_SKILL_SLICE_TIRED "No te quedan fuerzas para seguir rebanando.\n"
#define _LANG_SKILL_SLICE_ME "Rebanas a " + target->query_cap_name() + \
      " con una serie de cortes rápidos.\n"
#define _LANG_SKILL_SLICE_TARGET caster->query_cap_name() + " hace girar sus armas " + \
      "y te rebana con una serie de cortes rápidos.\n"
#define _LANG_SKILL_SLICE_ROOM caster->query_cap_name() + " hace girar sus armas y " + \
      "rebana a " + target->query_cap_name() + " con una serie de cortes rápidos " + \
      "como el rayo.\n"

#define _LANG_SKILL_SWIM_NAME "nadar"
#define _LANG_SKILL_SWIM_ALIASES ({ "bucear" })
#define _LANG_SKILL_SWIM_HELP "Con 'nadar' a secas te metes en el agua desde la " + \
      "orilla, y con 'nadar <dirección>' te mueves por ella; estando en el agua, " + \
      "'nadar' a secas te dice dónde ves la orilla. En el agua no se camina: cada brazada se tira contra tu habilidad, " + \
      "y es más fácil a favor de la corriente que contra ella o en aguas " + \
      "profundas, y más difícil cuanto más peso llevas. Si fallas tragas agua, y " + \
      "si hay corriente te arrastra. Aprendes a nadar al llegar al agua.\n"
#define _LANG_SKILL_SWIM_NO_WATER "Aquí no hay agua en la que nadar.\n"
#define _LANG_SKILL_SWIM_NOT_THERE "No puedes nadar hacia ahí.\n"
#define _LANG_SKILL_SWIM_WHICH_WAY "Hay agua hacia más de un lado: nadar <dirección>.\n"
#define _LANG_SKILL_SWIM_START "Comienzas a nadar."
#define _LANG_SKILL_SWIM_START_ROOM "empieza a nadar."

#define _LANG_SKILL_FOLLOW_NAME "seguir"
#define _LANG_SKILL_FOLLOW_ALIASES ({ })
#define _LANG_SKILL_FOLLOW_HELP "Con 'seguir <alguien>' vas a donde vaya esa persona, " + \
      "por la misma salida y justo detrás, pase lo que pase. Si tiene activado el " + \
      "permiso de que la sigan, la sigues sin más; si no, tu habilidad se mide " + \
      "contra la suya de despistar, y si fallas te da esquinazo. A quien está " + \
      "escondido no se le puede seguir: primero hay que encontrarle. Dejas de " + \
      "seguir con 'detener seguimiento'.\n"
#define _LANG_SKILL_FOLLOW_NOBODY "No ves a nadie así a quien seguir.\n"
#define _LANG_SKILL_FOLLOW_ALREADY "Ya le estás siguiendo.\n"
#define _LANG_SKILL_FOLLOW_CIRCLE "No puedes seguir a quien ya te va siguiendo.\n"
#define _LANG_SKILL_FOLLOW_ME "Empiezas a seguir a " + target->query_cap_name() + ".\n"
#define _LANG_SKILL_FOLLOW_TARGET caster->query_cap_name() + " empieza a seguirte.\n"
#define _LANG_SKILL_FOLLOW_ROOM caster->query_cap_name() + " empieza a seguir a " + \
      target->query_cap_name() + ".\n"
#define _LANG_SKILL_FOLLOW_FAIL_ME target->query_cap_name() + " te da esquinazo.\n"
#define _LANG_SKILL_FOLLOW_FAIL_TARGET caster->query_cap_name() + \
      " intenta seguirte, pero le das esquinazo.\n"

#define _LANG_SKILL_EVADE_NAME "despistar"
#define _LANG_SKILL_EVADE_ALIASES ({ "perder" })
#define _LANG_SKILL_EVADE_HELP "Con 'despistar' intentas quitarte de encima a quienes " + \
      "te siguen y están contigo, y con 'despistar <alguien>' solo a esa persona. " + \
      "Tu habilidad se mide contra la suya de seguir. Cuanto mejor despistas, más " + \
      "difícil es seguirte.\n"
#define _LANG_SKILL_EVADE_NOBODY "Nadie de los que están aquí te va siguiendo.\n"
#define _LANG_SKILL_EVADE_ME "Das esquinazo a " + follower->query_cap_name() + ".\n"
#define _LANG_SKILL_EVADE_FOLLOWER caster->query_cap_name() + \
      " te da esquinazo y le pierdes la pista.\n"
#define _LANG_SKILL_EVADE_FAIL_ME follower->query_cap_name() + " no te pierde de vista.\n"

#define _LANG_SKILL_SWIM_ON_BRIDGE "Estás sobre el puente; si quieres ir al agua, salta.\n"
#define _LANG_SKILL_CLIMB_WHAT "¿Trepar a qué?\n"

#define _LANG_SKILL_STEAL_NAME "robar"
#define _LANG_SKILL_STEAL_ALIASES ({ "hurtar" })
#define _LANG_SKILL_STEAL_HELP "Con 'robar <objeto> a <alguien>' intentas quitarle algo " + \
      "a otra persona sin que se dé cuenta; con 'robar monedas a <alguien>' (o " + \
      "'dinero') te llevas unas pocas monedas, y con 'robar <objeto> de " + \
      "<contenedor> a <alguien>' sacas algo de un contenedor abierto que lleve " + \
      "encima. Tu destreza, tu nivel y tu habilidad se miden contra la atención " + \
      "de la víctima: lo pesado es más difícil, ir escondido ayuda, y quien está " + \
      "alerta o peleando es más difícil de engañar. Si te notan, te llevas lo " + \
      "robado pero lo saben; si te " + \
      "descubren, no te llevas nada, dejas de estar escondido y los monstruos te " + \
      "atacarán. No puedes robar lo que alguien lleva puesto o empuñado, ni a " + \
      "los jugadores de nivel bajo, ni en lugares donde está prohibido pelear, " + \
      "ni mientras luchas.\n"
#define _LANG_SKILL_STEAL_START "Te acercas con disimulo, sin quitar ojo a tu presa."
#define _LANG_SKILL_STEAL_ROUND1 "Estiras la mano con mucho cuidado...\n"
#define _LANG_SKILL_STEAL_SYNTAX "Sintaxis: robar <objeto> a <alguien>, robar monedas a <alguien> " + \
      "o robar <objeto> de <contenedor> a <alguien>.\n"
#define _LANG_SKILL_STEAL_VICTIM_WORD " a "
#define _LANG_SKILL_STEAL_CONTAINER_WORD " de "
#define _LANG_SKILL_STEAL_COIN_WORDS ({ "monedas", "moneda", "dinero" })
#define _LANG_SKILL_STEAL_NO_VICTIM "No ves a nadie así por aquí.\n"
#define _LANG_SKILL_STEAL_SELF "Robarte a ti mismo no tiene mucha gracia.\n"
#define _LANG_SKILL_STEAL_NOT_LIVING "Eso no tiene bolsillos que vaciar.\n"
#define _LANG_SKILL_STEAL_DEAD "A los muertos ya no les queda nada que robar.\n"
#define _LANG_SKILL_STEAL_IMMORTAL "Mejor no te metas a robar a quien no debes.\n"
#define _LANG_SKILL_STEAL_NOSTEAL "No consigues robar eso.\n"
#define _LANG_SKILL_STEAL_PROTECTED "Todavía es demasiado joven para que te ensañes con " + \
      "sus bolsillos.\n"
#define _LANG_SKILL_STEAL_SAFE_PLACE "Aquí no se permiten esas cosas.\n"
#define _LANG_SKILL_STEAL_DARK "No ves lo bastante bien para meter la mano en un bolsillo.\n"
#define _LANG_SKILL_STEAL_NO_ITEM "No encuentras eso encima de esa persona.\n"
#define _LANG_SKILL_STEAL_NO_CONTAINER "No encuentras ningún contenedor así encima de esa persona.\n"
#define _LANG_SKILL_STEAL_CLOSED "Ese contenedor está cerrado.\n"
#define _LANG_SKILL_STEAL_NO_ITEM_IN "No encuentras eso dentro del contenedor.\n"
#define _LANG_SKILL_STEAL_IN_USE "Eso lo lleva puesto o empuñado, no puedes robárselo.\n"
#define _LANG_SKILL_STEAL_MONEY_ITEM "Para llevarte monedas, di 'robar monedas a <alguien>'.\n"
#define _LANG_SKILL_STEAL_NO_COINS "No lleva ni una moneda encima.\n"
#define _LANG_SKILL_STEAL_GONE "Ya no está donde estaba.\n"
#define _LANG_SKILL_STEAL_CANNOT_CARRY "No consigues quedarte con $item$.\n"
#define _LANG_SKILL_STEAL_CLEAN_ME "Le quitas $item$ a $hcname$ sin que se dé cuenta.\n"
#define _LANG_SKILL_STEAL_NOTICED_ME "Le quitas $item$ a $hcname$, pero se da cuenta.\n"
#define _LANG_SKILL_STEAL_NOTICED_VICTIM "¡$mcname$ te acaba de robar $item$!\n"
#define _LANG_SKILL_STEAL_NOTICED_ROOM "$mcname$ le roba $item$ a $hcname$.\n"
#define _LANG_SKILL_STEAL_CAUGHT_ME "$hcname$ te pilla con la mano en el bolsillo.\n"
#define _LANG_SKILL_STEAL_CAUGHT_VICTIM "¡Pillas a $mcname$ intentando robarte!\n"
#define _LANG_SKILL_STEAL_CAUGHT_ROOM "$mcname$ intenta robar a $hcname$, pero lo pillan.\n"

#define _LANG_SKILL_FOCUS_NAME "centrar"
#define _LANG_SKILL_FOCUS_ALIASES ({ })
#define _LANG_SKILL_FOCUS_OFF_WORDS ({ "off", "no", "parar", "fin" })
#define _LANG_SKILL_FOCUS_HELP "Con 'centrar <alguien>' concentras todos tus golpes " + \
      "en uno solo de tus oponentes, aunque la pelea te llevara a otro. Si alguien " + \
      "lo protege, tendrás que golpear antes a su protector. Con 'centrar off' " + \
      "dejas de hacerlo. Tu oponente se fija cada vez más en ti: el doble si tu " + \
      "actitud de combate es defensiva, y nada si es de apoyo. Cuesta un punto de " + \
      "energía por cada asalto, y pierdes la concentración pasados dos turnos por " + \
      "cada nivel tuyo.\n"
#define _LANG_SKILL_FOCUS_SYNTAX "Sintaxis: centrar <objetivo>, centrar off\n"
#define _LANG_SKILL_FOCUS_NOT_FOCUSED "No estás centrando tus golpes en nadie.\n"
#define _LANG_SKILL_FOCUS_NOT_FIGHTING "No estás luchando contra eso.\n"
#define _LANG_SKILL_FOCUS_OFF "Dejas de centrar tus golpes.\n"
#define _LANG_SKILL_FOCUS_START_ME "Empiezas a centrar tus golpes contra " + \
      ob->query_cap_name() + ".\n"
#define _LANG_SKILL_FOCUS_START_ROOM caster->query_cap_name() + " concentra sus " + \
      "golpes contra " + ob->query_cap_name() + ".\n"
#define _LANG_SKILL_FOCUS_TARGET_GONE "Tu objetivo ya no está en la pelea, y dejas " + \
      "de centrar tus golpes.\n"
#define _LANG_SKILL_FOCUS_LOST "Pierdes tu concentración mental.\n"
#define _LANG_SKILL_FOCUS_TIRED "Estás demasiado cansado para seguir centrando tus golpes.\n"

#define _LANG_SKILL_PROTECT_NAME "proteger"
#define _LANG_SKILL_PROTECT_ALIASES ({ })
#define _LANG_SKILL_PROTECT_OFF_WORDS ({ "off", "no", "parar", "fin" })
#define _LANG_SKILL_PROTECT_HELP "Con 'proteger <alguien>' te pones al lado de alguien " + \
      "que esté contigo, y los golpes dirigidos a él pueden caer sobre ti. Tiene que " + \
      "aceptar tu protección. Con 'proteger off <alguien>' dejas de protegerle, y con " + \
      "'proteger off' dejas de proteger a todos los que estén contigo.\n"
#define _LANG_SKILL_PROTECT_SYNTAX "Sintaxis: proteger <objetivo>, proteger off [<objetivo>]\n"
#define _LANG_SKILL_PROTECT_WHO "¿Proteger a quién?\n"
#define _LANG_SKILL_PROTECT_SELF "No puedes protegerte a ti mismo.\n"
#define _LANG_SKILL_PROTECT_REFUSED ob->query_cap_name() + " no quiere tu protección.\n"
#define _LANG_SKILL_PROTECT_ME "Proteges a " + ob->query_short() + ".\n"
#define _LANG_SKILL_PROTECT_HOW_TO_STOP "Utiliza 'proteger off' para dejar de hacerlo.\n"
#define _LANG_SKILL_PROTECT_ROOM caster->query_cap_name() + " protege a " + ob->query_short() + ".\n"
#define _LANG_SKILL_PROTECT_THEM caster->query_cap_name() + " te protege.\n"
#define _LANG_SKILL_PROTECT_NOT_PROTECTING "No estás protegiendo a nadie con ese nombre.\n"
#define _LANG_SKILL_PROTECT_NOBODY "No estás protegiendo a nadie aquí.\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_THEM_HERE caster->query_cap_name() + \
      " se retira y no te protege más.\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_ROOM caster->query_cap_name() + " deja de proteger a " + \
      ob->query_cap_name() + ".\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_THEM caster->query_cap_name() + " deja de protegerte.\n"
#define _LANG_SKILL_PROTECT_UNPROTECT_ME "Te retiras y dejas de proteger a " + ob->query_cap_name() + ".\n"

#define _LANG_SKILL_JUDGE_NAME "juzgar"
#define _LANG_SKILL_JUDGE_ALIASES ({ })
#define _LANG_SKILL_JUDGE_HELP "Con 'juzgar <objeto>' te dices a ti mismo cuánto " + \
      "daño hace un arma que llevas o cuánto protege una armadura o un escudo. " + \
      "No cuesta energía. Tu valoración no siempre es exacta, y mejora con la " + \
      "práctica.\n"
#define _LANG_SKILL_JUDGE_NOT_CARRIED "Solo puedes juzgar lo que llevas contigo.\n"
#define _LANG_SKILL_JUDGE_WORDS ({ "muy mala", "mala", "pobre", "inferior a la media", \
      "mediana", "superior a la media", "buena", "grande", "maravillosa", \
      "increíble", "invencible", "insuperable" })
#define _LANG_SKILL_JUDGE_WEAPON "La capacidad de daño de " + item->query_short() + \
      " es " + word + " (daño de tipo " + type_name + ").\n"
#define _LANG_SKILL_JUDGE_ARMOUR "La protección de " + item->query_short() + " es " + \
      word + ".\n"
#define _LANG_SKILL_JUDGE_USELESS capitalize(target->query_short()) + " te parece " + \
      "bastante inútil.\n"

// pray.c

#define _LANG_SKILL_PRAY_NAME "rezar"
#define _LANG_SKILL_PRAY_ALIASES ({ })
#define _LANG_SKILL_PRAY_HELP "Con 'rezar <plegaria>' pides un favor a tu dios, " + \
      "y solo donde se le rinde culto: en uno de sus templos, o ante un altar o " + \
      "una estatua dedicados a él. Las plegarias que escucha dependen de cada " + \
      "dios; las más comunes son curación (recuperas vida), poder (recuperas " + \
      "energía), bendición (más constitución por un tiempo), fuerza (más fuerza " + \
      "por un tiempo) y sabiduría (más sabiduría e inteligencia por un tiempo). " + \
      "Los dioses no siempre escuchan, y menos cuanto más se les molesta: si " + \
      "rezas demasiado se enfadarán y acabarán por maldecirte. Quien no adora a " + \
      "ningún dios no sabe rezar.\n"
#define _LANG_SKILL_PRAY_START "Elevas una plegaria."
#define _LANG_SKILL_PRAY_START_ROOM "reza en silencio."
#define _LANG_SKILL_PRAY_NO_DEITY "No adoras a ningún dios: no tienes a quién rezar.\n"
#define _LANG_SKILL_PRAY_WRONG_PLACE "No te atreves a rezar aquí: tu $noun$ podría enfadarse.\n"
#define _LANG_SKILL_PRAY_SYNTAX "¿Rezar por qué? Las plegarias que $god$ escucha son: " + \
      "$prayers$.\nSintaxis: rezar <plegaria>\n"
#define _LANG_SKILL_PRAY_CURSED "Estás maldito y no puedes rezar.\n"
#define _LANG_SKILL_PRAY_REFUSED "$god$ rehúye escuchar una plegaria así.\n"

// base-extraction.c, shared by skinning.c, gutting.c, dismembering.c, pulling.c

#define _LANG_SKILL_EXTRACT_SEPARATORS ({ " de ", " del " })
#define _LANG_SKILL_EXTRACT_START "Te inclinas sobre el cuerpo con la hoja en la mano."
#define _LANG_SKILL_EXTRACT_START_ROOM "se inclina sobre un cuerpo con una hoja en la mano."
#define _LANG_SKILL_EXTRACT_SYNTAX "Sintaxis: " + query_effect_name() + " <cuerpo> | " + \
      query_effect_name() + " <parte> de <cuerpo>\n"
#define _LANG_SKILL_EXTRACT_NO_BLADE "Necesitas empuñar un cuchillo o una daga para poder hacer eso.\n"
#define _LANG_SKILL_EXTRACT_NO_CORPSE "No ves ningún cuerpo así por aquí.\n"
#define _LANG_SKILL_EXTRACT_UNKNOWN_RACE "No sabes reconocer qué ser era este, así que no " + \
      "sabrías por dónde empezar.\n"
#define _LANG_SKILL_EXTRACT_NOTHING "No hay nada en este cuerpo que sepas sacar así.\n"
#define _LANG_SKILL_EXTRACT_NO_SUCH_PART "Este cuerpo no tiene nada así.\n"
#define _LANG_SKILL_EXTRACT_WRONG_SKILL "No es así como se saca eso.\n"
#define _LANG_SKILL_EXTRACT_TAKEN "A este cuerpo ya le han sacado eso.\n"
#define _LANG_SKILL_EXTRACT_NOT_SKILLED "Aún no dominas lo suficiente este arte como para " + \
      "intentarlo con este cuerpo.\n"
#define _LANG_SKILL_EXTRACT_BROKEN "Algo falla y no consigues nada.\n"
#define _LANG_SKILL_EXTRACT_DONE "Trabajas el " + corpse_name + " con la hoja y consigues " + \
      part->query_article() + " " + part_name + ".\n"
#define _LANG_SKILL_EXTRACT_DONE_ROOM caster->query_cap_name() + " trabaja el " + corpse_name + \
      " con una hoja y consigue " + part->query_article() + " " + part_name + ".\n"
#define _LANG_SKILL_EXTRACT_RUINED "Se te va la mano y estropeas lo que intentabas sacar del " + \
      corpse_name + ".\n"
#define _LANG_SKILL_EXTRACT_RUINED_ROOM caster->query_cap_name() + " trabaja el " + corpse_name + \
      " con una hoja, pero lo estropea.\n"

// skinning.c

#define _LANG_SKILL_SKINNING_NAME "despellejar"
#define _LANG_SKILL_SKINNING_ALIASES ({ "desollar" })
#define _LANG_SKILL_SKINNING_HELP "Con 'despellejar <cuerpo>', empuñando un cuchillo o una " + \
      "daga, quitas la piel, el plumaje o las escamas de un cuerpo para venderlos o " + \
      "trabajarlos después. Quitar la piel a un animal es lo más fácil; las plumas de " + \
      "un ave piden más destreza, y las escamas de un reptil, más todavía. Mejora con " + \
      "la práctica.\n"

// gutting.c

#define _LANG_SKILL_GUTTING_NAME "destripar"
#define _LANG_SKILL_GUTTING_ALIASES ({ "eviscerar" })
#define _LANG_SKILL_GUTTING_HELP "Con 'destripar <cuerpo>', empuñando un cuchillo o una " + \
      "daga, abres un cuerpo y le sacas el corazón. Un corazón se pudre con el tiempo y " + \
      "pierde su valor. Mejora con la práctica.\n"

// dismembering.c

#define _LANG_SKILL_DISMEMBERING_NAME "desmembrar"
#define _LANG_SKILL_DISMEMBERING_ALIASES ({ "cercenar" })
#define _LANG_SKILL_DISMEMBERING_HELP "Con 'desmembrar <cuerpo>' o 'desmembrar <parte> de " + \
      "<cuerpo>', empuñando un cuchillo o una daga, separas la cabeza, las manos o la cola " + \
      "de un cuerpo. Lo que se corta se pudre con el tiempo y pierde su valor. Mejora con " + \
      "la práctica.\n"

// pulling.c

#define _LANG_SKILL_PULLING_NAME "arrancar"
#define _LANG_SKILL_PULLING_ALIASES ({ })
#define _LANG_SKILL_PULLING_HELP "Con 'arrancar <cuerpo>' o 'arrancar <parte> de <cuerpo>', " + \
      "empuñando un cuchillo o una daga, le sacas a un cuerpo los dientes, los colmillos, " + \
      "las garras o los cuernos. No se pudren. Mejora con la práctica.\n"

// sneak.c

#define _LANG_SKILL_SNEAK_NAME "sigilar"
#define _LANG_SKILL_SNEAK_ALIASES ({ })
#define _LANG_SKILL_SNEAK_HELP "Con 'sigilar <dirección>' das un paso hacia allí sin que " + \
      "nadie vea cómo sales ni cómo entras. Si no hay nadie que pueda verte, siempre lo " + \
      "consigues y no te cuesta nada. Si hay alguien, depende de lo bien que conozcas la " + \
      "habilidad y de tu destreza, la oscuridad ayuda y te cuesta un punto de energía; sin " + \
      "ella no lo intentas y das un paso como cualquiera. No te dice si lo consigues: si " + \
      "fallas, te ven pasar. Un paso sigiloso no rompe tu escondite. Escondido, abrir o " + \
      "cerrar una puerta tiene la misma probabilidad de hacerse en silencio y sin " + \
      "delatarte. No sirve con mucha luz encima, ni en mitad de una pelea.\n"
#define _LANG_SKILL_SNEAK_WHICH_WAY "¿Hacia dónde quieres sigilar?\n"
#define _LANG_SKILL_SNEAK_NOT_THERE "No ves cómo sigilar en esa dirección.\n"
#define _LANG_SKILL_SNEAK_BRIGHT "Es inútil intentar sigilar con tanta luz encima: " + \
      "pareces una antorcha andante.\n"
#define _LANG_SKILL_SNEAK_NO_SURFACE "El suelo de aquí no te deja pasar desapercibido.\n"
#define _LANG_SKILL_SNEAK_STUCK "No consigues andar sigilosamente por ahí.\n"
#define _LANG_SKILL_SNEAK_GO "Andas sigilosamente hacia " + dir + ".\n"
