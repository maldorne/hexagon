
// attendable.c

#define _LANG_ATT_NON_ATTENDABLE "For various reasons they cannot assist you at this time.\n"

// shop.c

#define _LANG_SHOP_SIGN_DESC "A small wooden sign.\n"
#define _LANG_SHOP_SIGN_INFO "" + \
    "Shop information:\n" + \
    "  - 'list' to see items on sale.\n" + \
    "  - 'browse <item>' to see item information (price, condition, etc).\n" + \
    "  - 'buy <item>' to buy something.\n"
#define _LANG_SHOP_SIGN_INFO_SELL "" + \
    "  - 'value <item>' to know how much will be paid for something you own.\n" + \
    "  - 'sell <item>' to sell something you own.   \n"
#define _LANG_SHOP_STORE_IN_USE "The storeroom is being used right now, try again in a minute.\n"
#define _LANG_SHOP_STORE_FULL "The storeroom is full, nothing more can be stored.\n"
#define _LANG_SHOP_DEFAULT_SELL_MSGS ({ "You sell $ob$ for $money$.\n", "$client$ sells $ob$.\n" })
#define _LANG_SHOP_DEFAULT_BUY_MSGS ({ "You buy $ob$ for $money$.\n", "$client$ buys $ob$.\n" })
#define _LANG_SHOP_DEFAULT_VALUE_MSG "$ob$ is valued in $money$.\n"
#define _LANG_SHOP_DEFAULT_BROWSE_MSG "$ob$ costs $money$, description:\n$extra$"
#define _LANG_SHOP_NOTHING_TO_VERB "Nothing to " + query_verb() + ".\n"
#define _LANG_SHOP_NOT_FOUND "" + capitalize(str) + " not found.\n"

#define _LANG_SHOP_VERBS_LIST ({ "list" })
#define _LANG_SHOP_LIST_SUMMARY "We have " + query_num(sizeof(all), 20) + " item"+(sizeof(all)==1?"":"s")+" available:\n\n"
#define _LANG_SHOP_LIST_JUST_ONE "Just one " + name_list[i] 
#define _LANG_SHOP_LIST_LOTS "A great selection of " + (string)list[name_list[i]][1]->query_plural()
#define _LANG_SHOP_LIST_EMPTY "The store is empty right now.\n"

#define _LANG_SHOP_VERBS_SELL ({ "sell" })
#define _LANG_SHOP_ONLY_SELL "This shop does not buy items.\n"
#define _LANG_SHOP_SELL_SYNTAX "Syntax: sell <items>\n"
#define _LANG_SHOP_NOT_THIS_TYPE "This shop does not buy this kind of items.\n"
#define _LANG_SHOP_SELL_NOT_MORE "It seems they already have enough " + obs[0]->query_plural() + ".\n"
#define _LANG_SHOP_SELL_TOO_MANY "You cannot sell all of that at the same time.\n"
#define _LANG_SHOP_SELL_NOT_THIS_TYPE "You cannot sell your "+obs[i]->short()+", these " + \
          "kind of items cannot be sold.\n"
#define _LANG_SHOP_SELL_CANNOT_SELL "You cannot sell "+query_multiple_short(cannot)+", maybe you are using it, " + \
        "or it's something not accepted here.\n"
#define _LANG_SHOP_SELL_NOTHING "Nothing to sell.\n"

#define _LANG_SHOP_VERBS_BROWSE ({ "browse" })
#define _LANG_SHOP_BROWSE_SYNTAX "Syntax: browse <items>\n"

#define _LANG_SHOP_VERBS_VALUE ({ "value" })
#define _LANG_SHOP_VALUE_SYNTAX "Syntax: value <items>\n"

#define _LANG_SHOP_VERBS_BUY ({ "buy" })
#define _LANG_SHOP_BUY_SYNTAX "Syntax: buy <items>\n"
#define _LANG_SHOP_BUY_TOO_MANY "You cannot buy all of that at the same time.\n"
#define _LANG_SHOP_BUY_CANNOT_MOVE "You cannot get " + query_multiple_short(cannot) + ".\n"
#define _LANG_SHOP_BUY_TOO_EXPENSIVE capitalize(query_multiple_short(too_much)) + " is too expensive.\n"
#define _LANG_SHOP_BUY_NOTHING "Nothing to buy.\n"

#define _LANG_SHOP_VERBS_LOGS ({ "logs" })
#define _LANG_SHOP_LOGS_SYNTAX "Syntax: logs\n"
#define _LANG_SHOP_LOGS_EMPTY "The activity log is empty.\n"
#define _LANG_SHOP_LOGS_NO_PERMISSION "You do not have permission to see the activity log.\n"

// pub.c

#define _LANG_PUB_SIGN_NAME "menu"
#define _LANG_PUB_SIGN_ALIASES ({ "menu" })
#define _LANG_PUB_SIGN_DESC "A small piece of paper.\n"
#define _LANG_PUB_BUY_CMDS ({ "buy" })
#define _LANG_PUB_MENU_FOODS "Food"
#define _LANG_PUB_MENU_DRINKS "Soft drinks"
#define _LANG_PUB_MENU_ALCOHOL "Alcoholic beverages"
#define _LANG_PUB_MENU_INSTRUCTIONS "Use 'buy' to ask for what you want.\n"
#define _LANG_PUB_BUY_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_PUB_BUY_SYNTAX query_verb() + " <something> [for <player>]"
#define _LANG_PUB_BUY_PREPOSITION "for"
#define _LANG_BUY_FOR_NOT_HERE "That person does not seem to be here.\n"
#define _LANG_PUB_BUY_FOR_REJECT_MSG_ME forwho->query_cap_name() + " rejects your offer.\n"
#define _LANG_PUB_BUY_FOR_REJECT_MSG_THEIR this_player()->query_cap_name() + " invites you, but you reject the offer.\n"
#define _LANG_PUB_BUY_FOR_REJECT_MSG_ENV this_player()->query_cap_name()+" invites " + forwho->query_cap_name() + ", but " + forwho->query_demonstrative() + " rejects the offer.\n"
#define _LANG_PUB_BUY_ITEM_NOT_FOUND "I'm sorry, but '"+str+"' is not available.\n"
#define _LANG_PUB_BUY_CANNOT_AFFORD "You cannot afford something so expensive.\n"
#define _LANG_PUB_BUY_FOR_MSG_ME "You buy " + str + " for " + forwho->query_cap_name() + ".\n"
#define _LANG_PUB_BUY_FOR_MSG_THEIR this_player()->query_cap_name() + " buys " + str + " for you.\n"
#define _LANG_PUB_BUY_FOR_MSG_ENV this_player()->query_cap_name() + " buys " + str " for " + forwho->query_cap_name() + ".\n"

#define _LANG_PUB_DEFAULT_DRINK_MESS_ME "You drink a glass of " + name
#define _LANG_PUB_DEFAULT_DRINK_MESS_OTHERS "drinks a glass of " + name
#define _LANG_PUB_DEFAULT_FOOD_MESS_ME "You eat a full plate of " + name
#define _LANG_PUB_DEFAULT_FOOD_MESS_OTHERS "eats a full plate of " + name
#define _LANG_PUB_DEFAULT_MENU_HEADER "On the menu there is:"
