// job entrances. `me` is the player joining or leaving, `job` the job object

#define _LANG_JOB_ENTRANCE_CMDS_JOIN ({ "alistarse", "alistar", "alistarme" })
#define _LANG_JOB_ENTRANCE_CMDS_LEAVE ({ "renunciar" })
#define _LANG_JOB_ENTRANCE_YES ({ "s", "si", "sí" })
#define _LANG_JOB_ENTRANCE_CMDS_RULES ({ "normas" })

#define _LANG_JOB_ENTRANCE_SIGN_LONG "Un cartel cuelga de la pared, junto a la entrada.\n"
#define _LANG_JOB_ENTRANCE_SIGN_NO_JOB "Aquí no se enseña ningún oficio ahora mismo."
#define _LANG_JOB_ENTRANCE_SIGN_TITLE "Oficio: "
#define _LANG_JOB_ENTRANCE_SIGN_COMMANDS \
        "  alistarse    aprendes este oficio\n" + \
        "  renunciar    abandonas el oficio que tienes\n" + \
        "  normas       quién puede aprenderlo y las normas del oficio\n\n" + \
        "Solo se puede tener un oficio a la vez. Quien vuelve a un oficio que " + \
        "dejó recupera el nivel que tenía, pero no la experiencia que le faltaba " + \
        "para el siguiente."

#define _LANG_JOB_ENTRANCE_NO_JOB "Aquí no se enseña ningún oficio ahora mismo.\n"
#define _LANG_JOB_ENTRANCE_ALREADY_MEMBER "Ya tienes este oficio.\n"
#define _LANG_JOB_ENTRANCE_OTHER_JOB "Ya tienes un oficio: tendrás que renunciar " + \
        "a él antes de aprender otro.\n"
#define _LANG_JOB_ENTRANCE_NOT_LEGAL "No reúnes las condiciones para aprender " + \
        "este oficio.\n"
#define _LANG_JOB_ENTRANCE_JOINED "Te alistas: desde hoy tu oficio es " + \
        lower_case((string)job->query_short()) + ". Escribe 'normas' para " + \
        "leer las normas del oficio.\n"
#define _LANG_JOB_ENTRANCE_JOINED_OTHERS me->query_cap_name() + " se alista " + \
        "para aprender el oficio.\n"
#define _LANG_JOB_ENTRANCE_NO_JOB_TO_LEAVE "No tienes ningún oficio.\n"
#define _LANG_JOB_ENTRANCE_CANNOT_LEAVE "Por alguna razón no te está permitido " + \
        "abandonar tu oficio.\n"
#define _LANG_JOB_ENTRANCE_LEAVE_CONFIRM "¿Seguro que quieres renunciar a tu " + \
        "oficio? Conservarás tu nivel, pero perderás la experiencia de oficio " + \
        "que llevas acumulada. (s/n) "
#define _LANG_JOB_ENTRANCE_LEAVE_CANCELLED "De acuerdo, sigues con tu oficio.\n"
#define _LANG_JOB_ENTRANCE_LEFT "Renuncias a tu oficio: a partir de ahora no " + \
        "tienes ninguno.\n"
