/* Creador (Ni idea)
   Mostrar paths en la info Feb'2000 Goku
   Recodificaion para eliminar multiples peticiones a users() Feb'2000 Goku
   Mostrar Idle sobre usuarios coincidentes Feb'2000 Goku
   Inclusion de coincidencias en environments Feb'2000 Goku
*/

#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
#define BLANK "   "

protected string get_ipmsg(object);
protected string get_file_name(object);
protected string get_filemsg(object);

void setup()
{
    position = 1;
}

protected int cmd(string str, object me, string verb)
{
    object *usuarios = users();
    object *ips, *users_ok, env;
    int i = 0, j=0;
    string ip;

    users_ok = ({ });
    for(i=0;i<sizeof(usuarios);i++) {
	users_ok += ({ usuarios[i] });
    }

    if (!TP->query_coder()) return 0;

    write("%^BOLD%^GREEN%^Gente con el mismo ip:%^RESET%^\n");
    i=0;
    while(i < sizeof(usuarios))
    {
	ip = query_ip_name(usuarios[i]);
	ips  = ({ });
	j = i+1;
	while (j < sizeof(usuarios))
	{
            if (ip && (ip == query_ip_name(usuarios[j])))
	    {
		ips += ({ usuarios[j] });
		usuarios -= ({ usuarios[j] });
	    }
	    j++;
	}
	if(sizeof(ips))
	{
	    write("%^BOLD%^* "+ip+" ->%^RESET%^\n");
	    write(get_ipmsg(usuarios[i]));
	    for (j=0;j<sizeof(ips);j++) {
		write(get_ipmsg(ips[j]));
	    }
	}
	i++;
    }
    write("%^BOLD%^GREEN%^Gente con el mismo environment (party detect)%^RESET%^:\n");
    usuarios = ({ });
    for(i=0;i<sizeof(users_ok);i++) {
	usuarios += ({ users_ok[i] });
    }
    i=0;

    while(i < sizeof(usuarios))
    {
	env   = environment(usuarios[i]);
	ips  = ({ });
	j = i+1;
	while (j < sizeof(usuarios))
	{
	    if (env && (env == environment(usuarios[j])) )
	    {
		ips += ({ usuarios[j] });
		usuarios -= ({ usuarios[j] });
	    }
	    j++;
	}
	if(sizeof(ips))
	{
	    write("%^BOLD%^* "+get_file_name(usuarios[i])+" ->%^RESET%^\n");
	    write(get_filemsg(usuarios[i]));
	    for (j=0;j<sizeof(ips);j++) {
		write(get_filemsg(ips[j]));
	    }
	}
	i++;
    }
    write("%^BOLD%^GREEN%^Gente con el mismo login:%^RESET%^\n");
    usuarios = ({ });
    for(i=0;i<sizeof(users_ok);i++) {
	usuarios += ({ users_ok[i] });
    }
    for (i=0; i<sizeof(usuarios)-1; i++)
    {

	ip = usuarios[i]->query_ident();
	for(j=i+1; j<sizeof(usuarios); j++)
	    if (ip && (ip == usuarios[j]->query_ident()))
		write(ip+"\t--- "+usuarios[i]->query_cap_name()+" y "+
		  usuarios[j]->query_cap_name()+".\n");
    }
    return 1;
}

protected string get_ipmsg(object obj)
{
    string back;

    if( query_idle(obj) > 120 ) {
	back = sprintf("  %sIdle: %d%s %10-s%s%s\n",
	  "%^RED%^",query_idle(obj)/60,"%^RESET%^",obj->query_cap_name(),
	  BLANK,get_file_name(obj));
    } else {
	back = sprintf("          %10-s%s%s\n",obj->query_cap_name(),
	  BLANK,get_file_name(obj));
    }
    return(back);
}

protected string get_file_name(object obj)
{
    return (environment(obj)? virtual_file_name(environment(obj))
      :"Esta en el limbo");
}

protected string get_filemsg(object obj)
{
    string back;

    if( query_idle(obj) > 120 ) {
	back = sprintf("  %sIdle: %d%s %10-s%s%s\n",
	  "%^RED%^",query_idle(obj)/60,"%^RESET%^",obj->query_cap_name(),
BLANK,query_ip_name(obj));
    } else {
	back = sprintf("          %10-s%s%s\n",obj->query_cap_name(),
BLANK,query_ip_name(obj));
    }
    return(back);
}
