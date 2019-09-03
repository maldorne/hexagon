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
 */

#include <mud/cmd.h>
#include <living/races.h>
#include <common/properties.h>
#include <user/player.h>
#include <language.h>

inherit CMD_BASE;

// TODO: this shouldn`t be hard coded here
#define RACES ({ \
  "humano", \
  "tresio", \
  "velan", \
  "aenur", \
})


void setup()
{
  position = 0;
}

string query_usage()
{
  return "who";
}

string query_short_help()
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
int compare_race(object ob, string str)
{
  return ((lower_case(ob->query_race_name()) == str) ||
    (lower_case(ob->query_base_race_name()) == str));
}

int remove_logon(object ob)
{
  return (!(ob->query_short() == "logon"));
}

string who_string(int width, int cre, string str)
{
  object * arr, * arr_aux, user;
  int i, num_people, num_disconnected_people;
  int creators, what;
  string s, tmp, nam, imm, play, prt, race;
  //  mixed ee;
  string ttl;
  ttl = fix_string("======] %^GREEN%^" + mud_name() + "%^RESET%^ [======");
  // Radix...

  if (!strlen(str))
    what = 0;
  else if (str == _LANG_WHO_OPTION_CODERS)
    what = 1;
  else if (str == _LANG_WHO_OPTION_PLAYERS)
    what = 2;
  else if (query_valid_race(str))
    what = 3;
  else
    return _LANG_WHO_HELP;

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
  else
  {
    arr = sort_array(arr, "compare_obs", this_object());
    race = _LANG_WHO_OPTION_PLAYERS;
  }

  creators = num_people = 0;
  s = imm = play = "";

  prt = "\n";
  prt += sprintf("%p%|*s\n", '-', width + (strlen(ttl) - visible_strlen(ttl)), ttl);
  prt += sprintf("%|*s\n", width, _LANG_WHO_REAL_WORLD_DATE, width);
  prt += sprintf("%|*s\n", width, ctime(time(), 3), width);

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
      if (!what || what == 1)
      {
        // Here it denies to show "super invis"
        if (user->query_invis() > 1)
        {
          if (!this_player()->query_administrator())
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

        if (tmp = (string) arr[i]->query_gtitle())
          s += ", " + tmp;

        // if (tmp = (string) arr[i]->query_title())
        //   s += ", " + tmp;

        if (tmp = arr[i]->query_extitle())
          s += " (" + tmp + ")";

        if (cre && arr[i]->query_property(AWAY_PROP))
          s += _LANG_WHO_AWAY_MSG;

        if (user->query_idle() > 120)
          s += _LANG_WHO_IDLE_MSG;

        // imm += sprintf(" %s%*-=s", nam, width, s) + "%^RESET%^\n";
        imm += sprintf(" %s%-*s", nam, width, s) + "%^RESET%^\n";

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

      if (arr[i]->query_property(GUEST_PROP))
        s += _LANG_WHO_GUEST_MSG;
      if (tmp = (string) arr[i]->query_extitle())
        s += " (" + tmp + ")";
      if (user->query_idle() > 120)
        s += _LANG_WHO_IDLE_MSG;

      play += sprintf("          %*-=s", width - 10, nam + s) + "\n";
      num_people++;
    }
  } // for

  if (!what || (what == 1))
  {
    // coders
    ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(_LANG_WHO_OPTION_CODERS) + "%^RESET%^ [");

    // show only if we have found coders
    if (creators)
    {
      prt += sprintf("%p%|*s\n", '-', width + (strlen(ttl) - visible_strlen(ttl)), ttl);
      prt += imm;
    }
  }

  // players
  if (what != 1)
  {
    if (what == 3)
    {
      ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(race) + "%^RESET%^ [");
    }
    else
    {
      ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(_LANG_WHO_OPTION_PLAYERS) + "%^RESET%^ [");
    }
    if (num_people)
    {
      prt += sprintf("%p%*|s\n", '-', width + (strlen(ttl) - visible_strlen(ttl)), ttl);
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
  prt += sprintf("%p%|*s\n", '-', width + (strlen(tmp) - visible_strlen(tmp)), tmp);

  // added by neverbot, to check disconnected people
  // (they won't appear in users users())
  if (this_player()->query_coder())
  {
    // every object with enable_commands()
    // arr_aux = livings();
    // remove the users appearing in the who
    // arr_aux -= players();

    arr_aux = users();
    num_disconnected_people = 0;

    for (i = 0; i < sizeof(arr_aux); i++)
    {
      if (!arr_aux[i]->player())
      {
        if (this_user()->query_coder()) {
          prt += sprintf("          %-*s", width, arr_aux[i]->query_email()) + "\n";
        }
        num_disconnected_people++;
      }
    }

    if (num_disconnected_people > 0)
    {
      tmp = fix_string(_LANG_WHO_DISCONNECTED_MSG);
      prt += sprintf("%p%|*s\n", '-', width + (strlen(tmp) - visible_strlen(tmp)), tmp);
    }
  }

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