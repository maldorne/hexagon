/*
 * Who.c, once upon in /global/finger.c
 * made splitted by Ducky, some changes by Baldrick and Aragorn.
 * Externalized by someone@RD.
 * Moved back to FR and Fr'ized by Baldrick. oct '95
 *
 * Color added by Radix ... splish splash
 * Damned Hide command, fixed for players and immortals, Radix 1996
 * For an overall ~12% increase, you get who sorted - Radix 1996
 *  - ie with 25 users, it's about 8k cpu cycles sorted, normally 7k
 *  Nevertheless, will improve greatly if sorted the FINAL string
 *  array, instead of doing the obs first
 *
 * Added race option, and cleaned up the 'logon the unknown's - Radix
 * Some translations made by Osucaru@Rl. Nov '97
 * Modified for selective who Spp@Rl Jan'98
 *
 * neverbot 21/01/03, ported to ccmud:
 *  - less filter_array done to the object list
 *  - Added who <my name>, same behaviour as whoami.
 *  - new guild_joined, 18/7/03
 *
 * Translated for Hexagon mudlib, neverbot 09/2019
 *  - removed whoami feature again
 *
 * Options, neverbot 09/2026
 *  - a game is named either by its directory or by the name it shows
 *  - "-games" adds the game of every player as a right-hand column
 */

#include <mud/cmd.h>
#include <living/races.h>
#include <common/properties.h>
#include <user/player.h>
#include <areas/weather.h>
#include <user/terminal.h>
#include <language.h>

inherit CMD_BASE;

// Width of the right-hand column the "-games" option adds
#define GAME_COLUMN 18

// TODO: this shouldn`t be hard coded here
#define RACES ({ \
  "human", \
  "elf", \
})

string query_usage()
{
  return _LANG_WHO_SYNTAX;
}

string query_help()
{
  return _LANG_WHO_HELP;
}

// Sorted and race functions - Radix 1996
int compare_obs(object ob1, object ob2)
{
  string s1, s2;
  s1 = ob1->query_name();
  s2 = ob2->query_name();
  if (s1 == s2)
    return 0;
  if (!s1 || !s2)
    return 1;
  if (s1 > s2)
    return 1;
  return -1;
}

int query_valid_race(string race)
{
  return member_array(lower_case(race), RACES) != -1;
}

int query_inmortal(object me)
{
  return me->query_coder();
}

int no_coders(object me)
{
  return !(me->query_coder());
}

// base_race added, for the subrace/culture system, neverbot 6/03
// a race or the base race of a lineage both match
int compare_race(object ob, string str)
{
  return ((lower_case(ob->query_race_name()) == str) ||
    (lower_case(ob->query_base_race_name()) == str));
}

int remove_logon(object ob)
{
  return (!(ob->query_short() == "logon"));
}

// The guild object somebody belongs to, or nil when they belong to none. The
// object is asked for rather than read from disk: a guild somebody has joined
// is already in memory.
object guild_of(object ob)
{
  mixed path;
  object guild;

  path = ob->query_guild_ob();
  if (!stringp(path) || !strlen(path))
    return nil;

  guild = find_object(path);
  if (!guild)
    catch(guild = load_object(path));

  return guild;
}

// Whether a game answers to this word, by the name of its directory
// ("ciudad-capital") or by the name it shows ("Ciudad Capital")
int game_answers_to(object game, string str)
{
  return (game_name(game) == str) ||
         (lower_case(game->query_game_name()) == str);
}

// Whether the word names a game somebody can be playing
int query_valid_game(string str)
{
  object * games;
  int i;

  games = handler("games")->query_game_objects();

  for (i = 0; i < sizeof(games); i++)
    if (game_answers_to(games[i], str))
      return 1;

  return 0;
}

// Whether the word names a guild somebody connected belongs to, by its name or
// by any of the words it answers to
int query_valid_guild(string str)
{
  object * arr, guild;
  int i;

  arr = players();

  for (i = 0; i < sizeof(arr); i++)
    if ((guild = guild_of(arr[i])) && guild->id(str))
      return 1;

  return 0;
}

int compare_game(object ob, string str)
{
  object game;

  return (game = game_master_object(ob)) && game_answers_to(game, str);
}

int compare_guild(object ob, string str)
{
  object guild;

  return (guild = guild_of(ob)) && guild->id(str);
}

// The options asked for and the filter left over. An option is a word starting
// with a dash and may come before or after the filter, which may itself be
// several words ("Ciudad Capital"). Answers nil when an option is unknown.
mixed * parse_options(string str)
{
  string * words, filter;
  int show_games, i;

  filter = "";
  show_games = 0;

  if (!str || !strlen(str))
    return ({ "", 0 });

  words = explode(str, " ");

  for (i = 0; i < sizeof(words); i++)
  {
    if (!strlen(words[i]))
      continue;

    if (words[i][0] == '-')
    {
      if (member_array(lower_case(words[i]), _LANG_WHO_OPTION_GAMES) == -1)
        return nil;

      show_games = 1;
    }
    else if (strlen(filter))
      filter += " " + words[i];
    else
      filter = words[i];
  }

  return ({ filter, show_games });
}

// A row whose right-hand column ends at the margin. The text carries colour
// codes, which take no room on screen, so the padding is measured on the text
// without them.
string who_row(int width, string text, string right)
{
  int pad;

  pad = width - GAME_COLUMN - 1 -
        strlen(TERM_HANDLER->clean_string(text), TRUE);

  if (pad < 1)
    pad = 1;

  return " " + text + sprintf("%*s", pad, "") + sprintf("%*s", GAME_COLUMN, right);
}

string who_string(int width, int cre, string str)
{
  object * arr, user;
  int i, num_people;
  int creators, what;
  string s, tmp, nam, imm, play, prt, race;
  //  mixed ee;
  string ttl;
  mixed * options;
  int show_games;
  ttl = fix_string("======] %^GREEN%^" + mud_name() + "%^RESET%^ [======");
  // Radix...

  if (!(options = parse_options(str)))
    return _LANG_WHO_SYNTAX + "\n\n" + _LANG_WHO_HELP + "\n";

  str = options[0];
  show_games = options[1];

  if (!strlen(str))
    what = 0;
  else if (str == _LANG_WHO_OPTION_CODERS)
    what = 1;
  else if (str == _LANG_WHO_OPTION_PLAYERS)
    what = 2;
  else if (query_valid_race(str))
    what = 3;
  else if (query_valid_game(lower_case(str)))
    what = 4;
  else if (query_valid_guild(lower_case(str)))
    what = 5;
  else
    return _LANG_WHO_SYNTAX + "\n\n" + _LANG_WHO_HELP + "\n";

  arr = players();
  arr = filter_array(arr, "remove_logon", this_object());

  if (what == 1)
  {
    arr = filter_array(arr, "query_inmortal", this_object());
  }
  else if (what == 3)
  {
    // if we want to filter by an specific race, we can use
    // the query_race_name function
    // filter coders, neverbot 21/01/03
    arr = filter_array(arr, "no_coders", this_object());
    arr = filter_array(arr, "compare_race", this_object(), lower_case(str));
    race = str;
    str = _LANG_WHO_OPTION_PLAYERS;
  }
  else if (what == 4)
  {
    // one game of the mud: whoever stands inside it, coders included
    arr = filter_array(arr, "compare_game", this_object(), lower_case(str));
    arr = sort_array(arr, "compare_obs", this_object());
    race = str;
  }
  else if (what == 5)
  {
    arr = filter_array(arr, "compare_guild", this_object(), lower_case(str));
    arr = sort_array(arr, "compare_obs", this_object());
    race = str;
  }
  else
  {
    arr = sort_array(arr, "compare_obs", this_object());
    race = _LANG_WHO_OPTION_PLAYERS;
  }

  creators = num_people = 0;
  s = imm = play = "";

  prt = "\n";
  prt += sprintf("%p%|*s\n", '-', width, ttl);
  prt += sprintf("%|*s\n", width, _LANG_WHO_REAL_WORLD_DATE, width);
  // the clock of the game the reader is in: this file belongs to no game,
  // so resolving from itself would answer with the never-advanced lib one
  prt += sprintf("%|*s\n", width,
                 handler("weather", this_player())->date_string(), width);

  // traverse the player list
  // if what == 1 only coders
  // if what == 2 only players
  // if what == 3 only players of an specific race
  // if !what coders + players
  for (i = 0; i < sizeof(arr); i++)
  {
    // get the user object
    user = arr[i]->user();

    if (!(tmp = (string) arr[i]->query_short()))
      continue;

    if (arr[i]->query_coder())
    {
      if (what != 2 && what != 3)
      {
        // Here it denies to show "super invis"
        if (user->query_invis() > 1)
        {
          if (!this_player()->query_admin())
            continue;
        }

        if (cre && user->query_invis() == 1)
          tmp = "*" + (string) arr[i]->query_short() + "*";
        if (cre && user->query_invis() == 2)
          tmp = "**" + (string) arr[i]->query_short() + "**";
        if (!cre && user->query_invis() == 1)
          continue;
        s = "";
        nam = tmp;

        if (cre && (tmp = arr[i]->query_in_editor()))
        {
          // only admins see this
          if (stringp(tmp))
            if (this_player()->query_admin())
              s += _LANG_WHO_EDITING_MSG;

        }

        if (strlen(tmp = (string) arr[i]->query_title()))
          s += ", " + tmp;

        if (cre && arr[i]->query_property(AWAY_PROP))
          s += _LANG_WHO_AWAY_MSG;

        if (user->query_idle() > 120)
          s += _LANG_WHO_IDLE_MSG;

        // linkdead marker only after 60 s of dropped connection so a
        // brief tcp glitch does not flag the player as disconnected
        if (user->query_linkdead() &&
            (time() - user->query_linkdead_at()) >= 60)
          s += _LANG_WHO_LINKDEAD_MSG;

        imm += " " + nam + s + "%^RESET%^\n";

        if (!user->query_invis() || cre)
        {
          creators++;
        }
      }
    } // if (query_coder)
    else // is not a coder
      if (what != 1) // have to show players

    {
      tmp = (string) arr[i]->query_short() + " " +
        (arr[i]->query_race_ob() ?
          arr[i]->query_race_ob()->query_race_gender_string(arr[i]) :
          STD_RACE->query_race_gender_string(arr[i]));
      // GENDER_TABLE->query_race_gender_string(arr[i]);

      if (!tmp)
        continue;
      s = "";
      nam = tmp;

      // the title they wear, if any: no comma when there is none
      if (strlen(tmp = (string) arr[i]->query_title()))
        nam += ", " + tmp;

      if (arr[i]->query_property(GUEST_PROP))
        s += _LANG_WHO_GUEST_MSG;

      // if (tmp = (string) arr[i]->query_extitle())
      //   s += " (" + tmp + ")";

      if (user->query_idle() > 120)
        s += _LANG_WHO_IDLE_MSG;

      // linkdead marker only after 60 s of dropped connection so a
      // brief tcp glitch does not flag the player as disconnected
      if (user->query_linkdead() &&
          (time() - user->query_linkdead_at()) >= 60)
        s += _LANG_WHO_LINKDEAD_MSG;

      if (show_games)
        play += who_row(width, nam + s, game_pretty_name(arr[i])) + "\n";
      else
        play += " " + nam + s + "\n";
      num_people++;
    }
  } // for

  if (what != 2 && what != 3)
  {
    // coders
    ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(_LANG_WHO_OPTION_CODERS) + "%^RESET%^ [");

    // show only if we have found coders
    if (creators)
    {
      prt += sprintf("%p%|*s\n", '-', width, ttl);
      prt += imm;
    }
  }

  // players
  if (what != 1)
  {
    if (what >= 3)
    {
      ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(race) + "%^RESET%^ [");
    }
    else
    {
      ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(_LANG_WHO_OPTION_PLAYERS) + "%^RESET%^ [");
    }
    if (num_people)
    {
      prt += sprintf("%p%|*s\n", '-', width, ttl);
      prt += play;
    }
  }

  // only coders
  if (what == 1)
  {
    if (!creators)
      tmp = _LANG_WHO_NO_CODERS_MSG;
    else
      tmp = _LANG_WHO_ONLY_CODERS_MSG;
  }
  // only players
  else if (what == 2 || what == 3)
  {
    if (!num_people)
      tmp = _LANG_WHO_NO_PLAYERS_MSG;
    else if (cre && num_people == 1)
      tmp = _LANG_WHO_IS_THE_ONLY_ONE;
    else if (num_people == 1)
      tmp = _LANG_WHO_YOU_ARE_THE_ONLY_ONE;
    else
      tmp = _LANG_WHO_ONLY_PLAYERS_MSG;
  }
  // what == 0, coders + players
  else
  {
    // uncanny case... except we are in invis 2 and we are the only one coder connected
    if (!num_people && !creators)
      tmp = _LANG_WHO_NO_PLAYERS_MSG;

    else if (num_people + creators == 1)
      tmp = _LANG_WHO_YOU_ARE_THE_ONLY_ONE;

    else if (!creators && num_people)
      tmp = _LANG_WHO_ONLY_PLAYERS_MSG;

    else if (!num_people && creators)
      tmp = _LANG_WHO_ONLY_CODERS_MSG;

    else
      tmp = _LANG_WHO_MULTIPLE_MSG;
  }

  tmp = fix_string(tmp);
  prt += sprintf("%p%|*s\n", '-', width, tmp);

  // NOTE: the old "Disconnected" section that printed rows of raw
  // account emails is intentionally gone. With the linkdead grace
  // implemented in /lib/user.c the disconnected players stay in the
  // normal player list and get flagged inline with _LANG_WHO_LINKDEAD_MSG
  // (see the per-player loop above). Emails no longer appear in `who`.

  return prt;
} /* who_string() */

int do_who(string str)
{
  tell_object(this_player(),
    who_string((int) this_user()->query_cols(),
      this_player()->query_coder(), str));
  return 1;
}

int cmd(string str, object me, string verb)
{
  write(who_string((int) me->user()->query_cols(),
    (int) me->query_coder(), str));
  // me->set_trivial_action();

  return 1;
}