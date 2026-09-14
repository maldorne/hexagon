
// finger object....
// this object returns finger information about a given person.

// Translated for CcMud, neverbot 21/4/2003

#include <mud/mail.h>
#include <mud/secure.h>
#include <areas/calendar.h>
#include <language.h>

// One header row: an indent and two columns of the same width
#define FINGER_ROW "   %-30s%-30s\n"

string account_name;

string title;
int last_log_on, time_on, start_time, gender;
string home_dir, last_on_from;
mapping aliases;
string ident;
string last_pos;
string * social_object_list;
string role_name;

string make_string(mixed *al)
{
  string str;

  str = (string)"/lib/player/alias.c"->alias_string(al);
  sscanf(str, "%s $*$", str);
  return str;
}

string domain_finger(string name)
{
  int i;
  string ret, *names;
  object master, table;

  table = table("finger");
  master = load_object("/game/areas/"+name+"/master");

  if (!master)
    return _LANG_FINGER_DOMAIN_NOT_FOUND;

  if (table)
    name = table->get_nice_name(name);
  else
    name = capitalize(name);

  ret =  _LANG_FINGER_DOMAIN_HEADER;

  if (master->query_dom_lord() != "")
    ret += _LANG_FINGER_DOMAIN_MANAGER;
  else
    ret += _LANG_FINGER_DOMAIN_NO_MANAGER;

  // + implode((string *)master->query_members(), ", ")+".\n";
  // Esto cambiado para que siempre compruebe si siguen existiendo
  names = (string *)master->query_members();
  // Eliminamos de la lista el thane del dominio
  names -= ({ master->query_dom_lord() });

  if (sizeof(names))
  {
    if (sizeof(names) == 1)
      ret += _LANG_FINGER_DOMAIN_ONLY_CODER;
    else
      ret += _LANG_FINGER_DOMAIN_CODERS;

    i = 0;

    while (i < sizeof(names))
    {
      if (file_size(player_save_dir(names[i]) + "player.o") == -1)
        delete(names, i, 1);
      else
        i++;
    }

    for (i = 0; i< sizeof(names); i++)
    {
      ret += capitalize(names[i]);
      if (i != sizeof(names) - 1)
        ret += ", ";
    }
    ret += ".\n";
  }

  if (master->query_info())
    ret += wrap((string)master->query_info(), this_user()->query_cols(), 1);
  else
    ret += _LANG_FINGER_DOMAIN_NO_INFO;

  return ret;
} /* domain_finger() */

// to be able to use gender functions when passing the finger object as a parameter
int query_gender() { return gender; }

string finger_info(string name, varargs object me)
{
  int i;
  string retval, nick, *bing;
  object ob, table, user;
  mapping mail_stat;
  string birth_day, real_name, where, email;
  int may_read;

  table = table("finger");

  // if (nick)
  //  name = nick;

  seteuid("root");

  if (table && (table->get_domain_name(name) != name))
    return domain_finger(table->get_domain_name(name));

  if (file_size("/game/areas/"+name) == -2)
    return domain_finger(name);

  if (!"/lib/core/login"->test_user(name))
    return "";

  title = "";
  birth_day = "";
  last_log_on = 0;
  time_on = 0;
  email = "";
  where = "";
  real_name = "";
  home_dir = "";
  social_object_list = ({ });
  last_on_from = "";
  ident = "";
  start_time = 0;
  aliases = ([ ]);
  last_pos = "";
  gender = 1;
  account_name = "";

  restore_object(player_save_dir(name) + "player.o",1);

  // New account system, neverbot 12/2010
  // separation in users and players, now account info is in the user, neverbot 05/2019
  if (strlen(account_name))
  {
    user = clone_object("/lib/user.c");
    if (user && user->restore_me(account_name))
    {
      real_name = user->query_real_name();
      birth_day = user->query_birthday();
      where = user->query_location();
      email = account_name;
      start_time = user->query_start_time();
      last_on_from = user->query_last_on_from();
    }
    else
      return "";
  }

  // A name kept private is marked with a leading ':' and is only spelled out
  // to somebody who may read the character's own file.
  if (me && SECURE_OB->valid_read(player_save_dir(name) + "player.o",
                                  geteuid(me)))
    may_read = TRUE;

  if (!strlen(real_name) ||
      (real_name[0] == ':' && !may_read))
    real_name = "???";

  // Every row is two columns of the same width, so the labels line up whatever
  // each character has filled in.
  retval = sprintf(FINGER_ROW, _LANG_FINGER_NAME + capitalize(name),
                   _LANG_FINGER_REAL_NAME + real_name);

  if (strlen(birth_day))
    birth_day = handler("calendar")->convert_birthday(birth_day);

  if (strlen(where) > 65)
    where = where[.. 65];

  if (strlen(birth_day) || strlen(where))
    retval += sprintf(FINGER_ROW,
                      strlen(birth_day) ? _LANG_FINGER_BIRTHDAY + birth_day : "",
                      strlen(where) ? _LANG_FINGER_PLACE + where : "");

  // changed by neverbot, players wont see the coders home directories
  // if (home_dir)
  //   retval += sprintf("%30s", "Home directory : "+home_dir);
  // else if (home_dir)
  //   retval += "\n";

  if (me && me->query_coder() && strlen(email))
  {
    // some clientes (zmud) introduce a control character first in the string
    // when the input includes the '@'
    // just in case, check the first character too
    if ((email[0] == ':') || (email[1] == ':'))
    {
      if ((base_name(me) != "/lib/core/login") && may_read)
        retval += sprintf(FINGER_ROW, _LANG_FINGER_EMAIL + email, "");
    }
    else
      retval += sprintf(FINGER_ROW, _LANG_FINGER_EMAIL + email, "");
  }

  if ((role_name == "player") && sizeof(social_object_list))
  {
    if (social_object_list[0])
      retval+= _LANG_FINGER_RACE_GENDER;

    if (social_object_list[1] && me && me->query_coder())
    {
      if ( (file_size(social_object_list[1]) > 0) ||
           (file_size(social_object_list[1]+".c") > 0) )
        retval += _LANG_FINGER_GUILD;
      else
        retval += _LANG_FINGER_WRONG_GUILD;
    }
    else
      retval += "\n";
  }

  if ((role_name != "player"))
  {
    string * domains;
    domains = ({ });

    retval += _LANG_FINGER_CODER;

    // if (me && me->query_coder())
    //       retval += sprintf("%30s", "Directorio raíz: "+home_dir+"\n");

    // Find out which domains they are a member of...
    bing = get_dir("/game/areas/");

    for (i = 0; i < sizeof(bing); i++)
    {
      if ((file_size("/game/areas/"+bing[i]) == -2) &&
           file_size("/game/areas/"+bing[i]+"/master") > 0)
      {
        if ((string)("/game/areas/"+bing[i]+"/master")->query_dom_lord()==name)
        {
          domains += ({ bing[i] });
          bing = delete(bing, i, 1);
          i--;
        }
        else if (!("/game/areas/"+bing[i]+"/master")->query_member(name))
        {
          bing = delete(bing, i, 1);
          i--;
        }
        else
        {
          // bing[i] = capitalize(bing[i]);
        }
      }
    }

    if (sizeof(domains))
    {
      if (table)
        for (i = 0; i < sizeof(domains); i++)
          domains[i] = table->get_nice_name(domains[i]);

      retval += wrap(_LANG_FINGER_MANAGER, this_user()->query_cols(), 1);
    }

    if (!sizeof(bing) && sizeof(domains))
      retval += _LANG_FINGER_MANAGER_ONLY;
    else if (!sizeof(bing))
      retval += _LANG_FINGER_NO_MEMBER;
    else
    {
      if (table)
        for (i = 0; i < sizeof(bing); i++)
          bing[i] = table->get_nice_name(bing[i]);

      retval += _LANG_FINGER_MEMBER_OF;
    }

    // Added by Radix, July 1996
    bing = ({ });

    // catch(bing = "/game/groups/master"->query_patronages(name));
    // if (sizeof(bing))
    //   retval += "Es el Patrón de "+implode(bing,", ")+".\n";
  }

  retval += "\n";

  if (start_time)
    retval += _LANG_FINGER_FIRST_ON + ctime(start_time, 1) + ".\n";

  time_on = -time_on;
  retval += _LANG_FINGER_TIME_ON;

  if ((ob = find_living(name)) && (ob->query_player()) &&
     (!ob->query_coder() || !ob->query_invis()) &&
     (!ob->query_hidden()))
  {
    retval += _LANG_FINGER_CONNECTED_SINCE;
  }
  else
  {
    int tmp_time;

    // should be a nice number...
    tmp_time = time() - last_log_on;
    if (!tmp_time)
      tmp_time = 0;

    retval += _LANG_FINGER_LAST_ON;
  }

  if (ob && (!ob->query_coder() || !ob->query_invis()) &&
      interactive(ob) && ob->user())
  {
    if ((ob->user()->query_idle()) > 0)
      retval += _LANG_FINGER_IDLE_TIME;
  }

  // if (me && MASTER->query_admin(me->query_name()) && ident)
  // retval += ident + "@";

  if (me && SECURE_OB->query_admin(me->query_name()) && last_on_from)
    retval += _LANG_FINGER_LAST_CONNECTION_FROM;
  if (me && SECURE_OB->query_admin(me->query_name()) && last_pos)
    retval += _LANG_FINGER_LAST_POSITION;

  // retval += (string)MAILER->finger_mail(name);
  mail_stat = (mapping)"/lib/handlers/postal"->mail_status(name);

  if (!mail_stat["total"])
  {
    retval += _LANG_FINGER_MAIL_NO_MAIL;
  }
  else
  {
    retval += _LANG_FINGER_MAIL_NUM_MAILS;
    if (mail_stat["unread"])
      retval += _LANG_FINGER_MAIL_UNREAD_MAILS;
    else
      retval += ".\n";
  }

  // alias section
  {
    string file_name;
    file_name = "/home/"+lower_case(name)+"/.plan";

    if (!mappingp(aliases))
      aliases = ([ ]);

    if ((role_name != "player") && (file_size(file_name) >= 0))
    {
      retval += "\n"+_LANG_FINGER_PLAN+"\n";
      for (i = 1; i <= 10; i++)
      {
        string line;
        line = read_file_line(file_name, i, 1);
        if (line == "")
          break;
        else
          retval += "   "+line;
      }
      retval += "%^RESET%^\n";
    }
    else if (aliases[".plan"])
      retval += "\n"+_LANG_FINGER_PLAN+"\n   " + implode(explode(make_string(aliases[".plan"]),
        ";"), "\n   ") + "%^RESET%^\n";
  }

  return retval;
}
