
// finger

#define _LANG_FINGER_DOMAIN_NOT_FOUND "Este dominio tiene algún problema.\n"
#define _LANG_FINGER_DOMAIN_HEADER "Dominio de %^BOLD%^" + name + "%^RESET%^.\n"
#define _LANG_FINGER_DOMAIN_MANAGER "El coordinador de este dominio es " + \
              capitalize(master->query_dom_lord())+".\n"
#define _LANG_FINGER_DOMAIN_NO_MANAGER "Este dominio no tiene coordinador asignado.\n"
#define _LANG_FINGER_DOMAIN_ONLY_CODER "El único programador del dominio es "
#define _LANG_FINGER_DOMAIN_CODERS "Los programadores actuales del dominio son: "
#define _LANG_FINGER_DOMAIN_NO_INFO "El dominio no tiene información disponible.\n"

#define _LANG_FINGER_NAME "Nombre: "
#define _LANG_FINGER_REAL_NAME "Nombre real: "
#define _LANG_FINGER_BIRTHDAY "Cumpleaños: "
#define _LANG_FINGER_EMAIL "E-mail: "
#define _LANG_FINGER_PLACE "Ciudad: "
#define _LANG_FINGER_RACE_GENDER "   %^GREEN%^Es un"+(gender==2?"a ":" ") + \
              social_object_list[0]->query_race_gender_string(this_object(), 1)+"%^RESET%^. "
#define _LANG_FINGER_GUILD "   Es miembro del gremio " + \
              social_object_list[1]->short()+".\n"
#define _LANG_FINGER_WRONG_GUILD "   Es miembro de un gremio que no existe.\n"
#define _LANG_FINGER_CODER "   %^GREEN%^Es un"+(gender==2?"a":"")+ \
             " programador"+(gender==2?"a":"")+" en "+ \
             mud_name()+"%^RESET%^.\n"
#define _LANG_FINGER_MANAGER "Es " + (gender==2?"la":"el") + " coordinador" + \
             (gender==2?"a":"")+(sizeof(domains)==1?" del dominio ":" de los dominios ") + \
             query_multiple_short(domains, 0) + ".\n"
#define _LANG_FINGER_MANAGER_ONLY "   No es miembro de ningún otro dominio.\n"
#define _LANG_FINGER_NO_MEMBER "   No es miembro de ningún dominio.\n"
#define _LANG_FINGER_MEMBER_OF "   Es miembro de"+(sizeof(bing)==1?"l dominio":" los dominios")+ \
             " de " + (string)implode(bing, ", ") +".\n"
#define _LANG_FINGER_FIRST_ON "   Se conectó por primera vez el "
#define _LANG_FINGER_TIME_ON "   %^GREEN%^Tiene " + \
            seconds_to_time_string(time_on) + \
            " de antigüedad%^RESET%^.\n"
#define _LANG_FINGER_CONNECTED_SINCE "   Conectad"+(gender==2?"a":"o")+" desde el " + \
            ctime(last_log_on, 1)+".\n"
#define _LANG_FINGER_LAST_ON "   Se conectó por última vez hace:\n      " + \
            seconds_to_time_string(tmp_time) + ".\n"
#define _LANG_FINGER_IDLE_TIME "   %^GREEN%^Inactiv"+(gender==2?"a":"o")+" desde hace "+ \
            seconds_to_time_string(ob->user()->query_idle()) + \
            "%^RESET%^.\n"
#define _LANG_FINGER_LAST_CONNECTION_FROM "   Última conexión desde: " + last_on_from + "\n"
#define _LANG_FINGER_LAST_POSITION "   Última posición:\n     " + last_pos + "\n"
#define _LANG_FINGER_MAIL_NO_MAIL "   No tiene correo.\n"
#define _LANG_FINGER_MAIL_NUM_MAILS "   Tiene "+mail_stat["total"]+" mud-mail"+ \
            ((mail_stat["total"]==1)?"":"s")
#define _LANG_FINGER_MAIL_UNREAD_MAILS ", "+mail_stat["unread"]+" sin leer.\n"
#define _LANG_FINGER_PLAN "   Plan: "

// retire

#define _LANG_TRY_RETIRE_CODER "Eres un programador, no puedes retirarte de este modo. Pregúntale a un administrador.\n"
#define _LANG_TRY_RETIRE_DELETE_WARNING "%^BOLD%^Esto borrará el personaje con el que estás jugando.%^RESET%^\n"
#define _LANG_TRY_RETIRE_BE_CAREFUL "Asegúrate de saber lo que estás haciendo.\nNo introduzcas la contraseña si te arrepientes.\n\n"
#define _LANG_TRY_RETIRE_INPUT_PASSWORD "Introduce la contraseña de tu cuenta: "
#define _LANG_RETIRE_NO_PASSWORD "Sin contraseña no puedes retirarte.\n"
#define _LANG_RETIRE_WRONG_PASSWORD "Contraseña equivocada, no puedes retirarte.\n"
#define _LANG_RETIRE_OK "Ok, has borrado tu ficha, " + name + ".\n"
#define _LANG_RETIRE_GUEST_MSG "Ahora eres un invitado.\nSal del juego para eliminar definitivamente el personaje.\n"
