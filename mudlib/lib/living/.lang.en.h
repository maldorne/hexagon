
// queue

#define _LANG_DIDNT_WORK "The attempt of doing '%^RED%^" + \
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