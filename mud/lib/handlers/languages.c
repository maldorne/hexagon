
/*
 * The language handler.  This will keep track of the info on languages
 * Each language can be a written language a spoken language and have
 * a garble object.
 *
 * The function garble_say, garble_tell, garble_shout and
 * garble_written shoud be defined on the garbling object.
 * They should return the entire string to print.  So if the
 * routine returns 0 nothing is printed.
 */

#include <basic/communicate.h>


mapping languages;

void create() {
  languages = 
  ([
    "comun" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"common", 10 }),
    "thieves' cant" : ({ L_SPOKEN, LANGUAGES_PATH+"cant", 100 }),
    "grunt" :         ({ L_SPOKEN, LANGUAGES_PATH+"grunt", 100 }),
    "wizard spells" : ({ L_WRITTEN|L_MAGIC|L_SIZED, LANGUAGES_PATH+"wizard_lang", 100 }),
    "elf" :           ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"elf", 10 }),
    "drow" :          ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"drow", 10 }),
    "dwarf" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"dwarf",10}),
    "duergar" :       ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"duergar",10}),
    "halfling" :      ({ L_SPOKEN|L_WRITTEN|L_DISTANCE,LANGUAGES_PATH+"halfling",10}),
    "gnome" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"gnome", 10 }),
    "goblin" :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"goblin", 10 }),
    "orc" :           ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"orc", 10 }),
    "frog" :          ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"frog", 10 }),
    "newt" :          ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"newt", 10 }),
    "varniad" :       ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"varniad", 10 }),
    "ilythiiri" :     ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"ilythiiri", 10 }),
    "troll" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"troll", 10 }),
    "guppy" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"guppy", 10 }),
    "kender" :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"kender",10}),
    "quack" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"quack", 10 }),
    "werewolf" :      ({ L_SPOKEN|L_DISTANCE,LANGUAGES_PATH+"werewolf",10 }),
    "barbarian" :     ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"barbarian", 10 }),
    "lizard-man" :    ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH+"lizard-man", 10 }),
    "drizz" :         ({ L_SPOKEN, LANGUAGES_PATH+"drizz", 100 }),
    "gobbledegook" :  ({ L_SPOKEN|L_DISTANCE, LANGUAGES_PATH+"gobbledegook", 10 }),
    "arf" :           ({ L_SPOKEN|L_DISTANCE, LANGUAGES_PATH+"arf", 10 }),
    "bork" :          ({ L_SPOKEN|L_DISTANCE, LANGUAGES_PATH+"bork", 10 }),
    "giant" :         ({ L_SPOKEN|L_DISTANCE, LANGUAGES_PATH+"giant", 10 }),
    "skaven" :        ({ L_SPOKEN|L_DISTANCE, LANGUAGES_PATH+"skaven", 10}),
  ]);
} /* create() */

int query_language_spoken(string str) {
  if (!languages[str])
    return 0;
  return languages[str][0]&L_SPOKEN;
} /* query_language_spoken() */

int query_language_written(string str) {
  if (!languages[str])
    return 0;
  return languages[str][0]&L_WRITTEN;
} /* query_language_written() */

/* Distance languages can be used with tell and shout */
int query_language_distance(string str) {
  if (!languages[str])
    return 0;
  return languages[str][0]&L_DISTANCE;
} /* query_language_distance() */

/*
 * A magical langage gets a function called on it's object whenever it
 * is read even if the person knows the language.  It is also the language
 * spells are written in.
 *
 */
int query_language_magic(string str) {
  if (!languages[str])
    return 0;
  return languages[str][0]&L_MAGIC;
} /* query_language_magic() */

int query_language_size(string str) {
  if (!languages[str])
    return 0;
  return languages[str][2];
} /* query_language_size() */

/* This is called to resize the text if it doesnt fit... */
mixed squidge_text(string lang, mixed str, int size) {
  if (!languages[lang])
    return 0; /* Don't add it... */
  if (!(languages[lang][0]&L_MAGIC))
    return str[0..(size/languages[lang][2])];
  return (mixed)languages[lang][1]->squidge_text(str, size);
} /* squish_text() */

int add_language(string name, int flags, mixed ob, int size) {
  languages[name] = ({ flags, ob, size });
} /* add_language() */

int query_flags(string name) {
  if (!languages[name])
    return 0;
  return languages[name][0];
} /* query_flags() */

mixed query_garble_object(string name) {
  if (!languages[name])
    return 0;
  return languages[name][1];
} /* query_garble_object() */

string *query_languages() {
  return m_indices(languages);
} /* query_languages() */

int test_language(string str) {
  return pointerp(languages[str]);
} /* test_language() */
