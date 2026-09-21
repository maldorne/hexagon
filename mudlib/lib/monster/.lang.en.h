
// friends.c

#define _LANG_FRIENDS_PILE_IN this_object()->query_cap_name() + " joins the fight!\n"
#define _LANG_FRIENDS_PILE_IN_PROTECTING capitalize(this_object()->query_short()) + \
          " starts protecting " + ob->query_short() + ".\n"

// chatter.c

#define _LANG_SEEMS_CONFUSED "seems confused for a moment." 

// quest-giver

#define _LANG_GIVER_ASK_VERBS ({ "work", "jobs" })
#define _LANG_GIVER_ACCEPT_VERBS ({ "accept" })
#define _LANG_GIVER_HAND_IN_VERBS ({ "deliver" })

#define _LANG_GIVER_NOT_YOUR_KIND this_object()->query_cap_name() + " looks you up and down and turns " + \
      "their back on you without a word.\n"
#define _LANG_GIVER_NOTHING_TO_OFFER this_object()->query_cap_name() + " tells you: I have nothing for you " + \
      "right now.\n"
#define _LANG_GIVER_OFFER this_object()->query_cap_name() + " tells you about a job: %^BOLD%^" + \
      quest->query_title() + "%^RESET%^.\n" + quest->query_description() + \
      "Type 'accept' if you are interested.\n"
#define _LANG_GIVER_COME_BACK_DONE this_object()->query_cap_name() + " tells you: You have done what I asked. " + \
      "Type 'deliver' and we will talk.\n"
#define _LANG_GIVER_NOTHING_TO_ACCEPT this_object()->query_cap_name() + " has no work to give you.\n"
#define _LANG_GIVER_NOTHING_TO_HAND_IN this_object()->query_cap_name() + " expects nothing from you yet.\n"
