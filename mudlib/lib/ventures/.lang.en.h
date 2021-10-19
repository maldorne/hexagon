
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
