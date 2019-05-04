// this file will be included from config.h

// most of the translations in the game will be included at compile time
// using the GLOBAL_COMPILE_LANG definition, that will force texts to be
// included from the include_file function in the driver object

// in the other hand, the driver and auto objects does not execute
// the include_file call, so some of the texts have to be included here

// this file will ONLY have the texts used in the driver and auto objects

// texts.en

#define _LANG_AND "y"

string ordinal(int number)
{
  return "" + number + "º";
}
