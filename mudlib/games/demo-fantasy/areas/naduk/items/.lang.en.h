
#define _LANG_ITEMS_FOUNTAIN_NAME "circe fountain"
#define _LANG_ITEMS_FOUNTAIN_SHORT "Circe Fountain"
#define _LANG_ITEMS_FOUNTAIN_ALIASES ({ "fountain", "circe" })
#define _LANG_ITEMS_FOUNTAIN_LONG "This is a hexagonal shaped fountain, " + \
     "sculpted in white granite. The water plays in its " + \
     "interior forming numerous forms and making all kinds of " + \
     "pirouettes that delight your eyes and ears. This is an impressive fountain.\n"
#define _LANG_ITEMS_FOUNTAIN_TARGETS ({ "water", "water from the fountain" })
#define _LANG_ITEMS_FOUNTAIN_FAIL "Drink what? Maybe you want to drink water from the fountain.\n"
#define _LANG_ITEMS_FOUNTAIN_DRINK_VERBS ({ "drink" })
#define _LANG_ITEMS_FOUNTAIN_DRINK_MSG_ME "You approach the fountain ready to drink some " + \
       "fresh water. When you taste it, you cleanse your soul and " + \
       "you quench your thirst. You have never been so blissful... You smile.\n"
#define _LANG_ITEMS_FOUNTAIN_DRINK_MSG_OTHERS this_player()->query_cap_name() + \
      " drinks water from the Circe Fountain.\n" + this_player()->query_cap_name() + \
      " smiles.\n"
