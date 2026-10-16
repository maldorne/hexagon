// Strings for the temple room and the temple location component, which share
// temple-actions.c. $god$ is a deity's name, $noun$ god or goddess.

#define _LANG_TEMPLE_CMDS_WORSHIP ({ "worship" })
#define _LANG_TEMPLE_CMDS_OFFER ({ "offer" })
#define _LANG_TEMPLE_YES ({ "y", "yes" })
#define _LANG_TEMPLE_COIN_WORDS ({ "coins", "coin" })

#define _LANG_TEMPLE_NO_GOD "This place is consecrated to no god.\n"
#define _LANG_TEMPLE_CLOSED_NOW "Nobody officiates in this temple any more: those " + \
      "who rule these lands do not allow the worship of $god$.\n"
#define _LANG_TEMPLE_ALREADY_YOURS "You already belong to $god$, and forever.\n"
#define _LANG_TEMPLE_OTHER_GOD "Your $noun$, $god$, would be furious if you honoured " + \
      "another. You belong to them forever.\n"
#define _LANG_TEMPLE_NOT_FOLLOWER "Only the faithful of $god$ may make offerings here.\n"
#define _LANG_TEMPLE_WORSHIP_CONFIRM "Worshipping $god$ is forever: you will never be able " + \
      "to change your $noun$. Are you sure? (y/n) "
#define _LANG_TEMPLE_WORSHIP_CANCELLED "You think better of it and stand up.\n"
#define _LANG_TEMPLE_WORSHIP_FAILED "$god$ does not take you among the faithful.\n"
#define _LANG_TEMPLE_WORSHIP_DONE "You kneel before the altar and give your life to $god$. " + \
      "From today you belong to them, and may pray to them where they are worshipped.\n"
#define _LANG_TEMPLE_WORSHIP_ROOM "$who$ kneels before the altar and gives their life to $god$.\n"

#define _LANG_TEMPLE_OFFER_USAGE "Syntax: offer <item> | offer <amount> coins\n"
#define _LANG_TEMPLE_OFFER_NOTHING "You carry nothing like that.\n"
#define _LANG_TEMPLE_OFFER_IN_USE "You will have to stop using it before offering it.\n"
#define _LANG_TEMPLE_OFFER_REFUSED "$god$ does not want that. You keep it.\n"
#define _LANG_TEMPLE_OFFER_NO_COINS "$god$ does not want your coins.\n"
#define _LANG_TEMPLE_OFFER_NO_MONEY "You do not carry that much money.\n"
#define _LANG_TEMPLE_OFFER_COINS_ROOM "$who$ leaves some coins on the altar, and they vanish.\n"
#define _LANG_TEMPLE_OFFER_ITEM_ROOM "$who$ leaves $item$ on the altar, and it vanishes.\n"

#define _LANG_TEMPLE_SIGN_LONG "It is a marble plaque beside the entrance.\n"
#define _LANG_TEMPLE_ROOM_SHORT "Temple"
#define _LANG_TEMPLE_ROOM_LONG "A hall of high ceilings and dim light, with an altar at the far end. " + \
      "A god is worshipped here and given offerings.\n"
