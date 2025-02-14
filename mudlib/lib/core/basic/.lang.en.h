
// gender

#define _LANG_GENDER_MALE "male"
#define _LANG_GENDER_FEMALE "female"

#define _LANG_GENDER_TITLE_MALE "mr."
#define _LANG_GENDER_TITLE_FEMALE "mrs."

#define _LANG_GENDER_OBJECTIVE_MALE "him"
#define _LANG_GENDER_OBJECTIVE_FEMALE "her"
#define _LANG_GENDER_OBJECTIVE_NEUTRAL "its"

#define _LANG_GENDER_POSSESSIVE_FEMALE "her"
#define _LANG_GENDER_POSSESSIVE_MALE "his"
#define _LANG_GENDER_POSSESSIVE_NEUTRAL "its"
#define _LANG_GENDER_POSSESSIVE_SECOND "your"
#define _LANG_GENDER_POSSESSIVE_SECOND_PLURAL "your"

#define _LANG_GENDER_PRONOUN_MALE "he"
#define _LANG_GENDER_PRONOUN_FEMALE "she"
#define _LANG_GENDER_PRONOUN_NEUTRAL "it"

#define _LANG_GENDER_ARTICLE_FEMALE "the"
#define _LANG_GENDER_ARTICLE_MALE "the"
#define _LANG_GENDER_ARTICLE_PLURAL_FEMALE "the"
#define _LANG_GENDER_ARTICLE_PLURAL_MALE "the"
#define _LANG_GENDER_NUMERAL_FEMALE ((!undefinedp(next_word) && \
                                member_array(lower_case(next_word)[0..0], ({ "a", "e", "i", "o", "u" })) != -1) ? \
                                "an" : "a")
#define _LANG_GENDER_NUMERAL_MALE ((!undefinedp(next_word) && \
                                member_array(lower_case(next_word)[0..0], ({ "a", "e", "i", "o", "u" })) != -1) ? \
                                "an" : "a")
#define _LANG_GENDER_NUMERAL_PLURAL_FEMALE "some"
#define _LANG_GENDER_NUMERAL_PLURAL_MALE "some"
#define _LANG_GENDER_VOWEL_FEMALE ""
#define _LANG_GENDER_VOWEL_MALE ""
#define _LANG_GENDER_DEMONSTRATIVE_FEMALE "she"
#define _LANG_GENDER_DEMONSTRATIVE_MALE "he"

// read_desc.c

#define _LANG_READ_CMDS ({ "read" })
#define _LANG_READ_READ_WHAT "Read what?\n"
#define _LANG_READ_SOMETHING_WRITTEN "Looks that there's something written on "+this_object()->query_pronoun()+".\n"

// auto_load.c

#define _LANG_AUTO_LOAD_SOMETHING_WRONG "Ooops something broke.\n"
#define _LANG_AUTO_LOAD_DROP_SOMETHING "Oops, you dropped something.\n"

// contents.c

#define _LANG_CONTENTS_ARE_HERE "are here.\n"
#define _LANG_CONTENTS_IS_HERE "is here.\n"

// desc.c

#define _LANG_NO_LONG "Nothing particularly interesting.\n"

// events.c

#define _LANG_EVENTS_ENTERS " enters "

// condition.c

#define _LANG_CONDITION_WEAP "weapon"
#define _LANG_CONDITION_ARMO "armour"
#define _LANG_CONDITION_SHIE "shield"
#define _LANG_CONDITION_BROKE_MSG_ME "Your %^BOLD%^"+str+"%^RESET%^ breaks due to use.\n"
#define _LANG_CONDITION_BROKE_MSG_ROOM "" + environment(this_object())->query_cap_name() + \
                    "'s %^BOLD%^" + str + "%^RESET%^ breaks into a thousand pieces.\n"
#define _LANG_CONDITION_STATUS_0 "It is in a terrible state.\n"
#define _LANG_CONDITION_STATUS_10 "It is in very bad condition.\n"
#define _LANG_CONDITION_STATUS_20 "It is in bad condition.\n"
#define _LANG_CONDITION_STATUS_30 "It is in a poor condition.\n"
#define _LANG_CONDITION_STATUS_40 "It is pretty damaged.\n"
#define _LANG_CONDITION_STATUS_50 "It is damaged.\n"
#define _LANG_CONDITION_STATUS_60 "It is a bit damaged.\n"
#define _LANG_CONDITION_STATUS_70 "It is in a reasonable condition.\n"
#define _LANG_CONDITION_STATUS_80 "It is in good condition.\n"
#define _LANG_CONDITION_STATUS_90 "It is in very good condition.\n"
#define _LANG_CONDITION_STATUS_DEF "It is in excellent condition.\n"

// container.c

#define _LANG_CONTAINER_CONTAINS " contains:\n"
#define _LANG_CONTAINER_DROP "You can't hold everything you're trying to carry, " + \
            "so you drop your " + stuff[i]->query_name() + ".\n"
