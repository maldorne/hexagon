
// corpse.c

#define _LANG_CORPSE_SOMEBODY "alguien"
#define _LANG_CORPSE_NAME "cuerpo"
#define _LANG_CORPSE_ALIASES ({ "cuerpo", "cadáver", "cadaver", "restos" })
#define _LANG_CORPSE_PLURALS ({ "cuerpos", "cadáveres", "cadaveres", "restos" })
#define _LANG_CORPSE_DESC "Es un cadáver.\n"

#define _LANG_CORPSE_OF "cuerpo de "
#define _LANG_CORPSES_OF "cuerpos de "
#define _LANG_CORPSE_DESC_OF "Este es el cuerpo muerto de "+capitalize(n)+".\n"
#define _LANG_CORPSE_RAISE_MSG "Escuchas el aullido de un espíritu torturado.\n" + \
    "De repente, un " + cow->query_name() + " se levanta de entre los muertos.\n"
#define _LANG_CORPSE_UNKNOWN_BEIGN "ser desconocido"
#define _LANG_CORPSE_REMAINS "restos de "+query_name()
#define _LANG_CORPSE_ROTTEN_REMAINS "restos putrefactos de un "+query_name()
#define _LANG_CORPSE_REMAINS_DESC "Este es el cuerpo muerto de un "+race_name+".\n"

// base-undead.c

#define _LANG_UNDEAD_FRIEND_MSG "El " + this_object()->query_cap_name() + \
    " suspira y vuelve al mundo de los muertos.\n"

#define _LANG_SKELETON_NAME "esqueleto"
#define _LANG_SKELETON_DESC "Nada más que un esqueleto animado, este ser ha vuelto a " + \
       "la vida para traer la desgracia a los vivos.\n"

#define _LANG_ZOMBIE_NAME "zombi"
#define _LANG_SKELETON_DESC "Nada más que un cadáver animado, este ser ha vuelto a " + \
       "la vida para traer la desgracia a los vivos.\n"

#define _LANG_SPECTRE_NAME "espectro"
#define _LANG_SPECTRE_DESC "Nada más que un trozo de aire ruidoso con mal genio, este ser ha vuelto a " + \
        "la vida para traer la desgracia a los vivos.\n"

#define _LANG_WRAITH_NAME "wraith"
#define _LANG_WRAITH_DESC "Una sombra difusa se acerca a ti, malos recuerdos " + \
        "se agitan en tu mente mientras este ser intenta arrastrarte hacia el mundo de los muertos.\n"

#define _LANG_LICH_NAME "lich"
#define _LANG_LICH_DESC "No puedes ver nada más que una oscura capa con unos brillantes ojos  " + \
       " que te siguen mientras te mueves.\n"
