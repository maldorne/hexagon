
// common

#define _LANG_CANNOT_DO_WHEN_DEAD "You cannot do that while you are dead.\n"

// communicate.c

#define _LANG_SAY_VERBS ({ "say" })
#define _LANG_TELL_VERBS ({ "tell" })
#define _LANG_WHISPER_VERBS ({ "whisper" })
#define _LANG_SPEAK_VERBS ({ "speak" })
#define _LANG_SHOUT_VERBS ({ "shout" })
#define _LANG_COMMNS_LANG_PREP "in"
#define _LANG_COMMS_SYNTAX "Syntax: " + query_verb() + " <something>\n"
#define _LANG_COMMS_NO_CURRENT_LANG "You have to choose a language.\n" + \
                                    "Use the '"+_LANG_SPEAK_VERBS[0]+"' command.\n"

#define _LANG_COMM_I_EXCLAIM "you exclaim"
#define _LANG_COMM_I_ASK "you ask"
#define _LANG_COMM_I_SAY "you say"
#define _LANG_COMM_THEY_EXCLAIM "exclaims"
#define _LANG_COMM_THEY_ASK "asks"
#define _LANG_COMM_THEY_SAY "says"
#define _LANG_COMM_EXCLAIMING "exclaiming"
#define _LANG_COMM_ASKING "asking"
#define _LANG_COMM_SAYING "saying"
#define _LANG_COMMS_NO_ENV "In this emptyness nobody can say or hear a thing.\n"
#define _LANG_COMMS_NO_SPOKEN_LANG "is not a language that could be spoken.\n"

#define _LANG_TELL_SYNTAX "Syntax: tell <who> <message>\n"
#define _LANG_TELL_NO_LANGUAGE "You must select a language to speak, use 'speak' for that.\n"
#define _LANG_TELL_LANG_NOT_SPOKEN " is not a language that could be spoken.\n"
#define _LANG_TELL_LANG_NOT_DISTANCE " is not a language that could be spoken at a distance.\n"
#define _LANG_TELL_CANNOT_FIND "Cannot find '" + capitalize(str) + "' around here.\n"
#define _LANG_TELL_YOURSELF "Speaking to yourself is not very useful.\n"
#define _LANG_TELL_NO_CONNECTION ob->query_cap_name() + " has " + \
                                  ob->query_possessive() + " connection down.\n"
#define _LANG_TELL_MSG_THEM "tells you"
#define _LANG_TELL_MSG_ME "You tell"
#define _LANG_TELL_MSG_ASK_THEM "asks you"
#define _LANG_TELL_MSG_ASK_ME "You ask"

// queue

#define _LANG_QUEUE_DIDNT_WORK "The attempt of doing '%^RED%^" + \
                            curr_act + "%^RESET%^' didn't work.\n"
#define _LANG_QUEUE_RESTART_OPTIONS ({ "restart" })
#define _LANG_QUEUE_HALT_OPTIONS ({ "halt" })
#define _LANG_QUEUE_ABORT_OPTIONS ({ "abort" })
#define _LANG_QUEUE_DONT_NEED_RESET "You don't need to restart your character at this moment.\n"
#define _LANG_QUEUE_RESET_MSG "Trying to restart your character...\n"
#define _LANG_QUEUE_HALT_MSG "Command queue deleted.\n"
#define _LANG_QUEUE_NO_ABORT "You are not in the middle of an action that can be aborted.\n"
#define _LANG_QUEUE_RESTART_WARNING "%^BOLD%^ALERT%^RESET%^:\n" + \
      "There has been an error. Try to '%^BOLD%^restart%^RESET%^' your character to " + \
      "solve it.\n"
#define _LANG_QUEUE_TOO_MANY "Too many commands in the queue; ignoring '"+str+"'.\n"
#define _LANG_QUEUE_BLOCKED_MSG "You are busy, can't do anything.\n"
#define _LANG_QUEUE_ERROR_INSERTING "Error inserting in the command queue '"+str+"', "+i+"\n"
#define _LANG_QUEUE_VERB_SAY "say"
#define _LANG_QUEUE_VERB_EMOTE "emotion"
#define _LANG_QUEUE_ABORT_MSG "You are busy doing an abortable action.\n"

// handle

#define _LANG_HANDLE_WAIT_LOADING "You have to wait until your equipment is loaded.\n"
#define _LANG_HANDLE_DEAD "Being dead your actions does not have effect.\n "
#define _LANG_HANDLE_STILL "You are still busy with all that.\n"
#define _LANG_HANDLE_FAIL "You cannot "+query_verb()+" "+query_multiple_short(fail, 1)+".\n"

#define _LANG_HANDLE_GET_VERBS ({ "get", "take" })
#define _LANG_HANDLE_GIVE_VERBS ({ "give", "put" })
#define _LANG_HANDLE_DROP_VERBS ({ "drop" })

#define _LANG_HANDLE_GET_SYNTAX "Syntax: "+query_verb()+" <item> [from <somewhere>]\n"
#define _LANG_HANDLE_GET_PREP "from"
#define _LANG_HANDLE_GET_CANNOT_FIND "Cannot find any "+s2+" around here.\n"
#define _LANG_HANDLE_GET_NOTHING "Nothing to "+query_verb()+".\n"
#define _LANG_HANDLE_GET_MESSAGES ({ "You get", "You get with a little difficulty", \
          "You get as you can", \
          "It's very difficult, but you get", \
          "Using all your strength you get" })
#define _LANG_HANDLE_GET_MESSAGES_OTHERS ({ "gets", "gets with a little difficulty", \
            "gets as "+this_object()->query_possessive()+" can", \
            "has a lot of difficulty to get", \
            "using all "+this_object()->query_possessive()+" strength "+ \
            this_object()->query_possessive()+" can get" })
// this will never happen in english
#define _LANG_HANDLE_GET_COINS_MATCH "%scoins from %s"

#define _LANG_HANDLE_GIVE_SYNTAX "Syntax: "+query_verb()+" <item> <to/in> <somewhere>\n"
#define _LANG_HANDLE_PUT_VERB "put"
#define _LANG_HANDLE_PUT_PREP "in"
#define _LANG_HANDLE_GIVE_PREP "to"
#define _LANG_HANDLE_TAKE_PREP "from"
#define _LANG_HANDLE_GIVE_CANNOT_FIND "Cannot find "+person+".\n"
#define _LANG_HANDLE_NO_PER "Nada que "+query_verb()+con+per[j]->short()+".\n"
#define _LANG_HANDLE_REFUSE " no quiere que le den nada.\n"
#define _LANG_HANDLE_REFUSE_GIVE " ha intentado darte algo, pero no te interesa aceptar cosas.\n" + \
            "\t(escribe 'consentir dar sí' si quieres cambiarlo).\n"
#define _LANG_HANDLE_GIVE_ME "You " + ((query_verb()=="give")?"give":"put")
#define _LANG_HANDLE_GIVE_ROOM ((query_verb()=="give")?"gives":"puts")
#define _LANG_HANDLE_GIVE_THEY "gives you"

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
#define _LANG_HANDLE_DROP_CANNOT_FIND "You don't see any "+str+" to drop.\n"
#define _LANG_HANDLE_DROP_THEY "drops"
#define _LANG_HANDLE_DROP_ME "You drop"


// combat.c

#define _LANG_COMBAT_BEING_ATTACKED "%^BOLD%^You are being attacked by %^RESET%^" + \
                        name + "%^BOLD%^.%^RESET%^\n\n"
#define _LANG_COMBAT_PASSED_OUT_MSG "You are too stunned to fight back.\n"
#define _LANG_COMBAT_CHASING "You are chasing "
#define _LANG_COMBAT_KILL_NO_ATTACKEE_MSG_ME "You have killed "+his_name+".\n"
#define _LANG_COMBAT_KILL_NO_ATTACKEE_MSG_ROOM this_object()->query_cap_name() + \
            " has killed "+his_name+".\n"
#define _LANG_COMBAT_STOP_CHASING "You stop chasing "

// death.c

#define _LANG_DEATH_DEATH_BLOW_ROOM "\n" + killed_by->query_cap_name() + \
      " deals the death blow to " + this_object()->query_cap_name() + ".\n\n"
#define _LANG_DEATH_DEATH_BLOW_ATT "You deal the death blow to " + this_object()->query_cap_name() + "."
#define _LANG_DEATH_DEATH_BLOW_DEF killed_by->query_cap_name() + " deals you the death blow."
#define _LANG_DEATH_DEATH_BLOW_NO_KILLER this_object()->query_cap_name() + \
      " has died horribly.\n"
#define _LANG_DEATH_CORPSE_DEF "Your body loses its last breaths of life"
#define _LANG_DEATH_CORPSE_ATT this_object()->query_cap_name() + " falls to the ground wasted"

#define _LANG_HANDLE_MY_MESS_WEAPONS ", and your weapons fall to the ground"
#define _LANG_HANDLE_ROOM_MESS_WEAPONS ", and "+this_object()->query_possessive()+" weapons fall to the ground"
#define _LANG_HANDLE_MY_MESS_BELONGINGS "Your belongings fall from your hands"
#define _LANG_HANDLE_ROOM_MESS_BELONGINGS this_object()->query_cap_name()+"'s belongings fall from "+this_object()->query_possessive()+" hands"

// unarmed_combat.c

#define _LANG_UNARMED_STYLE_COMMANDS ({ "style", "styles" })
#define _LANG_UNARMED_DEFAULT_FUMBLE_MSG "Oh, how awkward!\n"
#define _LANG_UNARMED_DEFAULT_CRITICAL_MSG "Oh, impressive!\n"
#define _LANG_UNARMED_LOCALIZATION_PREP "on"                        
#define _LANG_NO_NEED_WHEN_DEAD "In your spiritual form you do not need to know that.\n"
#define _LANG_UNARMED_CODE_ERROR "It seems something wrong happen with your combat styles, notify it to an administrator.\n"

#define _LANG_UNARMED_DODGE_MESSAGE_ATT defender->query_cap_name() + " is able to dodge your attack.\n"
#define _LANG_UNARMED_DODGE_MESSAGE_DEF attacker->query_cap_name() + " tries to hit you, but you dodge " + attacker->query_possessive() + \
                                        " attack.\n"
#define _LANG_UNARMED_DODGE_MESSAGE_ROOM attacker->query_cap_name() + \
                                        " tries to hit " + defender->query_cap_name() + \
                                        ", but "+defender->query_demonstrative()+" dodges " + attacker->query_possessive() + " attack.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_ATT defender->query_cap_name()+" is able to block your attack.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_DEF attacker->query_cap_name()+" tries to hit you, but you block " + attacker->query_possessive() + \
                                        " attack.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_ROOM attacker->query_cap_name()+" tries to hit " + \
         defender->query_cap_name()+", but "+defender->query_demonstrative()+" blocks " + attacker->query_possessive() + " attack.\n"
// #define _LANG_UNARMED_HIT_MSG_ATT capitalize(msgs[0]) + " " + def->query_cap_name() + place + ".\n"
// #define _LANG_UNARMED_HIT_MSG_DEF att->query_cap_name() + " " + msgs[1] + place + ".\n"
// #define _LANG_UNARMED_HIT_MSG_ROOM att->query_cap_name() + " " + msgs[2] + " " + \
//                                         def->query_cap_name() + place + ".\n"

#define _LANG_UNARMED_LIST_HEADER "> %^GREEN%^You know the following combat styles: %^RESET%^<"
#define _LANG_UNARMED_LIST_FOOTER "  Your current combat style is '"+query_current_unarmed_style()+"'.\n" + \
                                  "  Use: style <name> to change it.\n"
#define _LANG_UNARMED_NOT_KNOWN "You don't know the combat style called '"+style+"'.\n"

#define _LANG_UNARMED_STYLE_INFO "Unarmed combat style: '"+style+"'. Ability: "+unarmed_ability+"%.\n"
#define _LANG_UNARMED_STYLE_IMPROVED "You improved your abilities with "+style+"!\n"

// hold.c

#define _LANG_HOLD_VERBS ({ "hold", "wield" })
#define _LANG_HOLD_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_HOLD_NOT_IN_INV "You don't have that in your inventory.\n"
#define _LANG_HOLD_ALREADY_HOLDING "Already holding that!\n"
#define _LANG_HOLD_CANNOT_HOLD "Cannot hold that.\n"
#define _LANG_HOLD_HANDS_NEEDED "Not enough free hands to hold that.\n"
#define _LANG_HOLD_MSG "You hold your " + ob->query_name() + ".\n"

#define _LANG_UNHOLD_VERBS ({ "unhold" })
#define _LANG_UNHOLD_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_HOLD_NOT_HOLDING "You are not holding that.\n"
#define _LANG_UNHOLD_MSG "You stop holding your " + ob->query_name() + ".\n"
#define _LANG_CANNOT_UNHOLD_MSG "You cannot unhold your " + ob->query_name() + ".\n"

// wear.c

#define _LANG_WEAR_VERBS ({ "wear" })
#define _LANG_WEAR_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_WEAR_NOT_IN_INV "You don't have that in your inventory.\n"
#define _LANG_WEAR_ALREADY_WEARING "You are already wearing that!\n"
#define _LANG_WEAR_NOT_WEARABLE "That item is not wearable!\n"
#define _LANG_WEAR_STILL_LOADING "Your inventory is still loading, cannot wear it yet.\n"
#define _LANG_WEAR_WRONG_BODY_TYPE "This item does not fit well to your body, " + \
              "you cannot wear it.\n"
#define _LANG_WEAR_SAME_TYPE "You are already wearing an item of the same type.\n"
#define _LANG_WEAR_NO_SIZE "This object has an error (has no size), please tell a coder.\n"
#define _LANG_WEAR_START "You start wearing your " + ob->query_name() + ".\n"
// #define _LANG_WEAR_END "You put on your " + ob->query_name() + ".\n"
#define _LANG_WEAR_FULL_SET "%^BOLD%^You start wearing the full set: " + \
                main->query_piece_set_name() + ".%^RESET%^\n"
#define _LANG_WEAR_PASSED_OUT_MSG "You are still trying to wear your " + ob->query_name() + ".\n"
#define _LANG_CANNOT_WEAR_MSG "You cannot wear your " + ob->query_name() + ".\n"

#define _LANG_UNWEAR_VERBS ({ "unwear" })
#define _LANG_UNWEAR_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_UNWEAR_NOT_WEARING "You cannot unwear something you are not wearing.\n"
#define _LANG_UNWEAR_START "You take off your " + ob->query_name() + ".\n"
// #define _LANG_UNWEAR_END "You take off your " + ob->query_name() + ".\n"
#define _LANG_UNWEAR_FULL_SET "%^BOLD%^You stop wearing the full set: " + \
              main_object->query_piece_set_name() + ".%^RESET%^\n"
#define _LANG_CANNOT_UNWEAR_MSG "You cannot unwear your " + ob->query_name() + ".\n"

// equip.c

#define _LANG_EQUIP_VERBS ({ "equip" })
#define _LANG_EQUIP_SYNTAX "Syntax: equip\n" + \
              "To equip automatically at login, use 'configuration'.\n"
#define _LANG_EQUIP_WIELDING "Wielding"
#define _LANG_EQUIP_HOLDING "Holding"
#define _LANG_EQUIP_WEARING "Wearing"
#define _LANG_EQUIP_TIED "Tied"
#define _LANG_EQUIP_CARRYING "Carrying"
#define _LANG_EQUIP_EMPTY_HANDS "Your hands are empty."
#define _LANG_EQUIP_NO_MONEY "You have no money."
#define _LANG_EQUIP_MONEY "You are carrying " + ret + "."
#define _LANG_EQUIP_OTHER_MONEY_1 "Seems to have no money."
#define _LANG_EQUIP_OTHER_MONEY_2 "Does not seem to carry a lot of money."
#define _LANG_EQUIP_OTHER_MONEY_3 "Seems to have money enough."
#define _LANG_EQUIP_OTHER_MONEY_4 "Look like carrying a lot of money!"

// health

#define _LANG_HEALTH_DRUNK_PASSES "The headache passes.\n"
#define _LANG_HEALTH_DRUNK_STARTS "Your head starts to hurt, how delightful.\n"
#define _LANG_HEALTH_PRACTICE_LOSER "The blow from " + winner->query_cap_name() + " leaves you incapacitated. " + \
              "You fall to the ground unconscious.\n"
#define _LANG_HEALTH_PRACTICE_WINNER "Your opponent falls unconscious, you have won the fight.\n"
#define _LANG_HEALTH_PRACTICE_PASSED_PROP "You are unconscious.\n"

#define _LANG_HEALTH_YOU_ARE "You are"
#define _LANG_HEALTH_THEY_ARE this_object()->query_pronoun() + " is"
#define _LANG_HEALTH_DEATH_THRESHOLD "at death's door"
#define _LANG_HEALTH_CRITICAL_THRESHOLD "in critical condition"
#define _LANG_HEALTH_BAD_THRESHOLD "in bad shape"
#define _LANG_HEALTH_FAIR_THRESHOLD "slightly injured"
#define _LANG_HEALTH_SLIGHTLY_INJURED "lightly injured"
#define _LANG_HEALTH_PERFECT_HEALTH "in perfect health condition"
#define _LANG_HEALTH_GOOD_HEALTH "in good shape"

#define _LANG_HEALTH_VOLUME_SOBER "sober"
#define _LANG_HEALTH_VOLUME_HAPPY "happy"
#define _LANG_HEALTH_VOLUME_DRUNK "blind"
#define _LANG_HEALTH_VOLUME_VERY_DRUNK "very blind"
#define _LANG_HEALTH_VOLUME_DEAD_DRUNK "dead to the world"
#define _LANG_HEALTH_VOLUME_NEAR_DEATH "near death"
#define _LANG_HEALTH_VOLUME_PRAYING_FOR_DEATH "pray for a painless death"

// social.c

#define _LANG_SOCIAL_FORGET_ABOUT_IT "Forget about it.\n"
#define _LANG_SOCIAL_LEVEL_UP "Level up!"
