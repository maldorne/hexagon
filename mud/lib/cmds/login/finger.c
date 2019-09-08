
#include <mud/cmd.h>
#include <mud/secure.h>
#include <areas/calendar.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return _LANG_FINGER_SYNTAX;
}

static int cmd(string str, object me, string verb)
{
  string ret, ttl;

  if (!strlen(str))
  {
    object *obs;
    string type;
    int i;
    obs = users();

    write(sprintf("%-12.12s    %-20.20s %-20.20s %-20.20s\n",
        _LANG_FINGER_NAME, _LANG_FINGER_REAL_NAME, _LANG_FINGER_CITY, _LANG_FINGER_BIRTHDAY));

    for (i = 0; i < sizeof(obs); i++)
    {
      string euid;
      string rnstr;
      string pname;
      object player;
      object user;

      user = obs[i];
      player = obs[i]->player();

      // should not happen... EVER!
      if (!player)
        continue;

      if (user->query_invis() && !this_user()->query_coder())
        continue;

      if (user->query_invis() > 1 && !this_user()->query_administrator())
        continue;

      // type = (obs[i]->query_earmuffs() ? "e" : " ");
      euid = geteuid(user);
      type = user->query_object_type();

      if (user->query_object_type() != "X")
      {
        string r, birthday, name;

        rnstr = (string)user->query_real_name();
        pname = player->query_cap_name();
        name  = player->query_name();

        if (rnstr && rnstr[0..0] == ":")
          if (!MASTER->valid_read("/save/players/"+name[0..0]+"/"+name,
              geteuid(this_user())))
            rnstr = "-";

        if (user->query_birthday())
          birthday = handler(CALENDAR_HANDLER)->convert_birthday(user->query_birthday());
        else
          birthday = "";

        r = sprintf("%-12.12s %2.2s %-20.20s %-20.20s %-20.20s\n",
          (user->query_invis() ? "(" + player->query_cap_name() + ")" : "" + player->query_cap_name()),
          type,
          (rnstr?rnstr:" "),
          ((ret = user->query_location())?ret:" "),
          birthday);

        write(r);
      }
      else
      {
        string r;
        r = sprintf("%-12.12s %2.2s\n",
          "login",
          type);
        write(r);
      }
    }

    return 1;
  }

  str = (string)this_player()->expand_nickname(str);

  if ("/lib/core/secure/bastards.c"->query_banish_reason(str))
  {
    string retval;
    retval = sprintf("%35-s%35-s\n", "Nombre : "+str, "(tiene prohibido el acceso)");

    // retval += "No tiene muchos segundos reales de edad.\n";
    // retval += "Nunca tuvo correo (ni lo tendra...).\n";
    // retval += "Sin plan.\nSin futuro.\n";

    ret = "/lib/core/secure/bastards.c"->query_banish_reason(str);
    write(retval + "Prohibido por: '" + capitalize(ret) + "'\n");
    return 1;
  }

  ttl = "===] %^GREEN%^"+mud_name()+"%^RESET%^ [===";

  ret = (string)"/lib/core/secure/finger"->finger_info(str, me);

  if (strlen(ret))
  {
    ret = handler("frames")->frame(ret, ttl, this_user()->query_cols());
    write(ret);
    return 1;
  }
  /*
  else if (me->query_coder() &&
      sscanf(str, "%s@%s", mud, mud) == 2)
  {
    "/net/daemon/out_finger"->do_finger(str);
    return 1;
  }
  */
  else
  {
    notify_fail("Nadie con el nombre " + str + " ha visitado "+mud_name()+".\n");
    return 0;
  }
}

