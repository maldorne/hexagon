/*
 * Edited by neverbot@Cc, Apr 04
 *
 * gender 0 only for items (and only by default if no other gender has been set).
 * this is needed for languages different than english
 */

#include <language.h>

int gender;

// create and stats added, neverbot 4/03
void create()
{
  gender = 1;
}

int query_gender() { return gender; }

int query_female() { return gender == 2; }
int query_male() { return gender == 1; }

/* The next function is soley for compatibility with old code. */
/* It should not be used in new source files. */
int query_neuter() { return !gender; }

string query_gender_string()
{
  switch (gender)
  {
    case 2: return _LANG_GENDER_FEMALE;
    default:
    case 1: return _LANG_GENDER_MALE;
  }
}

string query_gender_title()
{
  switch (gender)
  {
    case 2: return _LANG_GENDER_TITLE_FEMALE;
    default:
    case 1: return _LANG_GENDER_TITLE_MALE;
  }
}

string query_objective()
{
  switch (gender)
  {
    case 2: return _LANG_GENDER_OBJECTIVE_FEMALE;
    case 1: return _LANG_GENDER_OBJECTIVE_MALE;
    default: return _LANG_GENDER_OBJECTIVE_NEUTRAL;
  }
}

string query_possessive()
{
  switch (gender)
  {
    case 2: return _LANG_GENDER_POSSESSIVE_FEMALE;
    case 1: return _LANG_GENDER_POSSESSIVE_MALE;
    default: return _LANG_GENDER_POSSESSIVE_NEUTRAL;
  }
}

string query_pronoun()
{
  switch (gender)
  {
    case 1: return _LANG_GENDER_PRONOUN_MALE;
    case 2: return _LANG_GENDER_PRONOUN_FEMALE;
    default: return _LANG_GENDER_PRONOUN_NEUTRAL;
  }
}

// Bugfix by Aragorn 22/1/94
// change by neverbot, can only receive an int.
void set_gender(int arg)
{
  // added for gender in objects 
  if (arg == 0)
  {
    // if (interactive(this_object()))
    if (living(this_object()))
      return;

    gender = 0;
    return;
  }

  if (arg <= 1)
    gender = 1;
  else if (arg >= 2)
    gender = 2;
  // else
  //   gender = arg;
}

string query_article()
{
  if (gender == 2)
    return _LANG_GENDER_ARTICLE_FEMALE;
  return _LANG_GENDER_ARTICLE_MALE;
}

string query_article_plural()
{
  if (gender == 2)
    return _LANG_GENDER_ARTICLE_PLURAL_FEMALE;
  return _LANG_GENDER_ARTICLE_PLURAL_MALE;
}

// if next word is provided, the numeral will depend of the
// first letter of the next word: "a elf" -> "an elf"
string query_numeral(varargs string next_word)
{
   if (gender == 2)
    return _LANG_GENDER_NUMERAL_FEMALE;
   return _LANG_GENDER_NUMERAL_MALE;
}

string query_numeral_plural()
{
   if (gender == 2)
    return _LANG_GENDER_NUMERAL_PLURAL_FEMALE;
   return _LANG_GENDER_NUMERAL_PLURAL_MALE;
}

// makes sense for languages with gender in sustantives
// not used in english
// in spanish we can usually use 'a' for female, and 'o' for male
string query_vowel()
{
   if (gender == 2)
    return _LANG_GENDER_VOWEL_FEMALE;
   return _LANG_GENDER_VOWEL_MALE;
}

// neverbot, 7/03
string query_demonstrative()
{
   if (gender == 2)
    return _LANG_GENDER_DEMONSTRATIVE_FEMALE;
   return _LANG_GENDER_DEMONSTRATIVE_MALE;
}

mixed * stats()
{
  return ({ ({ "Gender", gender, }), });
}
