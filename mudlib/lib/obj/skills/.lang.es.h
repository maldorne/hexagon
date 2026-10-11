
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
#define _LANG_HIDE_REVEALED_ROOM_POST " moverse a tu alrededor... ¡estaba escondiéndose!%^RESET%^\n"
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
      " empieza a trepar por " + target->query_short() + ".\n"
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
#define _LANG_SKILL_SWIM_ROUND1 "Coges aire y das las primeras brazadas.\n"

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
