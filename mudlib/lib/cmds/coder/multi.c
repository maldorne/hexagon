/* 
  command to show multiple logins from the same ip.
  - show paths in the info Feb'2000 Goku
  - refactoring to remove multiple calls to users() Feb'2000 Goku
  - show idle time on matching users Feb'2000 Goku
  - show users in the same environment Feb'2000 Goku
*/

#include <mud/cmd.h>
// #include <language.h>

inherit CMD_BASE;

#define BLANK "   "

private string get_ipmsg(object ob);
private string get_file_name(object ob);
private string get_filemsg(object ob);

void setup()
{
  set_aliases(({ "multi" }));
  set_usage("multi");
  set_help("Shows multiple logins from the same ip and other multiplaying info.");
}

static int cmd(string str, object me, string verb)
{
  object *usrs;
  object *ips, *users_ok, env;
  int i, j, howmany;
  string ip;

  usrs = users();
  users_ok = ({ });
  i = 0;
  j = 0;

  for (i = 0; i < sizeof(usrs); i++)
    users_ok += ({ usrs[i] });

  tell_object(me, "%^BOLD%^GREEN%^People with the same ip:%^RESET%^\n");
  i = 0;
  howmany = 0;
  while (i < sizeof(usrs))
  {
    ip = query_ip_name(usrs[i]);
    ips  = ({ });
    j = i + 1;

    while (j < sizeof(usrs))
    {
      if (ip && (ip == query_ip_name(usrs[j])))
      {
        ips += ({ usrs[j] });
        usrs -= ({ usrs[j] });
      }
      j++;
    }

    if (sizeof(ips))
    {
      howmany++;
      tell_object(me, "%^BOLD%^* "+ip+" ->%^RESET%^\n");
      tell_object(me, get_ipmsg(usrs[i]));
      for (j = 0; j < sizeof(ips); j++) 
        tell_object(me, get_ipmsg(ips[j]));
    }
    i++;
  }

  if (howmany == 0)
    tell_object(me, "  No players with the same ip found.\n");

  tell_object(me, "%^BOLD%^GREEN%^People with the same environment (party detect):%^RESET%^\n");
  usrs = ({ });

  for (i = 0; i < sizeof(users_ok); i++) 
    usrs += ({ users_ok[i] });

  i = 0;
  howmany = 0;
  while (i < sizeof(usrs))
  {
    env = environment(usrs[i]);
    ips = ({ });
    j = i + 1;

    while (j < sizeof(usrs))
    {
      if (env && (env == environment(usrs[j])) )
      {
        ips += ({ usrs[j] });
        usrs -= ({ usrs[j] });
      }
      j++;
    }

    if (sizeof(ips))
    {
      howmany++;
      tell_object(me, "%^BOLD%^* "+get_file_name(usrs[i])+" ->%^RESET%^\n");
      tell_object(me, get_filemsg(usrs[i]));
      for (j = 0; j < sizeof(ips); j++) 
        tell_object(me, get_filemsg(ips[j]));
    }
    i++;
  }

  if (howmany == 0)
    tell_object(me, "  No players with the same environment found.\n");

  tell_object(me, "%^BOLD%^GREEN%^People with the same login:%^RESET%^\n");
  usrs = ({ });
  howmany = 0;

  for (i = 0; i < sizeof(users_ok); i++) 
    usrs += ({ users_ok[i] });

  for (i = 0; i < sizeof(usrs)-1; i++)
  {
    ip = usrs[i]->query_ident();
    for(j=i+1; j<sizeof(usrs); j++)
      if (ip && (ip == usrs[j]->query_ident()))
      {
        howmany++;
        tell_object(me, ip + "\t--- " + usrs[i]->query_cap_name() + " and "+
          usrs[j]->query_cap_name()+".\n");
      }
  }

  if (howmany == 0)
    tell_object(me, "  No multiple logins found.\n");

  return 1;
}

private string get_ipmsg(object obj)
{
  string back;

  if (obj->query_idle() > 120) 
  {
    back = sprintf("  %sIdle: %d%s %-10s%s%s\n",
      "%^RED%^", obj->query_idle()/60,"%^RESET%^", obj->player()->query_cap_name(),
      BLANK, get_file_name(obj));
  } 
  else 
  {
    back = sprintf("      %-10s%s%s\n", obj->player()->query_cap_name(),
      BLANK, get_file_name(obj));
  }

  return back;
}

private string get_file_name(object obj)
{
  return (environment(obj) ? file_name(environment(obj))
    : "no environment");
}

private string get_filemsg(object obj)
{
  string back;

  if (obj->query_idle() > 120) 
  {
    back = sprintf("  %sIdle: %d%s %-10s%s%s\n",
      "%^RED%^", obj->query_idle()/60,"%^RESET%^", obj->player()->query_cap_name(),
      BLANK, query_ip_name(obj));
  } 
  else 
  {
    back = sprintf("      %-10s%s%s\n", obj->player()->query_cap_name(),
      BLANK, query_ip_name(obj));
  }

  return back;
}
