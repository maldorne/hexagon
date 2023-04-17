
// clone

#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " looks for a " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "object") + \
        " in other dimension.\n"

// peace

#define _LANG_PEACE_MESSAGES_MINE "You raise your hand and exclaim: " + \
        "%^YELLOW%^Stop fighting, mortals!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " raises " + this_player()->query_possessive() + " hand and exclaims: " + \
        "%^YELLOW%^Stop fighting, mortals!%^RESET%^\n"
