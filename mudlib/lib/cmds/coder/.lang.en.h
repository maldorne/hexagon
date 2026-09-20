
// clone

#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " looks for a " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "object") + \
        " in other dimension.\n"

#define _LANG_CMD_CLONE_IN ({ "in" })

// heal

#define _LANG_HEAL_ALIASES ({ "heal" })
#define _LANG_HEAL_MESSAGE_MINE "%^YELLOW%^Your life and energy come back.%^RESET%^\n"
#define _LANG_HEAL_MESSAGE_OTHERS "%^YELLOW%^The life and energy of " + me->query_cap_name() + \
  " come back.%^RESET%^\n"

// peace

#define _LANG_PEACE_MESSAGES_MINE "You raise your hand and exclaim: " + \
        "%^YELLOW%^Stop fighting, mortals!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " raises " + this_player()->query_possessive() + " hand and exclaims: " + \
        "%^YELLOW%^Stop fighting, mortals!%^RESET%^\n"

// snoop

#define _LANG_SNOOP_WATCHED(who) (who) + " starts watching your session.\n"

// uptime

#define _LANG_CMD_UPTIME_MESSAGE "The mud has been up for " + \
  seconds_to_time_string(uptime()) + ".\n"

// free-title

#define _LANG_CMD_FREE_TITLE_ALIASES ({ "freetitle" })
#define _LANG_CMD_FREE_TITLE_SYNTAX "freetitle <text>\n" + \
                                    "          freetitle -c"
#define _LANG_CMD_FREE_TITLE_HELP "Writes by hand the title you wear, or clears it with -c."
#define _LANG_CMD_FREE_TITLE_SHOW "Your title written by hand is: " + me->query_manual_title() + "\n"
#define _LANG_CMD_FREE_TITLE_NONE "You have no title written by hand.\n"
#define _LANG_CMD_FREE_TITLE_CLEARED "Your title written by hand is gone.\n"
#define _LANG_CMD_FREE_TITLE_SET "You now wear the title '" + me->query_title() + "'.\n"
