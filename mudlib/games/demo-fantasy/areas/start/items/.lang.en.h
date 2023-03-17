
// base.c

#define _LANG_ITEMS_CHOOSE_VERBS ({ "choose" })
#define _LANG_ITEMS_CHOOSE_FAIL capitalize(query_verb()) + " what?\n"
#define _LANG_ITEMS_FIGHTER_MSG "From now on your wandering through the realms will be " + \
      "marked by your honor and by your skill with the sword.\n\n" + \
      "May the Gods accompany you on your journey...\n\n"
#define _LANG_ITEMS_MIXED_MSG "From now on your wandering through the world will be marked " + \
    "by your ability to negotiate, your skill to not " + \
    "draw attention and, in the worst cases, " + \
    "your mastery with the sword, deceit or even flight.\n\n" + \
    "May the Gods accompany you on your journey...\n\n"
#define _LANG_ITEMS_SCHOLAR_MSG "From now on your wandering through the world will be marked " + \
    "by your ability to study, your intelligence and the wisdom you get in your travels.\n\n" + \
    "May the Gods accompany you on your journey...\n\n"

// sword.c

#define _LANG_SWORD_NAME "sword"
#define _LANG_SWORD_DESC "A shiny and polished sword made of an unknown metal " + \
      "you can't recognize. It seems to contain the strength and energy of " + \
      "ancient warriors. With a sword like this, any fighter could shape " + \
      "their own destiny, traveling to places where only the law of " + \
      "steel prevails.\n\n%^BOLD%^If you pick up the sword, you will become a fighter.%^RESET%^\n"


// key.c

#define _LANG_KEY_NAME "key"
#define _LANG_KEY_DESC "A small, common-looking key with a little ring on one end. " + \
      "It's made of a fairly dark and apparently sturdy metal. Its strange shape  " + \
      "seems to make it capable of opening even the most complex lock.  " + \
      "Without a doubt, this key could unlock doors hiding incredible secrets  " + \
      "or great fortunes, and promises endless adventures...\n" + \
      "\n%^BOLD%^If you take the key, you will become the mixed class, the explorer.%^RESET%^\n"

// book.c

#define _LANG_BOOK_NAME "book"
#define _LANG_BOOK_DESC "This dusty, leather-bound book has a faded gold emblem " + \
    "on its cover depicting an owl perched atop an open book. Its pages are " + \
    "yellowed and well-worn, with tiny handwritten letters covering every inch. " + \
    "Its weight feels significant, as if holding it in your hands will grant you " + \
    "access to all the knowledge contained within. Without a doubt, this tome " + \
    "holds the secrets to unlocking the mysteries of the universe and promises " + \
    "to transform you into a great scholar, forever changing the course of your life...\n" + \
    "\n%^BOLD%^If you take the book, you will become a scholar.%^RESET%^\n"
