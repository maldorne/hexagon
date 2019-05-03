
// login.c

#define _LANG_LPMUD_VERSION "Versión de LPmud: "
#define _LANG_MUDLIB_VERSION "Versión de Mudlib: "
#define _LANG_PATCH_VERSON "Último parche: "
#define _LANG_LANGUAGE_VERSION "version en castellano"

#define _LANG_WHO_COMMAND "who"
#define _LANG_CREATE_COMMAND "crear"
#define _LANG_FINGER_COMMAND "finger"

#define _LANG_WORLD_STILL_BEING_CREATED "El mundo aún se está formando, inténtalo de nuevo en unos minutos.\n"
#define _LANG_TOO_MANY_PLAYERS "Hay demasiados jugadores conectados, inténtalo dentro de unos minutos.\n"
#define _LANG_ENTER_AN_OPTION "Introduce la opción elegida: "
#define _LANG_TIMED_OUT "Se terminó el tiempo.\n"
#define _LANG_COME_AGAIN_SOON "\n¡Vuelve dentro de poco!\n"
#define _LANG_OPTION_STRING_TOO_SHORT "La opción introducida es demasiado corta, el mínimo son " + MIN_LEN + " caracteres.\n"
#define _LANG_INVALID_CHARACTER "Caracter inválido '" + str[tmp..tmp] + "' en (" + str + ").\n"
#define _LANG_PLAYER_ACCESS_FORBIDDEN "El acceso a jugadores no está permitido en estos momentos.\n"
#define _LANG_ANSWER_YES_NO "Debes responder sí o no (s/n): "
#define _LANG_YES_OPTIONS_ARRAY ({ 's', 'S' })
#define _LANG_NO_OPTIONS_ARRAY ({ 'n', 'N' })
#define _LANG_TRY_AGAIN "De acuerdo, inténtalo de nuevo.\n"

#define _LANG_ENTER_EMAIL "Introduce tu dirección de email: "
#define _LANG_ENTER_VALID_EMAIL "\nIntroduce una dirección de email válida: "
#define _LANG_USED_EMAIL "Lo sentimos, pero esa dirección ya está utilizada.\n" + \
      "Por favor, introduce otra dirección de email: "
#define _LANG_ACCOUNT_RECOMMEND "Si ya tienes otra cuenta en " + mud_name() + ", recomendamos " + \
      "encarecidamente que crees tus " + \
      "distintos personajes dentro de la misma cuenta, ya que esto tiene muchas ventajas a la " + \
      "hora de compartir objetos, recursos, logros, etc.\n"
#define _LANG_ACCOUNT_NAME_CHOSEN "Has escogido como nombre de cuenta: '" + user_name + "', ¿estás seguro? (s/n): "
#define _LANG_DEFINE_ACCOUNT_PASSWORD "Introduce la contraseña que estará asociada a tu cuenta: "
#define _LANG_DEFINE_ACCOUNT_PASSWORD_REPEAT "Repite la contraseña para confirmar que no hay ningún error: "
#define _LANG_ACCOUNT_REPEAT "Volvamos a empezar."
#define _LANG_ACCOUNT_PASSWORD_REMOVED "De acuerdo, contraseña borrada."
#define _LANG_ACCOUNT_DIFFERENT_PASSWORDS "Las contraseñas son distintas."
#define _LANG_NEW_ACCOUNT_CREATED "\n   %^BOLD%^Nueva cuenta creada con el nombre '" + user_name + "'.%^RESET%^\n\n"

#define _LANG_NONEXISTANT_ACCOUNT "No existe una cuenta con ese nombre.\n"
#define _LANG_NONEXISTANT_ACCOUNT_OR_CHARACTER "No existe ninguna cuenta o personaje con ese nombre.\n"
#define _LANG_TYPE_ACCOUNT_PASSWORD "Introduce la contraseña asociada a tu cuenta: "
#define _LANG_TYPE_CHARACTER_PASSWORD "Introduce la contraseña de tu personaje: "
#define _LANG_WRONG_PASSWORD "Password incorrecto.\n"
#define _LANG_TOO_MANY_RETRIES "Demasiados reintentos...\nNo estarás haciendo nada malo, ¿verdad?\n"
#define _LANG_AVAILABLE_CHARACTERS_IN_ACCOUNT "Tienes disponibles en esta cuenta los siguientes personajes:\n"
#define _LANG_CHOOSE_ACCOUNT_CHARACTER "\nEscribe su nombre (o número) para empezar a jugar.\n" + \
      "   ('nuevo' para crear un nuevo personaje)\n> \n"
#define _LANG_CHOOSE_NEW "nuevo"
#define _LANG_TYPE_THE_NEW_CHARACTER_NAME "Introduce el nombre de tu nuevo personaje: "
#define _LANG_UNKNOWN_CHARACTER "No conozco a ese personaje, vuelve a introducir el nombre o número: "
#define _LANG_CHARACTER_RESTORE_ERROR "Error, no se ha podido recuperar el personaje.\n"
#define _LANG_ACCOUNT_RESTORE_ERROR "Error, no se ha podido recuperar tu cuenta.\n"
#define _LANG_ALREADY_PLAYING "Ya estás jugando, ¿tiro la otra copia del personaje? (s/n): "
#define _LANG_CODERS_FORBIDDEN "Los programadores no tienen permitido el acceso a este puerto.\n"

#define _LANG_WAIT_LOADING "Por favor espera... cargando el personaje.\n"
#define _LANG_CONNECTED_WELCOME "¡Te has conectado! Bienvenido a "+mud_name()+".\n\n"
#define _LANG_HAS_RECONNECTED _user->query_cap_name() + " se ha reconectado.\n"
