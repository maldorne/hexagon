
// shop.c

#define _LANG_SHOP_VERBS_LIST ({ "list" })
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

#define _LANG_SHOP_LIST_SUMMARY "We have " + query_num(sizeof(all), 20) + " item"+(sizeof(all)==1?"":"s")+" available:\n\n"
#define _LANG_SHOP_LIST_JUST_ONE "Just one " + name_list[i] 
#define _LANG_SHOP_LIST_LOTS "A great selection of " + (string)list[name_list[i]][1]->query_plural()
#define _LANG_SHOP_LIST_EMPTY "The store is empty right now.\n"
