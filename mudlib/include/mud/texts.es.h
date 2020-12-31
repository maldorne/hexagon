// this file will be included from config.h

// most of the translations in the game will be included at compile time
// using the GLOBAL_COMPILE_LANG definition, that will force texts to be
// included from the include_file function in the driver object

// in the other hand, the driver and auto objects does not execute
// the include_file call, so some of the texts have to be included here

// this file will ONLY have the texts used in the driver and auto objects

// texts.en

#define _LANG_ERROR_HAPPENED "Se ha producido un error.\n"
#define _LANG_DIDNT_WORK "El intento de hacer eso no funcionó.\n"

#define _LANG_AND "y"

string ordinal(int number)
{
  return "" + number + "º";
}

#define _LANG_NUMBERS_THOUSANDS_OF "miles de"
#define _LANG_NUMBERS_LOADS_OF "montones de"
#define _LANG_NUMBERS_THOUSAND "mil"
#define _LANG_NUMBERS_ONE_THOUSAND "mil"
#define _LANG_NUMBERS_ZERO "cero"
#define _LANG_NUMBERS_LOTS_OF "muchos"

#define _LANG_NUMBERS_10 "diez"
#define _LANG_NUMBERS_11 "once"
#define _LANG_NUMBERS_12 "doce"
#define _LANG_NUMBERS_13 "trece"
#define _LANG_NUMBERS_14 "catorce"
#define _LANG_NUMBERS_15 "quince"
#define _LANG_NUMBERS_16 "dieciséis"
#define _LANG_NUMBERS_17 "diecisiete"
#define _LANG_NUMBERS_18 "dieciocho"
#define _LANG_NUMBERS_19 "diecinueve"
#define _LANG_NUMBERS_20 "veinte"
#define _LANG_NUMBERS_21 "veintiún"
#define _LANG_NUMBERS_22 "veintidós"
#define _LANG_NUMBERS_23 "veintitrés"
#define _LANG_NUMBERS_24 "veinticuatro"
#define _LANG_NUMBERS_25 "veinticinco"
#define _LANG_NUMBERS_26 "veintiséis"
#define _LANG_NUMBERS_27 "veintisiete"
#define _LANG_NUMBERS_28 "veintiocho"
#define _LANG_NUMBERS_29 "veintinueve"

#define _LANG_NUMBERS_30 "treinta"
#define _LANG_NUMBERS_40 "cuarenta"
#define _LANG_NUMBERS_50 "cincuenta"
#define _LANG_NUMBERS_60 "sesenta"
#define _LANG_NUMBERS_70 "setenta"
#define _LANG_NUMBERS_80 "ochenta"
#define _LANG_NUMBERS_90 "noventa"

#define _LANG_NUMBERS_CONCAT " y "

#define _LANG_NUMBERS_1 "un"
#define _LANG_NUMBERS_2 "dos"
#define _LANG_NUMBERS_3 "tres"
#define _LANG_NUMBERS_4 "cuatro"
#define _LANG_NUMBERS_5 "cinco"
#define _LANG_NUMBERS_6 "seis"
#define _LANG_NUMBERS_7 "siete"
#define _LANG_NUMBERS_8 "ocho"
#define _LANG_NUMBERS_9 "nueve"

#define _LANG_NUMBERS_100 (!n) ? "cien" : "ciento"
#define _LANG_NUMBERS_200 "doscientos"
#define _LANG_NUMBERS_300 "trescientos"
#define _LANG_NUMBERS_400 "cuatrocientos"
#define _LANG_NUMBERS_500 "quinientos"
#define _LANG_NUMBERS_600 "seiscientos"
#define _LANG_NUMBERS_700 "setecientos"
#define _LANG_NUMBERS_800 "ochocientos"
#define _LANG_NUMBERS_900 "novecientos"

#define DIR_DOWN      "abajo"
#define DIR_UP        "arriba"
#define DIR_IN        "dentro"
#define DIR_OUT       "fuera"
#define DIR_NORTH     "norte"
#define DIR_SOUTH     "sur"
#define DIR_EAST      "este"
#define DIR_WEST      "oeste"
#define DIR_SOUTHWEST "sudoeste"
#define DIR_SOUTHEAST "sudeste"
#define DIR_NORTHEAST "noreste"
#define DIR_NORTHWEST "noroeste"
#define DIR_COMMON    "comun"

