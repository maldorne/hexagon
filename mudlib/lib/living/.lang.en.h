
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
#define _LANG_COMMS_NO_ENV "In this emptyness nobody can say or hear a thing.\n"
#define _LANG_COMMS_NO_SPOKEN_LANG "is not a language that could be spoken.\n"

// queue

#define _LANG_QUEUE_DIDNT_WORK "The attempt of doing '%^RED%^" + \
                            curr_act + "%^RESET%^' didn't work.\n"

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
#define _LANG_HANDLE_GET_CANNOT "Cannot find any "+s2+" around here.\n"
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

#define _LANG_HANDLE_GIVE_SYNTAX "Syntax: "+query_verb()+" <item> <to/in> <somewhere>\n"
#define _LANG_HANDLE_PUT_VERB "put"
#define _LANG_HANDLE_PUT_PREP "in"
#define _LANG_HANDLE_GIVE_PREP "to"
#define _LANG_HANDLE_GIVE_CANNOT "Cannot find "+person+".\n"

// combat.c

#define _LANG_COMBAT_BEING_ATTACKED "%^BOLD%^You are being attacked by %^RESET%^" + \
                        name + "%^BOLD%^.%^RESET%^\n\n"

// death.c

#define _LANG_DEATH_DEATH_BLOW_ROOM "\n" + killed_by->query_cap_name() + \
      " deals the death blow to " + this_object()->query_cap_name() + ".\n\n"
#define _LANG_DEATH_DEATH_BLOW_ATT "You deal the death blow to " + this_object()->query_cap_name() + "."
#define _LANG_DEATH_DEATH_BLOW_DEF killed_by->query_cap_name() + " deals you the death blow."
#define _LANG_DEATH_DEATH_BLOW_NO_KILLER this_object()->query_cap_name() + \
      " has died horribly.\n"
#define _LANG_DEATH_CORPSE_DEF "Your body loses its last breaths of life"
#define _LANG_DEATH_CORPSE_ATT this_object()->query_cap_name() + " falls to the ground wasted"

// unarmed_combat.c

#define _LANG_UNARMED_STYLE_COMMANDS ({ "style", "styles" })
#define _LANG_UNARMED_DEFAULT_FUMBLE_MSG "Oh, how awkward!\n"
#define _LANG_UNARMED_DEFAULT_CRITICAL_MSG "Oh, impressive!\n"
#define _LANG_UNARMED_LOCALIZATION_PREP "on"                        
#define _LANG_NO_NEED_WHEN_DEAD "In your spiritual form you do not need to know that.\n"
#define _LANG_UNARMED_CODE_ERROR "It seems something wrong happen with your combat styles, notify it to an administrator.\n"

#define _LANG_UNARMED_DODGE_MESSAGE_ATT defender->query_cap_name() + " is able to dodge your attack.\n"
#define _LANG_UNARMED_DODGE_MESSAGE_DEF attacker->query_cap_name() + " tries to hit you, but you dodge " + attacker->query_objective() + \
                                        " attack.\n"
#define _LANG_UNARMED_DODGE_MESSAGE_ROOM attacker->query_cap_name() + \
                                        " tries to hit " + defender->query_cap_name() + \
                                        ", but "+defender->query_demonstrative()+" dodges " + attacker->query_objective() + " attack.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_ATT defender->query_cap_name()+" is able to block your attack.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_DEF attacker->query_cap_name()+" tries to hit you, but you block " + attacker->query_objective() + \
                                        " attack.\n"
#define _LANG_UNARMED_BLOCK_MESSAGE_ROOM attacker->query_cap_name()+" tries to hit " + \
         defender->query_cap_name()+", but "+defender->query_demonstrative()+" blocks " + attacker->query_objective() + " attack.\n"
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
