
/*
 * The language handler. This will keep track of the info on languages
 * Each language can be a written language a spoken language and have
 * a garble object.
 *
 * The function garble shoud be defined on the garbling object.
 * It should return the entire string to print. So if the
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

int query_language_spoken(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return (TRUE && languages[lang][0]&L_SPOKEN);
}

int query_language_written(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return (TRUE && languages[lang][0]&L_WRITTEN);
}

/* Distance languages can be used with tell and shout */
int query_language_distance(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return (TRUE && languages[lang][0]&L_DISTANCE);
}

/*
 * A magical langage gets a function called on it's object whenever it
 * is read even if the person knows the language.  It is also the language
 * spells are written in.
 *
 */
int query_language_magic(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return (TRUE && languages[lang][0]&L_MAGIC);
}

int query_language_size(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return languages[lang][2];
}

/* This is called to resize the text if it doesnt fit... */
mixed squidge_text(string lang, mixed str, int size)
{
  if (undefinedp(languages[lang]))
    return 0; /* Don't add it... */

  if (!(languages[lang][0]&L_MAGIC))
    return str[0..(size/languages[lang][2])];

  return (mixed)languages[lang][1]->squidge_text(str, size);
}

int add_language(string lang, int flags, mixed ob, int size)
{
  languages[lang] = ({ flags, ob, size });
}

int query_flags(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return languages[lang][0];
}

mixed query_garble_object(string lang)
{
  if (undefinedp(languages[lang]))
    return 0;
  return languages[lang][1];
}

string *query_languages()
{
  return m_indices(languages);
}

int language_exists(string lang)
{
  return arrayp(languages[lang]);
}

// The real-world-language display name for a language id ("common" -> "común"
// in ES, "Common" in EN). Unknown ids fall back to the capitalised id, so a
// game that registers a language without a name entry still shows something
// sensible.
string query_language_display(string lang)
{
  mapping names;

  if (!lang || !strlen(lang))
    return "";

  names = _LANG_LANGUAGE_NAMES;
  if (mappingp(names) && !undefinedp(names[lang]))
    return names[lang];

  return capitalize(lang);
}

// Resolve user input (an id, an alias, or a display name, any case) to the
// canonical lowercase-English id, or 0 when nothing matches. Lets the player
// type `hablar comun` / `hablar Común` and reach the id "common".
string resolve_language(string input)
{
  mapping aliases, names;
  string * ids;
  int i;

  if (!input || !strlen(input))
    return nil;
  input = lower_case(input);

  // already a known id
  if (language_exists(input))
    return input;

  // an alias
  aliases = _LANG_LANGUAGE_ALIASES;
  if (mappingp(aliases))
  {
    ids = map_indices(aliases);
    for (i = 0; i < sizeof(ids); i++)
      if (member_array(input, aliases[ids[i]]) != -1)
        return ids[i];
  }

  // a display name (lower-cased)
  names = _LANG_LANGUAGE_NAMES;
  if (mappingp(names))
  {
    ids = map_indices(names);
    for (i = 0; i < sizeof(ids); i++)
      if (lower_case(names[ids[i]]) == input)
        return ids[i];
  }

  return nil;
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
