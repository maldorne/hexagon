
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
#include <translations/language.h>

mapping languages;

void create()
{
  languages = 
  ([
    STD_LANG :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH + "common", 10 }),
    // this has to be expanded in your own game, examples:
    // "grunt" :         ({ L_SPOKEN, LANGUAGES_PATH + "grunt", 100 }),
    // "wizard spells" : ({ L_WRITTEN|L_MAGIC|L_SIZED, LANGUAGES_PATH + "wizard_lang", 100 }),
    // "elf" :           ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH + "elf", 10 }),
    // "dwarf" :         ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH + "dwarf",10}),
    // "goblin" :        ({ L_SPOKEN|L_WRITTEN|L_DISTANCE, LANGUAGES_PATH + "goblin", 10 }),
    // "werewolf" :      ({ L_SPOKEN|L_DISTANCE, LANGUAGES_PATH + "werewolf",10 }),
    // "drizzt" :        ({ L_SPOKEN, LANGUAGES_PATH + "drizz", 100 }),
  ]);
} /* create() */

int query_language_spoken(string str)
{
  if (!languages[str])
    return 0;
  return languages[str][0]&L_SPOKEN;
}

int query_language_written(string str)
{
  if (!languages[str])
    return 0;
  return languages[str][0]&L_WRITTEN;
}

/* Distance languages can be used with tell and shout */
int query_language_distance(string str)
{
  if (!languages[str])
    return 0;
  return languages[str][0]&L_DISTANCE;
}

/*
 * A magical langage gets a function called on it's object whenever it
 * is read even if the person knows the language.  It is also the language
 * spells are written in.
 *
 */
int query_language_magic(string str)
{
  if (!languages[str])
    return 0;
  return languages[str][0]&L_MAGIC;
}

int query_language_size(string str)
{
  if (!languages[str])
    return 0;
  return languages[str][2];
}

/* This is called to resize the text if it doesnt fit... */
mixed squidge_text(string lang, mixed str, int size)
{
  if (!languages[lang])
    return 0; /* Don't add it... */

  if (!(languages[lang][0]&L_MAGIC))
    return str[0..(size/languages[lang][2])];

  return (mixed)languages[lang][1]->squidge_text(str, size);
}

int add_language(string name, int flags, mixed ob, int size)
{
  languages[name] = ({ flags, ob, size });
}

int query_flags(string name)
{
  if (!languages[name])
    return 0;
  return languages[name][0];
}

mixed query_garble_object(string name)
{
  if (!languages[name])
    return 0;
  return languages[name][1];
}

string *query_languages()
{
  return m_indices(languages);
}

int language_exists(string str)
{
  return arrayp(languages[str]);
}

string garble(string text, string lang)
{
  object language;
  mixed result;

  if (undefinedp(languages[lang]))
    return text;

  language = load_object(query_garble_object(lang));

  if (!language)
    return text; 

  result = language->garble("", text);

  if (!arrayp(result) || (!sizeof(result) >= 2))
    return text;

  return result[1];
}
