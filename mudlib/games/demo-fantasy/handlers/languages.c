
/*
 * The language handler.
 */

#include <basic/communicate.h>
#include <translations/language.h>

inherit "/lib/handlers/languages.c";

#define GAME_LANGUAGES game_root() + "obj/languages/"

void create()
{
  ::create();
  languages = 
  ([
    STD_LANG :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH + "common", 10 }),
    "goblin" :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "goblin", 10 }),
    "elf" :           ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "elf", 10 }),
    // "thieves' cant" : ({ L_SPOKEN, GAME_LANGUAGES + "cant", 100 }),
    // "grunt" :         ({ L_SPOKEN, GAME_LANGUAGES + "grunt", 100 }),
    // "wizard spells" : ({ L_WRITTEN|L_MAGIC|L_SIZED, GAME_LANGUAGES + "wizard_lang", 100 }),
    // "drow" :          ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "drow", 10 }),
    // "dwarf" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "dwarf",10}),
    // "duergar" :       ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "duergar",10}),
    // "halfling" :      ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "halfling",10}),
    // "gnome" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "gnome", 10 }),
    // "orc" :           ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "orc", 10 }),
    // "frog" :          ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "frog", 10 }),
    // "newt" :          ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "newt", 10 }),
    // "varniad" :       ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "varniad", 10 }),
    // "ilythiiri" :     ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "ilythiiri", 10 }),
    // "troll" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "troll", 10 }),
    // "guppy" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "guppy", 10 }),
    // "kender" :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "kender",10}),
    // "quack" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "quack", 10 }),
    // "werewolf" :      ({ L_SPOKEN|L_DISTANCE, GAME_LANGUAGES + "werewolf",10 }),
    // "barbarian" :     ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "barbarian", 10 }),
    // "lizard-man" :    ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, GAME_LANGUAGES + "lizard-man", 10 }),
    // "drizz" :         ({ L_SPOKEN, GAME_LANGUAGES + "drizz", 100 }),
    // "gobbledegook" :  ({ L_SPOKEN|L_DISTANCE, GAME_LANGUAGES + "gobbledegook", 10 }),
    // "arf" :           ({ L_SPOKEN|L_DISTANCE, GAME_LANGUAGES + "arf", 10 }),
    // "bork" :          ({ L_SPOKEN|L_DISTANCE, GAME_LANGUAGES + "bork", 10 }),
    // "giant" :         ({ L_SPOKEN|L_DISTANCE, GAME_LANGUAGES + "giant", 10 }),
    // "skaven" :        ({ L_SPOKEN|L_DISTANCE, GAME_LANGUAGES + "skaven", 10}),
  ]);
} 
