
// messages

#define _LANG_ERROR_HAPPENED "An error occurred.\n"
#define _LANG_DIDNT_WORK "The attempt of doing that didn't work.\n"

#define _LANG_AND "and"
#define _LANG_SYNTAX "Syntax"
#define _LANG_OF "of"

#define _LANG_YES "yes"
#define _LANG_NO "no"

#define _LANG_AFFIRMATIVE_OPTIONS ({ "y", "yes", "yeah", "yep", "yup", "sure", "ok", "okay" })
#define _LANG_NEGATIVE_OPTIONS ({ "n", "no", "not", "don't", "dont", "nope" })

// numbers

#define _LANG_ORDINAL_CONVERSION switch(number) { case 11: \
    case 12: \
    case 13: \
      return "" + number + "th"; \
    default: \
    switch (number%10) \
    { \
      case 1: \
      return "" + number + "st"; \
      case 2: \
      return "" + number + "nd"; \
      case 3: \
      return "" + number + "rd"; \
      default: \
      return "" + number + "th"; \
    } \
  }

#define _LANG_NUMBERS_THOUSANDS_OF "thousands of"
#define _LANG_NUMBERS_LOADS_OF "loads of"
#define _LANG_NUMBERS_THOUSAND "thousand"
#define _LANG_NUMBERS_ONE_THOUSAND number_as_string_aux(i) + " " + _LANG_NUMBERS_THOUSAND
#define _LANG_NUMBERS_ZERO "zero"
#define _LANG_NUMBERS_LOTS_OF "lots of"

#define _LANG_NUMBERS_10 "ten"
#define _LANG_NUMBERS_11 "eleven"
#define _LANG_NUMBERS_12 "twelve"
#define _LANG_NUMBERS_13 "thirteen"
#define _LANG_NUMBERS_14 "fourteen"
#define _LANG_NUMBERS_15 "fifteen"
#define _LANG_NUMBERS_16 "sixteen"
#define _LANG_NUMBERS_17 "seventeen"
#define _LANG_NUMBERS_18 "eighteen"
#define _LANG_NUMBERS_19 "nineteen"
#define _LANG_NUMBERS_20 "twenty"
#define _LANG_NUMBERS_21 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_1
#define _LANG_NUMBERS_22 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_2
#define _LANG_NUMBERS_23 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_3
#define _LANG_NUMBERS_24 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_4
#define _LANG_NUMBERS_25 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_5
#define _LANG_NUMBERS_26 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_6
#define _LANG_NUMBERS_27 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_7
#define _LANG_NUMBERS_28 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_8
#define _LANG_NUMBERS_29 _LANG_NUMBERS_20 + _LANG_NUMBERS_CONCAT + _LANG_NUMBERS_9

#define _LANG_NUMBERS_30 "thirty"
#define _LANG_NUMBERS_40 "fourty"
#define _LANG_NUMBERS_50 "fifty"
#define _LANG_NUMBERS_60 "sixty"
#define _LANG_NUMBERS_70 "seventy"
#define _LANG_NUMBERS_80 "eighty"
#define _LANG_NUMBERS_90 "ninety"

#define _LANG_NUMBERS_CONCAT " "

#define _LANG_NUMBERS_1 "one"
#define _LANG_NUMBERS_2 "two"
#define _LANG_NUMBERS_3 "three"
#define _LANG_NUMBERS_4 "four"
#define _LANG_NUMBERS_5 "five"
#define _LANG_NUMBERS_6 "six"
#define _LANG_NUMBERS_7 "seven"
#define _LANG_NUMBERS_8 "eight"
#define _LANG_NUMBERS_9 "nine"

#define _LANG_NUMBERS_100 "one hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_200 "two hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_300 "three hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_400 "four hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_500 "five hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_600 "six hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_700 "seven hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_800 "eight hundred" + ((!n) ? "" : " and")
#define _LANG_NUMBERS_900 "nine hundred" + ((!n) ? "" : " and")
