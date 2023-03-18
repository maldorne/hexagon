
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


// orb.c

#define _LANG_ORB_NAME "orb"
#define _LANG_ORB_DESC "A reddish orb, with subtle vaporous shapes moving inside, " + \
    "giving it a magical and mysterious appearance that attracts you. " + \
    "It seems that the orb can communicate with you in some primitive " + \
    "and unknown way, directly to your mind... it tells you that you " + \
    "should perhaps 'adjust'.\n"
#define _LANG_ADJUST_VERBS ({ "adjust", "arrange", "rearrange" })
#define _LANG_ORB_ALREADY_ADJUSTED "You have already adjust your character sheet.\n" + \
    "How did you get here?\n"
#define _LANG_ORB_RACIAL_BONUSES "\n    Str: %-+2d  Dex: %-+2d  Con: %-+2d  Int: %-+2d\n" + \
    "    Wis: %-+2d  Cha: %-+2d  Per: %-+2d  Wil: %-+2d\n\n"
#define _LANG_ORB_ADJUST_TXT "\nYou are going to use a method of assigning predefined points to finish " + \
    "creating your character. %^BOLD%^RED%^All you have to do%^RESET%^ is select the order " + \
    "of all your attributes from most to least important for " + this_player()->query_objective() + \
    ". These attributes are:\n\n" + \
    "    %^BOLD%^CYAN%^Str%^RESET%^ength\n    %^BOLD%^CYAN%^Dex%^RESET%^terity\n    %^BOLD%^CYAN%^Con%^RESET%^stitution\n    %^BOLD%^CYAN%^Int%^RESET%^elligence\n" + \
    "    %^BOLD%^CYAN%^Wis%^RESET%^dom\n    %^BOLD%^CYAN%^Cha%^RESET%^risma\n    %^BOLD%^CYAN%^Per%^RESET%^ception\n    %^BOLD%^CYAN%^Wil%^RESET%^lpower\n\n" + \
    "The importance of each of these attributes will depend on how you want your " + \
    "character to be in the future, and they will have a very important influence on many " + \
    "aspects of the game. " + \
    "Your racial bonuses are:\n " + racial_bonuses + \
    "Please enter the order of importance (from most to least).\n" + \
    "For example: %^BOLD%^str con dex per int cha wil wis%^RESET%^\n\n" + \
    "Type 'c' or 'cancel' at any time to exit the attribute adjustment."
#define _LANG_ORB_ADJUST_ORDER "\nOrder: "
#define _LANG_ORB_ADJUST_INVALID "That order is not valid.\n"
#define _LANG_ORB_ADJUST_TRY_AGAIN "Try again with a pattern in the form: %^BOLD%^str con dex per int cha wil wis%^RESET%^\n"
#define _LANG_ORB_ADJUST_CANCELLED "Adjust cancelled.\n"
#define _LANG_ORB_ADJUST_EIGHT "You must include all eight attributes.\n"
#define _LANG_ORB_ADJUST_CONFIRM "You have selected: "+spew+"\n\nIs this correct? (yes/no/cancel) "
#define _LANG_ATTRIBUTES ({ "str", "dex", "con", "int", "wis", "cha", "per", "wil" })
#define _LANG_ORB_ADJUST_OK "Ok, your attributes will be:\n"
#define _LANG_ORB_ADJUST_CONGRATS "Congrats! The attribute generation has been completed. " + \
    "Your character is now ready, you can start playing."
