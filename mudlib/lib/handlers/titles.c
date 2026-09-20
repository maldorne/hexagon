/*
 * titles.c -- every title that can be had, and who gets it.
 * neverbot 09/2026
 *
 * One flat mapping of id to data. Where a title comes from is a field of the
 * entry, not the way the table is indexed:
 *
 *   ([ "<game>:sword-hardened": ([ "text": "Espada Curtida",
 *                                  "from": TITLE_FROM_CLASS,
 *                                  "needs": ([ "class": "fighter",
 *                                              "level": 10 ]) ]),
 *      "<game>:champion":       ([ "text": ({ "Campeón", "Campeona" }),
 *                                  "from": TITLE_FROM_DEED ]) ])
 *
 * `text` is one string when the language or the word does not tell the genders
 * apart, and a pair when it does. An entry with `needs` is granted on its own as
 * soon as a player meets it; one without is granted by whoever awards the deed,
 * with a call to grant().
 *
 * A game has its own handler inheriting this one, which is where its class,
 * guild and job titles live -- classes belong to each game. Only what belongs to
 * no game stays here, so that it survives a character moving between games, and
 * ids carry the game they come from.
 */

#include <living/titles.h>
#include <language.h>

private mapping titles;

// prototypes
void add_title(string id, mapping data);

void create()
{
  titles = ([ ]);

  // finishing a demo game gives a character its first title, and it comes along
  // when they leave the demo for another game
  add_title("demo-finished", ([ "text": _LANG_TITLE_DEMO_FINISHED,
                                "from": TITLE_FROM_DEED ]));
}

void add_title(string id, mapping data)
{
  if (!stringp(id) || !strlen(id) || !mappingp(data))
    return;

  if (!titles)
    titles = ([ ]);

  titles[id] = data;
}

void remove_title(string id) { titles = map_delete(titles, id); }

mapping query_titles() { return titles; }
mapping query_title_data(string id) { return titles[id]; }
int is_title(string id) { return mappingp(titles[id]); }

// The words of a title for somebody of this gender: the only string when there
// is one, the feminine when there are two and the gender asks for it.
string query_title_text(string id, varargs int gender)
{
  mixed text;

  if (!mappingp(titles[id]))
    return nil;

  text = titles[id]["text"];

  if (stringp(text))
    return text;

  if (!pointerp(text) || !sizeof(text))
    return nil;

  if (gender == 2 && sizeof(text) > 1)
    return text[1];

  return text[0];
}

string query_title_from(string id)
{
  if (!mappingp(titles[id]))
    return nil;

  return titles[id]["from"];
}

// The name a class, guild or job is known by inside a title's conditions: the
// file it lives in, with no directory and no extension.
private string source_name(string path)
{
  string * parts;

  if (!stringp(path) || !strlen(path))
    return "";

  parts = explode(path, "/");
  path = parts[sizeof(parts) - 1];

  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];

  return path;
}

// Whether somebody meets what a title asks for. A level is a floor, so the
// titles of every rank already passed are granted and kept.
private int meets(object who, mapping needs)
{
  if (needs["class"] && source_name(who->query_class_ob()) != needs["class"])
    return 0;

  if (needs["guild"] && source_name(who->query_guild_ob()) != needs["guild"])
    return 0;

  if (needs["job"] && source_name(who->query_job_ob()) != needs["job"])
    return 0;

  if (needs["level"] && who->query_level() < needs["level"])
    return 0;

  if (needs["guild_level"] && who->query_guild_level() < needs["guild_level"])
    return 0;

  if (needs["job_level"] && who->query_job_level() < needs["job_level"])
    return 0;

  return 1;
}

// Give somebody a title and tell them so. Answers whether it was new to them.
int grant(object who, string id)
{
  string text;

  if (!who || !mappingp(titles[id]))
    return 0;

  if (!who->grant_title(id))
    return 0;

  text = query_title_text(id, who->query_gender());

  if (interactive(who))
    tell_object(who, _LANG_TITLE_GRANTED);

  return 1;
}

// Every title somebody now qualifies for, granted at once. Deeds are left
// alone: they have no conditions and are awarded by hand.
int check_titles(object who)
{
  string * ids;
  mapping needs;
  int i, granted;

  if (!who || !who->query_player())
    return 0;

  ids = map_indices(titles);

  for (i = 0; i < sizeof(ids); i++)
  {
    if (who->has_title(ids[i]))
      continue;

    needs = titles[ids[i]]["needs"];

    if (!mappingp(needs) || !map_sizeof(needs))
      continue;

    if (!meets(who, needs))
      continue;

    granted += grant(who, ids[i]);
  }

  return granted;
}

// The titles somebody holds that this game can resolve, in the order they were
// granted. An id from another game is kept in the sheet but not answered here.
string * query_titles_of(object who)
{
  string * held, * result;
  int i;

  result = ({ });

  if (!who)
    return result;

  held = who->query_earned_titles();

  for (i = 0; i < sizeof(held); i++)
    if (mappingp(titles[held[i]]))
      result += ({ held[i] });

  return result;
}
