
// effects / base_effect — cadenas visibles para el jugador (español)

// etiquetas de rol de combate, indexadas por las constantes de combat.h
#define _LANG_COMBAT_ROLE_STRINGS ({ "neutral", "ofensiva", "defensiva", "sanadora" })

// --- combat_fixes: mensajes de protección ---
#define _LANG_EFFECT_PROTECT_YOU_PRE "Proteges a "
#define _LANG_EFFECT_PROTECT_YOU_POST " del ataque.\n"
#define _LANG_EFFECT_PROTECT_VICTIM " valientemente te protege.\n"
#define _LANG_EFFECT_PROTECT_DOER " salta delante de tu víctima, interceptando tu hechizo y recibiéndolo en su cuerpo!\n"
#define _LANG_EFFECT_PROTECT_ROOM_1 " valientemente protege a "
#define _LANG_EFFECT_PROTECT_ROOM_2 " del hechizo de "
#define _LANG_EFFECT_PROTECT_ROOM_3 ".\n"

// --- environment_fixes ---
#define _LANG_EFFECT_ENV_NO_EFFECT "¡Tu hechizo no es efectivo!\n"
#define _LANG_EFFECT_ENV_WEAKER "Tu hechizo parece no funcionar tan bien como es normal en él.\n"
#define _LANG_EFFECT_ENV_STRONGER "Tu hechizo parece funcionar mejor de lo que es habitual en él.\n"

// --- guild_fixes ---
#define _LANG_EFFECT_PUNISH_ALIGN "Notas como tu Dios mira en tu corazón y no encuentra lo que debiera.\n" + \
    "Sientes como pierdes algo por esta humillación.\n"

// --- base_effect: mensajes de bloqueo ---
#define _LANG_EFFECT_NOT_APPROVED "Este efecto no es para mortales ya que aún no está aprobado.\n"
#define _LANG_EFFECT_OWN_AURA_BLOCKS "Tu propia aura protectora te impide hacer eso.\n"
#define _LANG_EFFECT_CANT_WHILE_HIDDEN "No puedes hacer eso estando escondido.\n"
#define _LANG_EFFECT_PROTECTED_TARGET_PRE "Algún tipo de protección te impide hacer eso sobre "
#define _LANG_EFFECT_PROTECTED_TARGET_POST ".\n"
#define _LANG_EFFECT_CANT_ON_PRE "No puedes hacer eso sobre "
#define _LANG_EFFECT_CANT_ON_POST ".\n"
#define _LANG_EFFECT_ROLE_BLOCKS "Tu rol de combate actual no te permite realizar eso.\n"

// --- base_effect: mensajes de comienzo ---
#define _LANG_EFFECT_START_SPELL_PRE "Comienzas a formular el hechizo '"
#define _LANG_EFFECT_START_SPELL_POST "'.\n"
#define _LANG_EFFECT_START_SPELL_ROOM " comienza a formular un hechizo.\n"
#define _LANG_EFFECT_START_SKILL_PRE " Comienzas a ejecutar la habilidad '"
#define _LANG_EFFECT_START_SKILL_POST "'.\n"

// --- base_effect: help() ---
#define _LANG_EFFECT_CATEGORY_NONE "ninguna"
#define _LANG_EFFECT_HELP_NAME "Nombre: "
#define _LANG_EFFECT_HELP_LEVEL "\nNivel: "
#define _LANG_EFFECT_HELP_CATEGORY "\tCategoría: "
#define _LANG_EFFECT_HELP_RANGE_NONE "\nDistancia: no"
#define _LANG_EFFECT_HELP_RANGE "\nDistancia: "
#define _LANG_EFFECT_HELP_COST "\tCoste: "
#define _LANG_EFFECT_HELP_ROLE_NEEDED "\nActitud de combate requerida: "
#define _LANG_EFFECT_HELP_DESC "\nDescripción:\n"
