/*
 * titles.c -- the titles a living has been given, and the one it wears.
 * neverbot 09/2026
 *
 * A title is kept as its id, never as its text: the text lives in the titles
 * handler of the game, which is what turns an id into words. Whatever is
 * granted stays granted, so a player keeps every title they have ever been
 * given and chooses which one to wear, or none at all.
 */

#include <living/titles.h>
#include <language.h>

// every title this living has been granted, in the order they came
string * earned_titles;
// the id being worn, or "" while none is
string worn_title;
// a title written by hand: an npc's, from its template, or a coder's own text.
// While it is set, it is what this living shows.
string manual_title;

void create()
{
  earned_titles = ({ });
  worn_title = "";
  manual_title = "";
}

void set_title(string str) { manual_title = str ? str : ""; }
string query_manual_title() { return manual_title; }

string * query_earned_titles() { return earned_titles; }
int has_title(string id) { return member_array(id, earned_titles) != -1; }

string query_worn_title() { return worn_title; }

// What this living shows after its name: the title written by hand, the text of
// the one it wears, or nothing. An id the game cannot resolve shows nothing and
// stays in the character sheet all the same.
string query_title()
{
  string text;

  if (strlen(manual_title))
    return manual_title;

  if (!strlen(worn_title))
    return "";

  text = handler(TITLES_HANDLER, this_object())->query_title_text(worn_title,
                                                  this_object()->query_gender());

  return text ? text : "";
}

// Granted for good. The new one is worn right away, which is how a player who
// never touches the command still shows the last thing they earned.
int grant_title(string id)
{
  if (!stringp(id) || !strlen(id) || has_title(id))
    return 0;

  earned_titles += ({ id });
  worn_title = id;
  return 1;
}

// Wear one of the titles this living has been given
int wear_title(string id)
{
  if (!has_title(id))
    return 0;

  worn_title = id;
  return 1;
}

void clear_worn_title() { worn_title = ""; }

// Ask the handler for whatever this living now qualifies for. Called wherever a
// level changes; deeds are granted by whoever awards them.
int check_new_titles()
{
  if (!this_object()->query_player())
    return 0;

  return handler(TITLES_HANDLER, this_object())->check_titles(this_object());
}

mixed * stats()
{
  return ({
           ({ "Titles earned", sizeof(earned_titles), }),
           ({ "Title worn", worn_title, }),
           ({ "Title by hand", manual_title, }),
         });
}
