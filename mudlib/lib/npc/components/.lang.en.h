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
#define _LANG_RAISE_PAY_VERBS ({ "pay" })
#define _LANG_RAISE_BUSY "I cannot call on the powers right now, try again in a moment.\n"
// price_text is the price in words, set by raise.c
#define _LANG_RAISE_INFO_CREDIT "I will bring you back, and what it costs, " + price_text + \
    ", you will owe me: pay when you can with 'pay'."
#define _LANG_RAISE_DEBT_TOLD "You will return to your mortal form, but you will owe me " + price_text + \
    ". Do not forget your debt: write 'pay' when you have money."
#define _LANG_RAISE_DEBT_REMINDER "You still owe me " + price_text + ". Write 'pay' to settle it."
#define _LANG_RAISE_DEBT_REFUSED "I will not bring you back while you owe me " + price_text + \
    ". Find somebody else to bring you back, and come back to pay me.\n"
#define _LANG_RAISE_NO_DEBT "You owe me nothing.\n"
#define _LANG_RAISE_DEBT_NO_MONEY "You carry no money to pay what you owe me: " + price_text + ".\n"
#define _LANG_RAISE_DEBT_SETTLED "Your debt is settled. If you need me again, I will be here."
// price_text is what was paid, left_text what is still owed
#define _LANG_RAISE_DEBT_PART_PAID "I receive " + price_text + ". You still owe me " + left_text + "."

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

// gatekeeper: the commands that close a door and lock it with the carried key
#define _LANG_GATEKEEPER_LOCK_COMMANDS(close, dir) ({ close + " " + dir, "lock " + dir })

// gift
#define _LANG_GIFT_BUSY "$npc$ is in the middle of a fight and cannot attend to you right now."

// verb-response
#define _LANG_VERB_RESPONSE_BUSY "$npc$ is in the middle of a fight and cannot attend to you right now."

// receive
#define _LANG_RECEIVE_PREP "to"
#define _LANG_RECEIVE_HAND_OVER_ME "You hand $item$ over to $npc$."
#define _LANG_RECEIVE_HAND_OVER_ROOM "$player$ hands $item$ over to $npc$."
#define _LANG_RECEIVE_NOT_HAVE "You have nothing like that to hand over.\n"
#define _LANG_RECEIVE_CANNOT "You cannot hand that over right now.\n"
#define _LANG_RECEIVE_SPENT "$npc$ needs nothing more from you."

// escort.c -- a guard taking a beaten criminal to prison

#define _LANG_ESCORT_ARRESTS "$guard$ stands over $who$, puts shackles on the prisoner and declares: 'In the name of the law, you are under arrest!'\n"
#define _LANG_ESCORT_ARRESTS_AWAY "A guard has reached $who$ and made the arrest.\n"
#define _LANG_ESCORT_DRAG_FROM "$guard$ drags $who$ away.\n"
#define _LANG_ESCORT_DRAG_TO "$guard$ arrives dragging $who$, unconscious.\n"

// trainer.c -- the service of teaching skills for money
// skill_name, points, price_text, now, to and name/row/price (list) are set by trainer.c

#define _LANG_TRAINER_LIST_VERBS ({ "list" })
#define _LANG_TRAINER_COST_VERBS ({ "cost" })
#define _LANG_TRAINER_TRAIN_VERBS ({ "train" })
#define _LANG_TRAINER_UP_TO "to"

#define _LANG_TRAINER_BUSY "I cannot see to you now, I am busy.\n"
#define _LANG_TRAINER_REFUSED "I have nothing to teach you, not yet.\n"
#define _LANG_TRAINER_WHICH "What do you want to learn? Type 'list' to see what I teach."
#define _LANG_TRAINER_NOT_TAUGHT "I do not teach that. Type 'list' to see what I know."
#define _LANG_TRAINER_UNKNOWN_SKILL "You do not know that skill, I cannot improve it for you."
#define _LANG_TRAINER_TOO_LOW "You do not know enough of that yet for me to help you: practise it first."
#define _LANG_TRAINER_NOTHING_MORE "You already know of that everything I can teach you."
#define _LANG_TRAINER_COST "Improving " + skill_name + " by " + points + " point" + (points == 1 ? "" : "s") + \
    " up to " + ask[2] + "% will cost you " + price_text + "."
#define _LANG_TRAINER_CANNOT_PAY "That would be " + price_text + ", and you do not carry so much. Come back when you do."
#define _LANG_TRAINER_DONE "So be it."
#define _LANG_TRAINER_TRAINED "You train " + skill_name + " up to " + ask[2] + "% for " + price_text + ".\n"
#define _LANG_TRAINER_LIST_HEADER "This is what I teach:\n\n"
#define _LANG_TRAINER_LIST_LINE "  " + capitalize(name) + ": from " + \
    (mappingp(row) && intp(row["min"]) ? row["min"] : 0) + "% to " + \
    (mappingp(row) && intp(row["max"]) ? row["max"] : 100) + "%, " + \
    (price > 0 ? price_words(price) + " each point" : "free") + ".\n"

// weapon masteries and levels (rows with "mastery"; min, max, now, value, left_text, cooldown_text
// and status are set by trainer.c)
#define _LANG_TRAINER_LEVEL "You do not have enough experience yet: you must be at least level " + \
    trainer_gate["min_level"] + " for me to teach you anything.\n"
#define _LANG_TRAINER_MASTERY_ILLEGAL "Your guild does not allow you to wield any of those weapons, and I will not teach you what you could not use."
#define _LANG_TRAINER_MASTERY_NEVER "You have to fight with that weapon first: I cannot teach you to handle what you have never wielded."
#define _LANG_TRAINER_MASTERY_TOO_LOW "I only teach what fighting can no longer give you, from " + min + "% in " + \
    skill_name + ". You are at " + now + "%: keep fighting with those weapons."
#define _LANG_TRAINER_MASTERY_FULL "With " + skill_name + " you have reached " + now + "%, and " + max + "% is the most I can teach."
#define _LANG_TRAINER_COOLDOWN "You learned from me not long ago. Come back in " + left_text + "."
#define _LANG_TRAINER_MASTERY_COST "One point of " + skill_name + " (from " + ask[1] + "% to " + ask[2] + "%) will cost you " + \
    price_text + ". I teach one point at a time" + (trainer_cooldown > 0 ? ", and I will not give you another until " + \
    cooldown_text + " have passed" : "") + "."
#define _LANG_TRAINER_MASTERY_TRAINED "You train " + skill_name + ": you now master " + value + "% for " + price_text + \
    ". I cannot give you another point for " + cooldown_text + ".\n"
#define _LANG_TRAINER_MASTERY_TRAINED_NOW "You train " + skill_name + ": you now master " + value + "% for " + price_text + ".\n"
#define _LANG_TRAINER_VALUE_NONE "nothing, you have never used it"
#define _LANG_TRAINER_MASTERY_LINE "  " + capitalize(name) + ": you master " + value_text + "; I teach from " + min + "% to " + max + \
    "%, " + (price > 0 ? price_words(price) + " each point" : "free") + ", one point a session. " + status + "\n"
#define _LANG_TRAINER_STATUS_ILLEGAL "Your guild does not allow any of those weapons."
#define _LANG_TRAINER_STATUS_NEVER "You have to fight with that weapon first."
#define _LANG_TRAINER_STATUS_TOO_LOW "You are not at " + min + "% yet: keep fighting."
#define _LANG_TRAINER_STATUS_FULL "I have nothing more to teach you."
#define _LANG_TRAINER_STATUS_WAIT "You can train another point in " + left_text + "."
#define _LANG_TRAINER_STATUS_READY "You can train a point now."
