
// queue

#define _LANG_QUEUE_DIDNT_WORK "El intento de hacer '%^RED%^" + \
                            curr_act + "%^RESET%^' no funcionó.\n"

// handle

#define _LANG_HANDLE_WAIT_LOADING "Tienes que esperar a que todo tu equipo esté cargado.\n"
#define _LANG_HANDLE_DEAD "Tu estado etéreo tiene poco efecto en el mundo material.\n "
#define _LANG_HANDLE_STILL "Aún estas ocupado con la carga.\n"
#define _LANG_HANDLE_FAIL "No puedes "+query_verb()+" "+query_multiple_short(fail, 1)+".\n"
                            
#define _LANG_HANDLE_GET_VERBS ({ "coger" })
#define _LANG_HANDLE_GIVE_VERBS ({ "poner", "dar" })
#define _LANG_HANDLE_DROP_VERBS ({ "dejar" })

#define _LANG_HANDLE_GET_SYNTAX "Sintaxis: "+query_verb()+" <objeto> [de <objeto>]\n"
#define _LANG_HANDLE_GET_PREP "de"
#define _LANG_HANDLE_GET_CANNOT "No encuentras ningún "+s2+" por aquí.\n"
#define _LANG_HANDLE_GET_NOTHING "Nada que "+query_verb()+".\n"
#define _LANG_HANDLE_GET_MESSAGES ({ "Coges", "Coges con un poco de dificultad", \
          "Te las arreglas como puedes para coger", \
          "Te resulta muy difícil coger", \
          "Usas toda tu fuerza y a duras penas logras coger" })
#define _LANG_HANDLE_GET_MESSAGES_OTHERS ({ "coge", "coge con un poco de dificultad", \
            "se las arregla como puede para coger", \
            "tiene muchas dificultades para coger", \
            "usa toda su fuerza y a duras penas logra coger" })

#define _LANG_HANDLE_GIVE_SYNTAX "Sintaxis: "+query_verb()+" <objeto> <a/en> <objeto>\n"
#define _LANG_HANDLE_PUT_VERB "poner"
#define _LANG_HANDLE_PUT_PREP "en"
#define _LANG_HANDLE_GIVE_PREP "a"
#define _LANG_HANDLE_GIVE_CANNOT "No se puede encontrar "+person+".\n"