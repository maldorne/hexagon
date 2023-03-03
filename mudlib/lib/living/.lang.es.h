
// common

#define _LANG_CANNOT_DO_WHEN_DEAD "No puedes hacer eso mientras estás muerto.\n"

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
#define _LANG_HANDLE_GET_CANNOT_FIND "No encuentras ningún "+s2+" por aquí.\n"
#define _LANG_HANDLE_GET_NOTHING "Nada que "+query_verb()+".\n"
#define _LANG_HANDLE_GET_MESSAGES ({ "Coges", "Coges con un poco de dificultad", \
          "Te las arreglas como puedes para coger", \
          "Te resulta muy difícil coger", \
          "Usas toda tu fuerza y a duras penas logras coger" })
#define _LANG_HANDLE_GET_MESSAGES_OTHERS ({ "coge", "coge con un poco de dificultad", \
            "se las arregla como puede para coger", \
            "tiene muchas dificultades para coger", \
            "usa toda su fuerza y a duras penas logra coger" })
#define _LANG_HANDLE_GET_COINS_MATCH "%smonedas de %s"

#define _LANG_HANDLE_GIVE_SYNTAX "Sintaxis: "+query_verb()+" <objeto> <a/en> <objeto>\n"
#define _LANG_HANDLE_PUT_VERB "poner"
#define _LANG_HANDLE_PUT_PREP "en"
#define _LANG_HANDLE_GIVE_PREP "a"
#define _LANG_HANDLE_TAKE_PREP "de"
#define _LANG_HANDLE_GIVE_CANNOT_FIND "No se puede encontrar "+person+".\n"
#define _LANG_HANDLE_NO_PER "Nada que "+query_verb()+con+per[j]->short()+".\n"
#define _LANG_HANDLE_REFUSE " no quiere que le den nada.\n"
#define _LANG_HANDLE_REFUSE_GIVE " ha intentado darte algo, pero no te interesa aceptar cosas.\n" + \
            "\t(escribe 'consentir dar sí' si quieres cambiarlo).\n"
#define _LANG_HANDLE_GIVE_ME ((query_verb()=="dar")?"Das":"Pones")
#define _LANG_HANDLE_GIVE_ROOM ((query_verb()=="dar")?"da":"pone")
#define _LANG_HANDLE_GIVE_THEY "te da"

#define _LANG_HANDLE_CANNOT "No puedes"

#define _LANG_HANDLE_WEIGHT_LIGHT_THEM " está incomodado por la carga adicional.\n"
#define _LANG_HANDLE_WEIGHT_MEDIUM_THEM " sucumbe momentáneamente bajo el peso de la carga.\n"
#define _LANG_HANDLE_WEIGHT_HEAVY_THEM " se tambalea mientras coge la carga.\n"
#define _LANG_HANDLE_WEIGHT_HEAVIER_THEM " se hunde al recibir tanto peso.\n"
#define _LANG_HANDLE_WEIGHT_LIGHT_ME "Durante unos momentos te incomoda la carga adicional.\n"
#define _LANG_HANDLE_WEIGHT_MEDIUM_ME "Sucumbes momentáneamente bajo el peso de la carga.\n"
#define _LANG_HANDLE_WEIGHT_HEAVY_ME "Te tambaleas al coger la carga. A duras penas puedes con ella.\n"
#define _LANG_HANDLE_WEIGHT_HEAVIER_ME "Te hundes bajo un peso que apenas puedes cargar.\n"
#define _LANG_HANDLE_PASSED_OUT_MSG "Todavía estas intentando coger la carga.\n"

#define _LANG_HANDLE_DROP_SYNTAX "Sintaxis: "+query_verb()+" <objeto(s)>\n"
#define _LANG_HANDLE_DROP_CANNOT_FIND "No ves ningún "+str+" que dejar.\n"
#define _LANG_HANDLE_DROP_THEY "deja"
#define _LANG_HANDLE_DROP_ME "Dejas"

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

#define _LANG_HANDLE_MY_MESS_WEAPONS ", y tus armas caen al suelo."
#define _LANG_HANDLE_ROOM_MESS_WEAPONS ", y sus armas caen de sus manos y chocan contra el suelo"
#define _LANG_HANDLE_MY_MESS_BELONGINGS "Tus pertenencias caen de tus manos"
#define _LANG_HANDLE_ROOM_MESS_BELONGINGS "Las pertenencias de "+this_object()->query_cap_name()+" caen de sus manos"

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
#define _LANG_WEAR_START "Empiezas a ponerte " + (ob->query_default_plural() ? "tus" : "tu") + " " + \
              ob->query_name() + ".\n"
// #define _LANG_WEAR_END "Terminas de ponerte " + (ob->query_default_plural() ? "tus" : "tu") + " " + \
//            ob->query_name() + ".\n"
#define _LANG_WEAR_FULL_SET "%^BOLD%^Te pones el conjunto completo de: " + \
              main->query_piece_set_name() + ".%^RESET%^\n"
#define _LANG_WEAR_PASSED_OUT_MSG "Todavía estás intentando ponerte " + (ob->query_default_plural() ? "tus" : "tu") + \
              " " + ob->query_name() + ".\n"
#define _LANG_CANNOT_WEAR_MSG "No puedes ponerte " + (ob->query_default_plural() ? "tus" : "tu") + \
              " " + ob->query_name() + ".\n"

#define _LANG_UNWEAR_VERBS ({ "quitarse", "quitarme", "desvestir" })
#define _LANG_UNWEAR_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"                
#define _LANG_UNWEAR_NOT_WEARING "No puedes quitarte algo que no lleves puesto.\n"
#define _LANG_UNWEAR_START "Te quitas " + (ob->query_default_plural() ? "tus" : "tu") + " " + ob->query_name() + ".\n"
// #define _LANG_UNWEAR_END "Te quitas " + (ob->query_default_plural() ? "tus" : "tu") + " " + ob->query_name() + ".\n"
#define _LANG_UNWEAR_FULL_SET "%^BOLD%^Dejas de llevar el conjunto completo de: " + \
              main_object->query_piece_set_name() + ".%^RESET%^\n"
#define _LANG_CANNOT_UNWEAR_MSG "No puedes quitarte " + (ob->query_default_plural() ? "tus" : "tu") + " " + \
              ob->query_name() + ".\n"

// equip.c

#define _LANG_EQUIP_VERBS ({ "equipar", "equiparse" })
#define _LANG_EQUIP_SYNTAX "Sintaxis: equipar o equiparse.\n" + \
              "Para equiparse automáticamente al conectar, utiliza 'configuración'.\n"
#define _LANG_EQUIP_WIELDING "Empuñando"
#define _LANG_EQUIP_HOLDING "Sosteniendo"
#define _LANG_EQUIP_WEARING "Llevando"
#define _LANG_EQUIP_TIED "Atados"
#define _LANG_EQUIP_CARRYING "Cargando"
#define _LANG_EQUIP_EMPTY_HANDS "Estás con las manos vacías."
#define _LANG_EQUIP_NO_MONEY "No llevas dinero encima."
#define _LANG_EQUIP_MONEY "Llevas encima " + ret + "."
#define _LANG_EQUIP_OTHER_MONEY_1 "No parece llevar dinero encima."
#define _LANG_EQUIP_OTHER_MONEY_2 "Apenas parece llevar dinero encima."
#define _LANG_EQUIP_OTHER_MONEY_3 "Parece estar cargado de monedas."
#define _LANG_EQUIP_OTHER_MONEY_4 "¡Lleva una bolsa de monedas a punto de reventar!"

// health

#define _LANG_HEALTH_DRUNK_PASSES "Se te pasa el dolor de cabeza.\n"
#define _LANG_HEALTH_DRUNK_STARTS "Empieza a dolerte la cabeza, qué alegría.\n"
#define _LANG_HEALTH_PRACTICE_LOSER "El golpe de "+winner->query_cap_name()+" te deja incapacitado. Caes al suelo inconsciente.\n"
#define _LANG_HEALTH_PRACTICE_WINNER "Tu contrincante cae inconsciente, has ganado la pelea.\n"
#define _LANG_HEALTH_PRACTICE_PASSED_PROP "Estás inconsciente.\n"

#define _LANG_HEALTH_YOU_ARE "Estás"
#define _LANG_HEALTH_THEY_ARE "Está"
#define _LANG_HEALTH_DEATH_THRESHOLD "al borde la muerte"
#define _LANG_HEALTH_CRITICAL_THRESHOLD "en muy mal estado"
#define _LANG_HEALTH_BAD_THRESHOLD "en mal estado"
#define _LANG_HEALTH_FAIR_THRESHOLD "algo herid" + this_object()->query_vowel()
#define _LANG_HEALTH_SLIGHTLY_INJURED "ligeramente herid" + this_object()->query_vowel()
#define _LANG_HEALTH_PERFECT_HEALTH "en perfecto estado"
#define _LANG_HEALTH_GOOD_HEALTH "en buen estado"

#define _LANG_HEALTH_VOLUME_SOBER "sobrio"
#define _LANG_HEALTH_VOLUME_HAPPY "alegre"
#define _LANG_HEALTH_VOLUME_DRUNK "ciego"
#define _LANG_HEALTH_VOLUME_VERY_DRUNK "muy ciego"
#define _LANG_HEALTH_VOLUME_DEAD_DRUNK "muerto para el mundo"
#define _LANG_HEALTH_VOLUME_NEAR_DEATH "cerca de la muerte"
#define _LANG_HEALTH_VOLUME_PRAYING_FOR_DEATH "reza por una muerte sin dolor"

#define _LANG_VOLUME_SOBER 
#define _LANG_VOLUME_HAPPY 
#define _LANG_VOLUME_BLIND 
#define _LANG_VOLUME_VERY_BLIND 
#define _LANG_VOLUME_DEAD_TO_WORLD 
#define _LANG_VOLUME_NEAR_DEATH 
#define _LANG_VOLUME_PRAY_FOR_PAINLESS_DEATH 
