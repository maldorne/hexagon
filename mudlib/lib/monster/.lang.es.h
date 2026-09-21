
// friends.c

#define _LANG_FRIENDS_PILE_IN "¡" + this_object()->query_cap_name() + " se une a la batalla!\n"
#define _LANG_FRIENDS_PILE_IN_PROTECTING capitalize(this_object()->query_short()) + \
          " comienza a proteger a " + ob->query_short() + ".\n"

// chatter.c

#define _LANG_SEEMS_CONFUSED "parece confuso por un momento." 

// quest-giver

#define _LANG_GIVER_ASK_VERBS ({ "trabajo", "trabajos" })
#define _LANG_GIVER_ACCEPT_VERBS ({ "aceptar" })
#define _LANG_GIVER_HAND_IN_VERBS ({ "entregar" })

#define _LANG_GIVER_NOT_YOUR_KIND this_object()->query_cap_name() + " te mira de arriba abajo y " + \
      "te da la espalda sin decir palabra.\n"
#define _LANG_GIVER_NOTHING_TO_OFFER this_object()->query_cap_name() + " te dice: No tengo nada para ti " + \
      "por ahora.\n"
#define _LANG_GIVER_OFFER this_object()->query_cap_name() + " te habla de un trabajo: %^BOLD%^" + \
      quest->query_title() + "%^RESET%^.\n" + quest->query_description() + \
      "Escribe 'aceptar' si te interesa.\n"
#define _LANG_GIVER_COME_BACK_DONE this_object()->query_cap_name() + " te dice: Ya has hecho lo que te pedí. " + \
      "Escribe 'entregar' y hablamos.\n"
#define _LANG_GIVER_NOTHING_TO_ACCEPT this_object()->query_cap_name() + " no tiene ningún trabajo que darte.\n"
#define _LANG_GIVER_NOTHING_TO_HAND_IN this_object()->query_cap_name() + " no espera nada de ti todavía.\n"
