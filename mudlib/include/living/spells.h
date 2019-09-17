
// neverbot 10/2001

#define RANGE_HANDLER "/obj/handlers/range"

#define SP_OBJECT 1 
#define SP_FUNC 2
#define SP_PARAM 3
#define S_OBJECT 0
#define S_FUNC 1
#define SP_NO_RNDS 0
// Sacados de /std/living/spells.c y de /std/spells/base.c, neverbot 7/01
#define SP_DIV 5
#define SP_GP_COST 3

/* 
   SP_OBJECT
   SP_FUNC
   SP_PARAM
   S_OBJECT
   S_FUNC
   SP_NO_ROUNDS: int que representan la posicion de ciertos datos dentro de arrays.
   
   SP_DIV: fraccion de vida que si perdemos causa que nos desconcentremos al formular.
           (si perdemos 1/SP_DIV de nuestros hps).
*/

#define MAX(a,b) ( a>b?a:b )
#define MIN(a,b) ( a<b?a:b )

#define SPELL_ROOT   "/std/spells/"
#define CLERIC_ROOT  "/std/spells/cleric/"
#define MAGE_ROOT    "/std/spells/wizard/"
#define BARD_ROOT    "/std/spells/bard/"
#define DRUID_ROOT   "/std/spells/druid/"

#define BASE_SPELL   "/std/spells/base.c"
#define BASES        "/std/spells/bases/"
#define AREA_SPELL   "/std/spells/bases/area.c"
#define FLECHA_SPELL "/std/spells/bases/flecha.c"
#define ITEMS        "/std/spells/items/"
#define NPCS         "/std/spells/npcs/"

#define SHADOWS      SPELL_ROOT+"shadows/"
#define OBJETOS      SPELL_ROOT+"items/"
#define ESCUELAS     SPELL_ROOT+"escuelas/"
#define ESFERAS      SPELL_ROOT+"esferas/"

// Escuelas de la magia (magos)
#define ABJURACION       ESCUELAS+"abjuracion/"
#define ADIVINACION      ESCUELAS+"adivinacion/"
#define ALTERACION       ESCUELAS+"alteracion/"
#define CONJURACION      ESCUELAS+"conjuracion/"
#define ENCANTAMIENTO    ESCUELAS+"encantamiento/"
#define ILUSION          ESCUELAS+"ilusion/"
#define INVOCACION       ESCUELAS+"invocacion/"
#define NIGROMANCIA      ESCUELAS+"nigromancia/"
#define ELEMENTAL_AGUA   ESCUELAS+"elemental agua/"
#define ELEMENTAL_AIRE   ESCUELAS+"elemental aire/"
#define ELEMENTAL_FUEGO  ESCUELAS+"elemental fuego/"
#define ELEMENTAL_TIERRA ESCUELAS+"elemental tierra/"


// Esferas de la magia (clerigos)
#define ANIMAL        ESFERAS+"animal/"
#define ASTRAL        ESFERAS+"astral/"
#define CAOS          ESFERAS+"caos/"
#define CLIMA         ESFERAS+"clima/"
#define COMBATE       ESFERAS+"combate/"
#define CREACION      ESFERAS+"creacion/"
#define CURADORA      ESFERAS+"curadora/"
#define DANYADORA     ESFERAS+"danyadora/"
#define DEFENSA       ESFERAS+"defensa/"
#define ELEMENTAL     ESFERAS+"elemental/"
#define GUARDIANA     ESFERAS+"guardiana/"
#define GUERRA        ESFERAS+"guerra/"
#define LEY           ESFERAS+"ley/"
#define NECROMANTICA  ESFERAS+"necromantica/"
#define PENSAMIENTO   ESFERAS+"pensamiento/"
#define PROTECCION    ESFERAS+"proteccion/"
#define SOL           ESFERAS+"sol/"
#define TIEMPO        ESFERAS+"tiempo/"
#define TOTAL         ESFERAS+"total/"
#define VEGETAL       ESFERAS+"vegetal/"


// Mensajes iguales para todos los hechizos??
#define MSG_NO_TARGET  "El hechizo falla, no parece haber nadie aquí con ese nombre.\n"
#define MSG_AFECTADO   target->query_cap_name()+" ya está afectado por ese hechizo.\n"
#define MSG_SUIC_R     caster->query_cap_name()+" deja de formular un hechizo.\n"
#define MSG_SUIC_C     "¿¡No querrás suicidarte!?\n"
#define MSG_CAN_R      caster->query_cap_name()+" pronuncia el cántico '"+CANCION+"'.\n"
#define MSG_CAN_C      "Pronuncias el cántico '"+CANCION+"'.\n"
#define MSG_NO_PUEDO   "No puedes formular este hechizo en este momento.\n"
#define MSG_NO_PUEDO_R caster->query_cap_name()+" deja de formular un hechizo.\n"

// Mensajes de error para no mezclar auras incompatibles
// asi evitamos colocar distintas shadows que solapen la misma funcion:

#define MSG_AURA_SPELL_DAMAGE_C   "No puedes terminar el hechizo, otro de las mismas características te lo impide.\n"
#define MSG_AURA_SPELL_DAMAGE_R   caster->query_cap_name()+" deja de formular un hechizo.\n"
#define MSG_AURA_ADJUST_HP_C      "No puedes terminar el hechizo, otro de las mismas características te lo impide.\n"
#define MSG_AURA_ADJUST_HP_R      caster->query_cap_name()+" deja de formular un hechizo.\n"
#define MSG_AURA_WEAPON_DAMAGE_C  "No puedes terminar el hechizo, otro de las mismas características te lo impide.\n"
#define MSG_AURA_WEAPON_DAMAGE_R  caster->query_cap_name()+" deja de formular un hechizo.\n"

// Propiedades para usar en varios hechizos
#define PROPERTY_CEGADO                "BLIND"
#define PROPERTY_INVUL                 "invulnerable_hechizos"
#define INAUDIBLE                      "inaudible"
#define VISION_REAL                    "vision_real"
#define PROPERTY_ARMADURA_ESPIRITUAL   "spirit_armour"
#define PROPERTY_LEER_MAGIA            "read_magic"
#define PROPERTY_STILLCASTING          "stillcasting"
