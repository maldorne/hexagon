
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "finger <nombre>";
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
        "Nombre", "Nombre real", "Domicilio", "Cumpleaños"));

    for (i = 0; i < sizeof(obs); i++)
    {
      string euid;
      string rnstr;
      string pname;
      object player;

      player = obs[i]->player();

      if (obs[i]->query_invis() && !this_player()->query_coder())
        continue;

      if ((int)obs[i]->query_invis() > 1 && !this_player()->query_administrator())
        continue;

      // type = (obs[i]->query_earmuffs() ? "e" : " ");
      euid = geteuid(obs[i]);
      type = obs[i]->query_object_type();

      if (obs[i]->query_name() != "logon")
      {
        if (obs[i]->query_object_type() != "X")
        {
          string r;

          rnstr = (string)obs[i]->query_real_name();
          pname = player->query_cap_name();

          if (rnstr && rnstr[0..0] == ":")
            if (!MASTER->valid_read("/save/players/"+pname[0..0]+"/"+pname,
                geteuid(this_player(1))))
              rnstr = "-";

          r = sprintf("%-12.12s %2.2s %-20.20s %-20.20s %-20.20s\n",
            (obs[i]->query_invis() ? "(" + player->query_cap_name() + ")" : "" + player->query_cap_name()),
            type,
            (rnstr?rnstr:" "),
            ((ret = obs[i]->query_location())?ret:" "),
            ((ret = (string)obs[i]->query_birthday())?ret:" "));
          write(r);
        }
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
    write(retval+"Prohibido por: '"+capitalize(ret)+"'\n");
    return 1;
  }

  ttl = "======] %^GREEN%^"+mud_name()+"%^RESET%^ [======";

  ret = (string)"/lib/core/secure/finger"->finger_info(str, me);

  if (strlen(ret))
  {
    write(sprintf("%p%|*s\n\n", '-', this_user()->query_cols()+18, ttl));
    write(ret);
    write(sprintf("\n%p%|*s\n\n", '-', this_user()->query_cols()+18, ttl));
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

