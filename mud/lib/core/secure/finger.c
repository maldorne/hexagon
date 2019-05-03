
// finger object....
// this object returns finger information about a given person.

// Translated for CcMud, neverbot 21/4/2003

#include <mud/mail.h>
#include <mud/secure.h>

string account_name;

string title;
int last_log_on, time_on, start_time, gender;
string home_dir, last_on_from;
mapping aliases;
// string guild_ob;
string ident;
string last_pos;
// string race_ob;
string * social_object_list;
string role_name;

string make_string(mixed *al)
{
  string str;

  str = (string)"/global/player/alias.c"->alias_string(al);
  sscanf(str, "%s $*$", str);
  return str;
}

string banish_finger(string name)
{
  string retval;
  // string time, ban_by;
  string *file;

  file = explode(read_file("/banish/"+name+".o"), "\n");
  retval =  sprintf("%35s%35s\n", "Login name : "+name, "Real name : "+
    "Banished");
  retval += "Not real many seconds old.\n";
  retval += "No mail ever.\n";
  retval += "No plan.\nNo future.\n";
  if (this_player(1)->query_coder())
    if (sizeof(file) == 3)
      retval += implode(file, "\n")+"\n";
    else
      retval += "Banish info not in the correct format.\n";
  return retval;
} /* banish_finger() */

string domain_finger(string name)
{
  string ret, master;
  string *nombres;
  int i;
  object table;
  table = table("finger_table");

  master = "/d/"+name+"/master";

  if (table)
    ret =  "Dominio de %^BOLD%^" + table->get_nice_name(name) + "%^RESET%^.\n";
  else
    ret =  "Dominio de %^BOLD%^" + capitalize(name) + "%^RESET%^.\n";

  if (master->query_dom_lord() != "")
    ret += "El coordinador de este dominio es "+
        capitalize(master->query_dom_lord())+".\n";
  else
    ret += "Este dominio no tiene coordinador asignado.\n";

  // + implode((string *)master->query_members(), ", ")+".\n";
  // Esto cambiado para que siempre compruebe si siguen existiendo
  nombres = (string *)master->query_members();
  // Eliminamos de la lista el thane del dominio
  nombres -= ({ master->query_dom_lord() });

  if (sizeof(nombres))
  {
    if (sizeof(nombres) == 1)
    ret += "El único programador del dominio es ";
    else
    ret += "Los programadores actuales del dominio son: ";
    i = 0;
    while( i< sizeof(nombres) ){
    if (file_size("/save/players/"+nombres[i][0..0]+
            "/"+nombres[i]+".o") == -1)
      delete(nombres, i, 1);
    else
      i++;
    }
    for(i = 0; i< sizeof(nombres); i++){
     ret += capitalize(nombres[i]);
     if (i != sizeof(nombres) - 1)
      ret += ", ";
    }
    ret += ".\n";
  }

  if (master->query_info())
  ret += sprintf("  %-=*s", (int)this_user()->query_cols() - 3,
         (string)master->query_info());
  else
  ret += "El dominio no tiene información disponible.\n";
  return ret;
} /* domain_finger() */

// Para poder utilizar las funciones con gender pasando este objeto como parametro
int query_gender() { return gender; }

string finger_info(string name, varargs object me)
{
  int i;
  string retval, nick, *bing;
  object ob, table, user;
  mapping mail_stat;
  string birth_day, real_name, where, email;

  table = table("finger_table");

  if (nick)
   name = nick;

  seteuid("Root");

  if (table && (table->get_domain_name(name) != name))
    return domain_finger(table->get_domain_name(name));

  if (file_size("/d/"+name) == -2)
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

  restore_object("/save/players/"+name[0..0]+"/"+name+".o",1);

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

  if (strlen(real_name))
  {
    if (real_name[0] == ':')
    {
      if (MASTER->valid_read("/save/players/"+name[0..0]+"/"+name+".o", geteuid(me)))
      retval = sprintf("   %-35s%-35s\n", "Nombre: "
               +capitalize(name),"Nombre real: "+real_name);
      else
      retval = sprintf("   %-35s%-35s\n", "Nombre: "+
            capitalize(name), "Nombre Real: ???");
    }
  }

  if (!retval)
    retval =  sprintf("   %-35s%-35s\n", "Nombre: "+capitalize(name),
          "Nombre real: "+(real_name?real_name:"???"));
  if (birth_day)
    retval += sprintf("   %-35s", "Cumpleaños: " + birth_day);

  if (me && me->query_coder())
  {
  if (strlen(email))
  {
    // Algunos clientes (zmud) meten como primer caracter uno de
    //  control, por si acaso tambien comprobamos el caracter 1
    if ((email[0] == ':') || (email[1] == ':'))
    {
      if ((base_name(me) != "/lib/core/login") &&
          (MASTER->valid_read("/save/players/"+name[0..0]+"/"+name,
          geteuid(me))) )
        retval += "E-mail: "+email+"\n";
    }
    else
      retval += "E-mail: "+email+"\n";
  }
  else if (birth_day)
    retval += "\n";
  }

  // Modificado por neverbot, a los jugadores no les importa el
  // directorio de los inmos
  // if (home_dir)
  //   retval += sprintf("%35s", "Home directory : "+home_dir);
  // else if (home_dir)
  //   retval += "\n";
  if (strlen(where))
  {
    if (strlen(where) > 65)
      where = where[..65];
    retval += sprintf("   %-35s", "Ciudad: "+where+"\n");
  }

  if ((role_name == "player") && sizeof(social_object_list))
  {
    if (social_object_list[0])
      retval+= "   %^GREEN%^Es un"+(gender==2?"a ":" ") +
           social_object_list[0]->query_race_gender_string(this_object(), 1)+"%^RESET%^. ";

    if (social_object_list[1] && me && me->query_coder())
    {
      if ( (file_size(social_object_list[1]) > 0) ||
        (file_size(social_object_list[1]+".c") > 0) )
      retval += "   Es miembro del gremio "
           + social_object_list[1]->short()+".\n";
      else
      retval += "   Es miembro de un gremio que no existe.\n";
    }
    else
      retval += "\n";
  }

  if ((role_name != "player"))
  {
    string * domains;
    domains = ({ });

    retval += "   %^GREEN%^Es un"+(gender==2?"a":"")+
              " programador"+(gender==2?"a":"")+" en "+
              mud_name()+"%^RESET%^.\n";

    // if (me && me->query_coder())
    //       retval += sprintf("%35s", "Directorio raíz: "+home_dir+"\n");

    // Find out which domains they are a member of...
    bing = get_dir("/d/");

    for (i = 0; i < sizeof(bing); i++)
    {
      if (file_size("/d/"+bing[i]) == -2)
      {
        if ((string)("/d/"+bing[i]+"/master")->query_dom_lord()==name)
        {
          domains += ({ bing[i] });
          bing = delete(bing, i, 1);
          i--;
        }
        else if (!("/d/"+bing[i]+"/master")->query_member(name))
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

      retval += sprintf("\n   %-=*s", this_user()->query_cols() - 3,
        "Es " + (gender==2?"la":"el") + " coordinador" + (gender==2?"a":"") +
        (sizeof(domains)==1?" del dominio ":" de los dominios " ) +
        query_multiple_short(domains, 0) + ".\n");
    }

    if (!sizeof(bing) && sizeof(domains))
      retval += "   No es miembro de ningún otro dominio.\n";
    else if (!sizeof(bing))
      retval += "\n   No es miembro de ningún dominio.\n";
    else
    {
      retval += "\n   Es miembro de";
      if (sizeof(bing) == 1)
        retval += "l dominio de "  ;
      else
       retval += " los dominios de "  ;

      if (table)
        for (i = 0; i < sizeof(bing); i++)
          bing[i] = table->get_nice_name(bing[i]);

      retval += (string)implode(bing, ", ");
      retval += ".\n";
    }
    // Added by Radix, July 1996
    bing = ({ });

    /*
    catch(bing = "/d/grupos/master"->query_patronages(name));
    if (sizeof(bing))
      retval += "Es el Patrón de "+implode(bing,", ")+".\n";
    */
  }

  retval += "\n";

  if (start_time)
    retval += "   Se conectó por primera vez el "+ctime(start_time,1)+".\n";

  time_on = -time_on;
  retval += "   %^GREEN%^Tiene ";
  if (time_on > 86400)
    retval += sprintf("%d día"+((time_on/86400==1)?"":"s")+", ",
              time_on/86400);
  if (time_on > 3600)
    retval += sprintf("%d hora"+((time_on/3600==1)?"":"s")+", ",
               (time_on/3600)%24);

  retval += sprintf("%d minuto"+(((time_on/60)%60==1)?"":"s")+
            " y %d segundo"+((time_on%60==1)?"":"s")+
            " de antigüedad",
          (time_on/60)%60, time_on%60);
  retval += "%^RESET%^.\n";

  if ((ob=find_living(name)) && (function_exists("is_player",ob)) &&
    (!ob->query_coder() || !ob->query_invis())
    && (!ob->query_hidden()))
  {
    retval += "   Conectad"+(gender==2?"a":"o")+" desde el "+ctime(last_log_on)+".\n";
  }
  else
  {
    int tmp_time, sec, min, hour, day;

    // Should be a nice number...
    tmp_time = time()-last_log_on;
    if (!tmp_time)
    {
      sec = min = hour = day = 0;
    }

    retval += "   Se conectó por última vez hace ";
    if (tmp_time < 60)
      retval += tmp_time+" segundos.\n";
    else if (tmp_time < 60*60) // One minute
      retval += (tmp_time/60)+" minuto"+(tmp_time<120?"":"s")+
              (tmp_time%60?" y "+(tmp_time%60)+" segundo"+
              ((tmp_time%60 < 2)?"":"s"):"")+".\n";
    else if (tmp_time < 24*60*60) // Hours...
      retval += (hour = tmp_time/(60*60))+" hora"+
              (hour<2?"":"s")+((min = (tmp_time/60)%60)?" y "+
               min+" minuto"+(min<2?"":"s"):"")+".\n";
    else // Days...
      retval += (day = tmp_time/(24*60*60))+" día"+
               (day<2?"":"s")+((hour = (tmp_time/(60*60))%24)?" y "+
               hour+" hora"+(hour<2?"":"s"):"")+
               ".\n";
      // retval += "Last logged on "+ctime(last_log_on, 1)+"\n";
  }

  if (ob && (!ob->query_coder() || !ob->query_invis()))
    if (interactive(ob) && ob->user())
    {
      if ((ob->user()->query_idle()) > 0)
        retval += "   Inactiv"+(gender==2?"a":"o")+" desde hace "+((ob->user()->query_idle())/60)+
          " minuto"+(((ob->user()->query_idle())/60 == 1)?"":"s")+" y "+
          ((ob->user()->query_idle())%60)+" segundo"+
          (((ob->user()->query_idle())%60 == 1)?"":"s")+".\n";
    }
    else if (function_exists("is_player", ob))
      retval += "%^GREEN%^Conexión caída%^RESET%^.\n";

  // if (me && MASTER->query_admin(me->query_name()) && ident)
  // retval += ident + "@";

  if (me && MASTER->query_admin(me->query_name()) && last_on_from)
    retval += "\n   Última conexión desde: " + last_on_from + "\n";
  if (me && MASTER->query_admin(me->query_name()) && last_pos)
    retval += "   Última posición: "+last_pos+"\n";

  // retval += (string)MAILER->finger_mail(name);
  mail_stat = (mapping)"/lib/handlers/postal"->mail_status(name);

  if (!mail_stat["total"])
  {
    // retval += "\n   No tiene correo.\n";
  }
  else
  {
    retval += "\n   Tiene "+mail_stat["total"]+" Mud-Mail"+
            ((mail_stat["total"]==1)?"":"s");
    if (mail_stat["unread"])
      retval += ", "+mail_stat["unread"]+" sin leer.\n";
    else
      retval += ".\n";
  }

  // Alias section
  {
    string file_name;
    file_name = "/home/"+lower_case(name)+"/.plan";

    if (!mappingp(aliases))
      aliases = ([ ]);

    //   if (aliases[".proyecto"])
    // //retval += "Project: "+make_string(aliases[".project"])+"\n";
    // retval += "\n   Proyecto:\n   " + implode(explode(make_string(aliases[".proyecto"]),
    //   ";"), "\n   ") + "%^RESET%^\n";

    if ((role_name != "player") && (file_size(file_name) >= 0))
    {
      retval += "\n   Plan:\n";
      for (i = 1; i <= 10; i++)
      {
        string line;
        line = read_file(file_name, i, 1);
        if (line == "")
          break;
        else
          retval += "   "+line;
      }
      retval += "%^RESET%^\n";
    }
    else if (aliases[".plan"])
      retval += "\n   Plan:\n   " + implode(explode(make_string(aliases[".plan"]),
        ";"), "\n   ") + "%^RESET%^\n";
  }

  return retval;
}
