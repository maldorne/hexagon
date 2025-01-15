
// gender.c

#define _LANG_GENDER_MALE "varón"
#define _LANG_GENDER_FEMALE "mujer"

#define _LANG_GENDER_TITLE_MALE "señor"
#define _LANG_GENDER_TITLE_FEMALE "señora"

#define _LANG_GENDER_OBJECTIVE_MALE "él"
#define _LANG_GENDER_OBJECTIVE_FEMALE "ella"
#define _LANG_GENDER_OBJECTIVE_NEUTRAL "él"

#define _LANG_GENDER_POSSESSIVE_FEMALE "su"
#define _LANG_GENDER_POSSESSIVE_MALE "su"
#define _LANG_GENDER_POSSESSIVE_NEUTRAL "su"
#define _LANG_GENDER_POSSESSIVE_SECOND "tu"
#define _LANG_GENDER_POSSESSIVE_SECOND_PLURAL "tus"

#define _LANG_GENDER_PRONOUN_MALE "él"
#define _LANG_GENDER_PRONOUN_FEMALE "ella"
#define _LANG_GENDER_PRONOUN_NEUTRAL "él"

#define _LANG_GENDER_ARTICLE_FEMALE "la"
#define _LANG_GENDER_ARTICLE_MALE "el"
#define _LANG_GENDER_ARTICLE_PLURAL_FEMALE "las"
#define _LANG_GENDER_ARTICLE_PLURAL_MALE "los"
#define _LANG_GENDER_NUMERAL_FEMALE (!undefinedp(next_word) && next_word[0..0] == "a" ? "un" : "una")
#define _LANG_GENDER_NUMERAL_MALE "un"
#define _LANG_GENDER_NUMERAL_PLURAL_FEMALE "unas"
#define _LANG_GENDER_NUMERAL_PLURAL_MALE "unos"
#define _LANG_GENDER_VOWEL_FEMALE "a"
#define _LANG_GENDER_VOWEL_MALE "o"
#define _LANG_GENDER_DEMONSTRATIVE_FEMALE "esta"
#define _LANG_GENDER_DEMONSTRATIVE_MALE "este"

// read_desc.c

#define _LANG_READ_CMDS ({ "leer" })
#define _LANG_READ_READ_WHAT "¿Leer el qué?\n"
#define _LANG_READ_SOMETHING_WRITTEN "Parece que tiene algo escrito.\n"

// auto_load.c

#define _LANG_AUTO_LOAD_SOMETHING_WRONG "¡Ups, algo se ha roto!\n"
#define _LANG_AUTO_LOAD_DROP_SOMETHING "Ups, se te ha caído algo.\n"

// contents.c

#define _LANG_CONTENTS_ARE_HERE "están aquí.\n"
#define _LANG_CONTENTS_IS_HERE "está aquí.\n"

// desc.c

#define _LANG_NO_LONG "No ves nada particularmente interesante.\n"

// events.c

#define _LANG_EVENTS_ENTERS " entra en "

// condition.c

#define _LANG_CONDITION_WEAP "arma"
#define _LANG_CONDITION_ARMO "armadura"
#define _LANG_CONDITION_SHIE "escudo"
#define _LANG_CONDITION_BROKE_MSG_ME "Tu %^BOLD%^"+str+"%^RESET%^ se acaba rompiendo a causa del uso.\n"
#define _LANG_CONDITION_BROKE_MSG_ROOM "" + capitalize(this_object()->query_article()) + \
            "%^BOLD%^" + str + "%^RESET%^ de " + environment(this_object())->query_cap_name() + " se rompe en mil pedazos.\n"
#define _LANG_CONDITION_STATUS_0 "Está destrozad"+aux+".\n"
#define _LANG_CONDITION_STATUS_10 "Está en muy malas condiciones.\n"
#define _LANG_CONDITION_STATUS_20 "Está en malas condiciones.\n"
#define _LANG_CONDITION_STATUS_30 "Está en un estado más bien pobre.\n"
#define _LANG_CONDITION_STATUS_40 "Está bastante estropead"+aux+".\n"
#define _LANG_CONDITION_STATUS_50 "Está estropead"+aux+".\n"
#define _LANG_CONDITION_STATUS_60 "Está un poco estropead"+aux+".\n"
#define _LANG_CONDITION_STATUS_70 "Está en un estado razonable.\n"
#define _LANG_CONDITION_STATUS_80 "Está en buenas condiciones.\n"
#define _LANG_CONDITION_STATUS_90 "Está en muy buenas condiciones.\n"
#define _LANG_CONDITION_STATUS_DEF "Está en excelente estado.\n"

// container.c

#define _LANG_CONTAINER_CONTAINS " contiene:\n"
#define _LANG_CONTAINER_DROP "No puedes sostener todo lo que intentas llevar, " + \
            "así que dejas caer tu " + stuff[i]->query_name() + ".\n"
