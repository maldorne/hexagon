// Strings for the location components under this directory.

// crossing.c -- a crossing that leads astray

#define _LANG_CROSSING_LOST "Disoriented, you look around and head "
#define _LANG_CROSSING_LOST_VERTICAL "Disoriented, you look around and head "

// temple.c -- a place consecrated to a deity

#define _LANG_TEMPLE_CLOSED "Nobody officiates here. The altar is under a " + \
    "layer of dust, and what offerings are left dried up long ago.\n"

// rope.c -- a rope tied to something, to climb or descend by

#define _LANG_ROPE_TIE_VERBS ({ "tie" })
#define _LANG_ROPE_UNTIE_VERBS ({ "untie" })
#define _LANG_ROPE_WORDS ({ "rope" })
#define _LANG_ROPE_ARTICLES ({ "the", "a", "that", "this" })
#define _LANG_ROPE_ARTICLE ""
#define _LANG_ROPE_ARTICLE_CONTRACTED ""
#define _LANG_ROPE_TO "to "
#define _LANG_ROPE_NO_ROPE "You carry no rope.\n"
#define _LANG_ROPE_ALREADY_TIED "A rope is already tied here.\n"
#define _LANG_ROPE_NOTHING_TIED "There is no rope tied here.\n"
#define _LANG_ROPE_TIE_SELF "You tie the rope firmly"
#define _LANG_ROPE_TIE_OTHERS " ties a rope firmly"
#define _LANG_ROPE_UNTIE_SELF "You untie the rope and pick it up.\n"
#define _LANG_ROPE_UNTIE_SELF_FLOOR "You untie the rope, which falls to the floor.\n"
#define _LANG_ROPE_UNTIE_OTHERS " unties the rope.\n"
#define _LANG_ROPE_LINE "A rope tied"
#define _LANG_ROPE_HANGS " hangs "
#define _LANG_ROPE_STRETCHES " stretches towards "

// swamp.c -- mud that holds back whoever leaves

#define _LANG_SWAMP_STUCK "Your feet sink into the mud and, however hard you try, you cannot move on. Walking here is a struggle.\n"

// acid.c -- a pool of acid that burns whoever stands in it

#define _LANG_ACID_ENTER "As you step in you wade into a pool of acid, which causes you terrible pain.\n"
#define _LANG_ACID_BURN "The acid starts to burn you, causing you great pain. You had better get to safety.\n"

// fire.c -- a burning location

#define _LANG_FIRE_ENTER "The flames wrap around you as you step in, burning you terribly.\n"
#define _LANG_FIRE_BURN "The fire keeps burning you, causing you great pain. You had better get to safety.\n"
#define _LANG_FIRE_OUT "The flames weaken until they die out completely.\n"
#define _LANG_FIRE_DOUSED "The flames weaken, but keep burning.\n"

// mount-shop.c -- a place that sells mounts
// The texts use the variables of the function that writes them: label (the
// short of the mount), price_text (its price in words), number (its place in
// the list), description (the first paragraph of its description), max (how
// many mounts a player may own) and buyer.

#define _LANG_MOUNT_SHOP_LIST_VERBS ({ "list" })
#define _LANG_MOUNT_SHOP_EXAMINE_VERBS ({ "examine", "browse" })
#define _LANG_MOUNT_SHOP_BUY_VERBS ({ "buy" })
#define _LANG_MOUNT_SHOP_ARTICLES ({ "the", "a", "an" })

#define _LANG_MOUNT_SHOP_SIGN_DESC "A wooden sign, with letters carved by hand.\n"
#define _LANG_MOUNT_SHOP_SIGN_TEXT "" + \
    "Mounts for sale:\n" + \
    "  - 'list' to see the mounts on sale and their prices.\n" + \
    "  - 'examine <mount>' to see one up close and what it costs.\n" + \
    "  - 'buy <mount>' to buy it: from then on it is yours.\n" + \
    "  - 'mounts' to see the mounts you own and where they are.\n"

#define _LANG_MOUNT_SHOP_EMPTY "Nothing is for sale here right now.\n"
#define _LANG_MOUNT_SHOP_LIST_HEADER "Mounts for sale:\n"
#define _LANG_MOUNT_SHOP_LIST_LINE "  " + number + ". " + label + ": " + price_text + ".\n"
#define _LANG_MOUNT_SHOP_EXAMINE label + "\n" + description + "\nIt costs " + price_text + ".\n"
#define _LANG_MOUNT_SHOP_EXAMINE_CANNOT_AFFORD "You cannot afford it.\n"
#define _LANG_MOUNT_SHOP_BUY_WHAT "Buy which mount?\n"
#define _LANG_MOUNT_SHOP_NOT_SOLD "That is not for sale here.\n"
#define _LANG_MOUNT_SHOP_BUY_LIMIT "You already own as many mounts as you can look after (" + max + ").\n"
#define _LANG_MOUNT_SHOP_BUY_CANNOT_AFFORD "You cannot afford it: " + label + " costs " + price_text + ".\n"
#define _LANG_MOUNT_SHOP_BUY_FAILED "The mount cannot be brought out right now.\n"
#define _LANG_MOUNT_SHOP_BUY_ME "You pay " + price_text + " and the mount is yours: " + label + ".\n"
#define _LANG_MOUNT_SHOP_BUY_ROOM buyer->query_cap_name() + " buys a mount: " + label + ".\n"
