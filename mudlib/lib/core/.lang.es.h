
// login.c

#define _LANG_LPMUD_VERSION "Versi�n de LPmud: "
#define _LANG_MUDLIB_VERSION "Versi�n de Mudlib: "
#define _LANG_PATCH_VERSON "�ltimo parche: "
#define _LANG_LANGUAGE_VERSION "espa�ol"

#define _LANG_WHO_COMMAND "who"
#define _LANG_CREATE_COMMAND "crear"
#define _LANG_FINGER_COMMAND "finger"

#define _LANG_LOGIN_CMDS_OPTIONS "Opciones: "
#define _LANG_OPTIONS_ACCOUNT_LOGIN "%^BOLD%^GREEN%^<email>%^RESET%^ para hacer login con tu cuenta."
#define _LANG_OPTIONS_CREATE_ACCOUNT "%^BOLD%^crear%^RESET%^ para crear una nueva cuenta."
#define _LANG_OPTIONS_GUEST_LOGIN "%^BOLD%^invitado%^RESET%^ para probar el juego temporalmente."
#define _LANG_OPTIONS_CHARACTER_LOGIN "%^BOLD%^GREEN%^<nombre de personaje>%^RESET%^ para seguir jugando."
#define _LANG_OPTIONS_CREATE_CHARACTER "%^BOLD%^crear%^RESET%^ para crear una nuevo personaje jugador."
#define _LANG_OPTIONS_LIST_CHARACTERS "%^BOLD%^characters%^RESET%^ para ver todos tus personajes disponibles."
#define _LANG_OPTIONS_CMD_FINGER "%^BOLD%^finger <nombre>%^RESET%^ para obtener datos sobre otro jugador."
#define _LANG_OPTIONS_CMD_WHO "%^BOLD%^who%^RESET%^ para ver la lista de jugadores conectados."

#define _LANG_WORLD_STILL_BEING_CREATED "El mundo a�n se est� formando, int�ntalo de nuevo en unos minutos.\n"
#define _LANG_TOO_MANY_PLAYERS "Hay demasiados jugadores conectados, int�ntalo dentro de unos minutos.\n"
#define _LANG_ENTER_AN_OPTION "Introduce la opci�n elegida: "
#define _LANG_TIMED_OUT "Se termin� el tiempo.\n"
#define _LANG_COME_AGAIN_SOON "\n�Vuelve dentro de poco!\n"
#define _LANG_OPTION_STRING_TOO_SHORT "La opci�n introducida es demasiado corta, el m�nimo son " + MIN_LEN + " caracteres.\n"
#define _LANG_INVALID_CHARACTER_OR_EMAIL "Nombre de usuario o email inv�lido" + (tmp >= 0) ? " (car�cter err�neo '" + str[tmp..tmp] + "' en '" + str + "').\n" : ".\n"
#define _LANG_PLAYER_ACCESS_FORBIDDEN "El acceso a jugadores no est� permitido en estos momentos.\n"
#define _LANG_ANSWER_YES_NO "Debes responder s� o no (s/n): "
#define _LANG_YES_OPTIONS_ARRAY ({ 's', 'S' })
#define _LANG_NO_OPTIONS_ARRAY ({ 'n', 'N' })
#define _LANG_TRY_AGAIN "De acuerdo, int�ntalo de nuevo.\n"

#define _LANG_ENTER_EMAIL "Introduce tu direcci�n de email: "
#define _LANG_ENTER_VALID_EMAIL "\nIntroduce una direcci�n de email v�lida: "
#define _LANG_USED_EMAIL "Lo sentimos, pero esa direcci�n ya est� utilizada.\n" + \
      "Por favor, introduce otra direcci�n de email: "
#define _LANG_ACCOUNT_RECOMMEND "Si ya tienes otra cuenta en " + mud_name() + ", recomendamos " + \
      "encarecidamente que crees tus " + \
      "distintos personajes dentro de la misma cuenta, ya que esto tiene muchas ventajas a la " + \
      "hora de compartir objetos, recursos, logros, etc.\n"
#define _LANG_ACCOUNT_NAME_CHOSEN "Has escogido como nombre de cuenta: '" + user_name + "', �est�s seguro? (s/n): "
#define _LANG_DEFINE_ACCOUNT_PASSWORD "Introduce la contrase�a que estar� asociada a tu cuenta: "
#define _LANG_DEFINE_ACCOUNT_PASSWORD_REPEAT "Repite la contrase�a para confirmar que no hay ning�n error: "
#define _LANG_ACCOUNT_REPEAT "Volvamos a empezar."
#define _LANG_ACCOUNT_PASSWORD_REMOVED "De acuerdo, contrase�a borrada."
#define _LANG_ACCOUNT_DIFFERENT_PASSWORDS "Las contrase�as son distintas."
#define _LANG_NEW_ACCOUNT_CREATED "\n   %^BOLD%^Nueva cuenta creada con el nombre '" + user_name + "'.%^RESET%^\n\n"

#define _LANG_NONEXISTANT_ACCOUNT "No existe una cuenta con ese nombre.\n"
#define _LANG_NONEXISTANT_ACCOUNT_OR_CHARACTER "No existe ninguna cuenta o personaje con ese nombre.\n"
#define _LANG_TYPE_ACCOUNT_PASSWORD "Introduce la contrase�a asociada a tu cuenta: "
#define _LANG_TYPE_CHARACTER_PASSWORD "Introduce la contrase�a de tu personaje: "
#define _LANG_WRONG_PASSWORD "Password incorrecto.\n"
#define _LANG_TOO_MANY_RETRIES "Demasiados reintentos...\nNo estar�s haciendo nada malo, �verdad?\n"
#define _LANG_CHOOSE_ACCOUNT_CHARACTER "\nEscribe su nombre (o n�mero) para empezar a jugar.\n" + \
      "   ('nuevo' para crear un nuevo personaje)\n> \n"
#define _LANG_CHOOSE_NEW "nuevo"
#define _LANG_TYPE_THE_NEW_CHARACTER_NAME "Introduce el nombre de tu nuevo personaje: "
#define _LANG_UNKNOWN_CHARACTER "No conozco a ese personaje, vuelve a introducir el nombre o n�mero: "
#define _LANG_CHARACTER_RESTORE_ERROR "Error, no se ha podido recuperar el personaje.\n"
#define _LANG_ACCOUNT_RESTORE_ERROR "Error, no se ha podido recuperar tu cuenta.\n"
#define _LANG_ALREADY_PLAYING "Ya est�s jugando, �tiro la otra copia del personaje? (s/n): "
#define _LANG_CODERS_FORBIDDEN "Los programadores no tienen permitido el acceso a este puerto.\n"

#define _LANG_INVALID_USER_NAME "Caracter inv�lido '"+str[tmp..tmp]+"' en ("+str+").\n\n" 
#define _LANG_CHARACTER_NAME_TOO_LONG "El nombre es demasiado largo, el m�ximo son "+MAX_LEN+" caracteres.\n"
#define _LANG_CHARACTER_NAME_TOO_SHORT "El nombre es demasiado corto, el m�nimo son "+MIN_LEN+" caracteres.\n"
#define _LANG_USED_CHARACTER_NAME "Lo sentimos, pero ese nombre ya est� utilizado.\n" + \
      "Por favor, introduce otro nombre para tu nuevo personaje: "
#define _LANG_CHARACTER_NAME_CHOSEN "Has escogido como nombre para tu personaje: '" + name + "', �est�s seguro? (s/n): "
#define _LANG_CHOOSE_CHARACTER_GENDER "�Tu personaje es hombre o mujer? (h/m): "
#define _LANG_GENDER_MAN_OPTIONS_ARRAY ({ 'h', 'H' })
#define _LANG_GENDER_WOMAN_OPTIONS_ARRAY ({ 'm', 'M' })
#define _LANG_ANSWER_CHOOSE_AN_OPTION "Debes responder una de las opciones."


#define _LANG_WAIT_LOADING "Por favor espera... cargando el personaje.\n"
#define _LANG_CONNECTED_WELCOME "%^BOLD%^�Te has conectado! Bienvenido a "+mud_name()+".%^RESET%^\n"
#define _LANG_HAS_RECONNECTED _user->query_cap_name() + " se ha reconectado.\n"


// game.c

#define _LANG_GAME_DEFAULT_FINGER_INFO "Uno de los juegos de " + mud_name() + ".\n"
