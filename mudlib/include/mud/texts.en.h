// this file will be included from config.h

// most of the translations in the game will be included at compile time
// using the GLOBAL_COMPILE_LANG definition, that will force texts to be
// included from the include_file function in the driver object

// in the other hand, the driver and auto objects does not execute
// the include_file call, so some of the texts have to be included here

// this file will ONLY have the texts used in the driver and auto objects

// texts.en

#define _LANG_ERROR_HAPPENED "An error happened.\n"
#define _LANG_DIDNT_WORK "The attempt of doing that didn't work.\n"

#define _LANG_AND "and"

string ordinal(int number)
{
  string retval;

  switch(number)
  {
    case 11:
      retval = "11th"; break;
    case 12:
      retval = "12th"; break;
    case 13:
      retval = "13th"; break;
    default:
    switch(number%10)
    {
      case 1:
      retval = number+"st"; break;
      case 2:
      retval = number+"nd"; break;
      case 3:
      retval = number+"rd"; break;
      default:
      retval = number+"th";
    }
  }
  return retval;
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

#define DIR_DOWN      "down"
#define DIR_UP        "up"
#define DIR_IN        "inside"
#define DIR_OUT       "outside"
#define DIR_NORTH     "north"
#define DIR_SOUTH     "south"
#define DIR_EAST      "east"
#define DIR_WEST      "west"
#define DIR_SOUTHWEST "southwest"
#define DIR_SOUTHEAST "southeast"
#define DIR_NORTHEAST "northeast"
#define DIR_NORTHWEST "northwest"
#define DIR_COMMON    "common"
#define DIR_STAIRS    "stairs"

#define DIR_DOWN_SHORTEN      "down"
#define DIR_UP_SHORTEN        "up"
#define DIR_IN_SHORTEN        "in"
#define DIR_OUT_SHORTEN       "out"
#define DIR_NORTH_SHORTEN     "n"
#define DIR_SOUTH_SHORTEN     "s"
#define DIR_EAST_SHORTEN      "e"
#define DIR_WEST_SHORTEN      "w"
#define DIR_SOUTHWEST_SHORTEN "sw"
#define DIR_SOUTHEAST_SHORTEN "se"
#define DIR_NORTHEAST_SHORTEN "ne"
#define DIR_NORTHWEST_SHORTEN "nw"
#define DIR_STAIRS_SHORTEN    "stairs"

