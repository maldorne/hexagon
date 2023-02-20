
// communicate.c

#define _LANG_SAY_VERBS ({ "decir" })
#define _LANG_TELL_VERBS ({ "tell" })
#define _LANG_WHISPER_VERBS ({ "susurrar" })
#define _LANG_SPEAK_VERBS ({ "hablar" })
#define _LANG_SHOUT_VERBS ({ "gritar" })
#define _LANG_COMMNS_LANG_PREP "en"
#define _LANG_COMMS_SYNTAX "Sintaxis: " + query_verb() + " <algo>\n"
#define _LANG_COMMS_NO_CURRENT_LANG "Debes seleccionar qué idioma hablas.\n" + \
                                    "Usa el comando '"+_LANG_SPEAK_VERBS[0]+"'.\n"

#define _LANG_COMM_I_EXCLAIM "exclamas"
#define _LANG_COMM_I_ASK "preguntas"
#define _LANG_COMM_I_SAY "dices"
#define _LANG_COMM_THEY_EXCLAIM "exclama"
#define _LANG_COMM_THEY_ASK "pregunta"
#define _LANG_COMM_THEY_SAY "dice"
#define _LANG_COMMS_NO_ENV "En el vacío nadie puede decir u oir nada.\n"
#define _LANG_COMMS_NO_SPOKEN_LANG "no es un lenguaje hablado.\n"

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

// combat.c

#define _LANG_COMBAT_BEING_ATTACKED "%^BOLD%^Estás siendo atacado por %^RESET%^" + \
                        name + "%^BOLD%^.%^RESET%^\n\n"

// death.c

#define _LANG_DEATH_DEATH_BLOW_ROOM "\n" + killed_by->query_cap_name() + \
      " propina el golpe mortal a " + this_object()->query_cap_name() + ".\n\n"
#define _LANG_DEATH_DEATH_BLOW_ATT "Propinas el golpe mortal a " + this_object()->query_cap_name() + "."
#define _LANG_DEATH_DEATH_BLOW_DEF killed_by->query_cap_name() + " te propina el golpe mortal."
#define _LANG_DEATH_DEATH_BLOW_NO_KILLER this_object()->query_cap_name() + \
      " ha muerto horriblemente.\n"
#define _LANG_DEATH_CORPSE_DEF "Tu cuerpo pierde sus últimos alientos de vida"
#define _LANG_DEATH_CORPSE_ATT this_object()->query_cap_name() + " cae al suelo destrozad"+this_object()->query_vowel()

// unarmed_combat.c

#define _LANG_UNARMED_STYLE_COMMANDS ({ "estilo", "estilos" })
#define _LANG_UNARMED_DEFAULT_FUMBLE_MSG "¡Oh, qué torpeza!\n"
#define _LANG_UNARMED_DEFAULT_CRITICAL_MSG "¡Oh, qué habilidad!\n"
#define _LANG_UNARMED_LOCALIZATION_PREP "en"
#define _LANG_NO_NEED_WHEN_DEAD "Estás en forma espiritual, no necesitas conocer eso.\n"
#define _LANG_UNARMED_CODE_ERROR "Parece que ha habido algún problema con tus técnicas de combate, " + \
                     "habla con algún programador.\n"

#define _LANG_UNARMED_DODGE_MESSAGE_ATT defender->query_cap_name() + " logra esquivar tu ataque.\n"
#define _LANG_UNARMED_DODGE_MESSAGE_DEF attacker->query_cap_name() + " intenta golpearte pero logras " + \
                                        "esquivar su ataque.\n"
#define _LANG_UNARMED_DODGE_MESSAGE_ROOM attacker->query_cap_name() + \
                                        " trata de golpear a " + defender->query_cap_name() + \
                                        " pero "+defender->query_demonstrative()+" logra esquivar su ataque.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_ATT defender->query_cap_name()+" logra bloquear tu ataque.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_DEF attacker->query_cap_name()+" intenta golpearte pero logras " + \
                                        "bloquear su ataque.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_ROOM attacker->query_cap_name()+" trata de golpear a " + \
         defender->query_cap_name()+" pero "+defender->query_demonstrative()+" para el ataque.\n"
// #define _LANG_UNARMED_HIT_MSG_ATT capitalize(msgs[0]) + " a " + def->query_cap_name() + place + ".\n"
// #define _LANG_UNARMED_HIT_MSG_DEF att->query_cap_name() + " " + msgs[1] + place + ".\n"
// #define _LANG_UNARMED_HIT_MSG_ROOM att->query_cap_name() + " " + msgs[2] + " a " + \
//                                         def->query_cap_name() + place + ".\n"

#define _LANG_UNARMED_LIST_HEADER "> %^GREEN%^Conoces los siguientes estilos de lucha: %^RESET%^<"
#define _LANG_UNARMED_LIST_FOOTER "  Tu estilo de lucha actual es '"+query_current_unarmed_style()+"'.\n" + \
                                  "  Utiliza: estilo <nombre> para cambiarlo.\n"
#define _LANG_UNARMED_NOT_KNOWN "No conoces el estilo de combate '"+style+"'.\n"

#define _LANG_UNARMED_STYLE_INFO "Estilo de combate desarmado: '"+style+"'. Nivel de habilidad: "+unarmed_ability+"%.\n"
#define _LANG_UNARMED_STYLE_IMPROVED "¡Has mejorado tus habilidades en "+style+"!\n"

// hold.c

#define _LANG_HOLD_VERBS ({ "empuñar", "empunyar", "sostener" })
#define _LANG_HOLD_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"
#define _LANG_HOLD_NOT_IN_INV "No llevas eso en tu inventario.\n"
#define _LANG_HOLD_ALREADY_HOLDING "¡Ya estás sosteniendo eso!\n"
#define _LANG_HOLD_CANNOT_HOLD "No puedes sostener eso.\n"
#define _LANG_HOLD_HANDS_NEEDED "No tienes suficientes manos libres para sostener eso.\n"
#define _LANG_HOLD_MSG "Sostienes tu " + ob->query_name() + ".\n"

#define _LANG_UNHOLD_VERBS ({ "desempuñar", "desempunyar", "soltar" })
#define _LANG_UNHOLD_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"
#define _LANG_HOLD_NOT_HOLDING "No estás sosteniendo ese objeto.\n"
#define _LANG_UNHOLD_MSG "Dejas de sostener tu " + ob->query_name() + ".\n"
#define _LANG_CANNOT_UNHOLD_MSG "No puedes dejar de sostener tu " + ob->query_name() + ".\n"

// wear.c

#define _LANG_WEAR_VERBS ({ "ponerse", "ponerme", "vestir" })
#define _LANG_WEAR_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"
#define _LANG_WEAR_NOT_IN_INV "No llevas eso en tu inventario.\n"
#define _LANG_WEAR_ALREADY_WEARING "¡Ya llevas eso puesto!\n"
#define _LANG_WEAR_NOT_WEARABLE "¡Este objeto no se puede vestir!\n"
#define _LANG_WEAR_STILL_LOADING "Tu equipamiento todavía está cargándose, aún no puedes ponértelo.\n"
#define _LANG_WEAR_WRONG_BODY_TYPE "Este equipo no se ajusta bien a tu cuerpo, " + \
              "no puedes ponértelo.\n"
#define _LANG_WEAR_SAME_TYPE "Ya tienes puesto un objeto del mismo tipo.\n"
#define _LANG_WEAR_NO_SIZE "Este objeto tiene un fallo (no tiene tamaño), pide ayuda a un programador.\n"
#define _LANG_WEAR_START "Empiezas a ponerte tu " + ob->query_name() + ".\n"
// #define _LANG_WEAR_END "Terminas de ponerte tu " + ob->query_name() + ".\n"
#define _LANG_WEAR_FULL_SET "%^BOLD%^Te pones el conjunto completo de: " + \
                main->query_piece_set_name() + ".%^RESET%^\n"
#define _LANG_WEAR_PASSED_OUT_MSG "Todavía estás intentando ponerte tu " + ob->query_name() + ".\n"
#define _LANG_CANNOT_WEAR_MSG "No puedes ponerte tu " + ob->query_name() + ".\n"

#define _LANG_UNWEAR_VERBS ({ "quitarse", "quitarme", "desvestir" })
#define _LANG_UNWEAR_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"                
#define _LANG_UNWEAR_NOT_WEARING "No puedes quitarte algo que no lleves puesto.\n"
#define _LANG_UNWEAR_START "Te quitas tu " + ob->query_name() + ".\n"
// #define _LANG_UNWEAR_END "Te quitas tu " + ob->query_name() + ".\n"
#define _LANG_UNWEAR_FULL_SET "%^BOLD%^Dejas de llevar el conjunto completo de: " + \
              main_object->query_piece_set_name() + ".%^RESET%^\n"
#define _LANG_CANNOT_UNWEAR_MSG "No puedes quitarte tu " + ob->query_name() + ".\n"
