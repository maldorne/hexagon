
// Definiciones para efectos

#define BASE_EFFECT "/std/effects/base_effect.c"
#define EFFECTS_TABLE "/table/effects_table.c"

#define RANGE_HANDLER  "/obj/handlers/range.c"

#define EFFECT_IS_FEAT 0
#define EFFECT_IS_SPELL 1

#define EFFECT_TYPE_SPELL "spell"

#define TARGET_TYPE_NONE "none"
#define TARGET_TYPE_ONE "one"
#define TARGET_TYPE_MANY "many"
#define TARGET_TYPE_ALL "all"
#define TARGET_TYPE_ITEM "item"
#define TARGET_TYPE_PASS "pass"
#define TARGET_TYPE_SELF "self"
#define TARGET_TYPE_TOUCH "touch"

#define PROPERTY_IS_CASTING "casting"
#define PROP_EFECTO_INAUDIBLE "inaudible"
#define PROP_EFFECT_TESTER "TEST"
#define PROPERTY_BLINDED "BLIND"

#define SP_OBJECT 1
#define SP_FUNC 2
#define SP_PARAM 3
#define S_OBJECT 0
#define S_FUNC 1
#define SP_NO_RNDS 0
#define SP_DIV 5
#define SP_GP_COST 3

// Mensajes iguales para todos los efectos
#define MSG_NO_TARGET   "No parece haber nadie aquí con ese nombre.\n"
#define MSG_DISSAPPEAR  "Tu objetivo ha desaparecido.\n"
#define MSG_NO_GPS      "Estás demasiado cansado para hacer eso.\n"
#define MSG_NO_COMBAT   "No puedes hacer eso en mitad de un combate.\n"
#define MSG_ONLY_COMBAT "Eso sólo tiene sentido en mitad de un combate.\n"
#define MSG_AFECTADO    target->query_cap_name()+" ya está afectado por ese efecto.\n"
#define MSG_NO_SELF     "No puedes hacer eso sobre ti mismo.\n"
#define MSG_WRONG_ENV   "No puedes hacer eso en un lugar como este.\n"
#define MSG_NO_BLIND    "Estando cegado no puedes hacer eso.\n"
#define MSG_SUIC_C      "¿¡No querrás suicidarte!?\n"
#define MSG_SUIC_R      caster->query_cap_name()+" deja de hacer lo que estaba haciendo.\n"
#define MSG_NO_PUEDO    "No puedes hacer eso en este momento.\n"
#define MSG_NO_PUEDO_R  caster->query_cap_name()+" deja de hacer lo que estaba haciendo.\n"
#define MSG_NO_ITEMS    "No puedes hacer eso sobre objetos inanimados.\n"
#define MSG_ON_ITEMS    "Eso sólo puede hacerse sobre objetos inanimados.\n"
#define MSG_NO_CONTINUE "No puedes continuar con lo que estabas haciendo.\n"
#define MSG_NO_MOVE     "Al moverte pierdes la concentración.\n"
#define MSG_NO_RANGE    "Tu objetivo está demasiado lejos.\n"
#define MSG_NO_ON_MOUNT "No puedes hacer eso sobre alguien que vaya montado.\n"
#define MSG_NO_RIDING   "No puedes hacer eso mientras vayas montando.\n"
#define MSG_OCUPADO     "Estás ocupado haciendo otra cosa.\n"
#define MSG_ERROR       "Este efecto tiene un error, avisa rápidamente a algún programador.\n"
