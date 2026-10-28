// Strings for the NPC components under this directory.

// raise.c -- the service of returning a ghost to its body

#define _LANG_RAISE_INFO_VERBS ({ "ask" })
#define _LANG_RAISE_VERBS ({ "raise", "raiseme" })

#define _LANG_RAISE_INFO "Uhmm, if you truly wish to return to your mortal form, " + \
    "you have only to ask: write 'raise'."
#define _LANG_RAISE_NOT_DEAD "Why do you want me to raise you if you don't need it?\n"
#define _LANG_RAISE_REFUSED "Your soul is not pure enough for me to bring you back to life.\n"
#define _LANG_RAISE_TOO_SOON "You were raised recently, you will have to wait a bit more.\n"
#define _LANG_RAISE_GESTURE "Hands are raised calling on the powers that bind " + \
    "soul to body.\n"
// price_text is the price in words, set by raise.c
#define _LANG_RAISE_INFO_PRICE "My services cost " + price_text + "."
#define _LANG_RAISE_CANNOT_PAY "Bringing you back costs " + price_text + \
    ", and I see no such sum here, neither on you nor on your body.\n"

// heal-service.c -- the service of healing whoever asks, for a price
// price_text is a price in words and who the player, set by heal-service.c

#define _LANG_HEAL_SERVICE_INFO_VERBS ({ "ask" })
#define _LANG_HEAL_SERVICE_VERBS ({ "heal", "healme" })

#define _LANG_HEAL_SERVICE_INFO "I can tend your wounds: write 'heal' and I will " + \
    "see to them."
#define _LANG_HEAL_SERVICE_INFO_PRICE "I charge " + price_text + " to tend you."
#define _LANG_HEAL_SERVICE_DEAD "I tend the living, not ghosts. Get yourself raised first.\n"
#define _LANG_HEAL_SERVICE_FIGHTING "Not while you are fighting! Finish that first.\n"
#define _LANG_HEAL_SERVICE_REFUSED "I cannot help you.\n"
#define _LANG_HEAL_SERVICE_TOO_SOON "I treated you a moment ago, give your body time.\n"
#define _LANG_HEAL_SERVICE_NOT_NEEDED "You look fine to me, I have nothing to heal.\n"
#define _LANG_HEAL_SERVICE_CANNOT_PAY "That will be " + price_text + ", and you do not " + \
    "have it. Come back when you do.\n"
#define _LANG_HEAL_SERVICE_BUSY "I cannot tend you right now, wait a moment.\n"
#define _LANG_HEAL_SERVICE_START "Keep still, I am starting."

// toll
#define _LANG_TOLL_VERBS ({ "pay" })
#define _LANG_TOLL_BLOCKED "Hey, " + who->query_cap_name() + ", if you want to cross, pay!"
#define _LANG_TOLL_EXEMPT "You do not have to pay anything, go through whenever you like."
#define _LANG_TOLL_ALREADY_PAID "Thank you, but you have already paid."
#define _LANG_TOLL_NO_MONEY "Hey, you! Are you trying to make a fool of me? You do not have enough money."
#define _LANG_TOLL_PAID "Thank you very much, the way is open for you."

// guard
#define _LANG_GUARD_WANTED_BLOCKED "Halt! You are wanted by the law, you will not pass."
#define _LANG_GUARD_WANTED_SHOUT "Halt, in the name of the law!"
#define _LANG_GUARD_RACE_BLOCKED "We do not let your people through."
#define _LANG_GUARD_NIGHT_BLOCKED "At night the gates do not open to strangers."

// gift
#define _LANG_GIFT_BUSY "$npc$ is in the middle of a fight and cannot attend to you right now."

// verb-response
#define _LANG_VERB_RESPONSE_BUSY "$npc$ is in the middle of a fight and cannot attend to you right now."
