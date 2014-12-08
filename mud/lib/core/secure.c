
#include <mud/secure.h>
#include <mud/patch_version.h>
#include <mud/config.h>
#include <mud/version.h>
#include <files/log.h>

inherit permission "/lib/core/secure/permission.c";

string * preload;
string * call_out_preload;

static int done;
static mapping snoop_list;

void create() 
{
	permission::create();

	preload = ({ });
	call_out_preload = ({ });
	
	done = FALSE;
	snoop_list = ([ ]);

	call_out("load_secure_object", 0);
} 

// #include "/lib/core/secure/permission.c"
#include "/lib/core/secure/crash.c"
#include "/lib/core/secure/coders.c"
#include "/lib/core/secure/domains.c"

#include "/lib/core/secure/ed_stuff.c"
#include "/lib/core/secure/logging.c"
#include "/lib/core/secure/retire.c"

#include "/lib/core/secure/preload.c"








/*
* This function is called every time a player connects.
* input_to() can't be called from here.
*/
object connect( int cPort ) 
{
	object ob;

	// Con pocos jugadores el objeto login puede descargarse de memoria, este log no
	// es relevante 
	/*
	if (!find_object("/secure/login")) 
	{
		log_file("reboot", "Mud reiniciado el: "+ctime(time(),4)+"["+time()+"]"+"\n");
	}
	*/

	write("Versión de LPmud: "+version()+".\nVersión de Mudlib: " 
		+ MUDLIB_VERSION + " // Último parche: " + PATCH_VERSION + "\n");

	/*
	if (cPort==4000) 
	ob = clone_object("/secure/new_login");
	else
	ob = clone_object("/secure/login");
	*/  
	// Un solo archivo de inicio, Folken 21/01/03
	ob = clone_object("/secure/login");

	write("\n");
	return ob;
} /* connect() */


// int query_admin(string str) 
// {
// 	return ((positions[str] == POS_ADMIN) || (is_administrator(str)));
// } 

// int query_only_lord(string str) 
// {
// 	return positions[str] == POS_ADMIN;
// } 

// string *query_admins() 
// {
// 	return filter_array(keys(positions), "query_only_lord", this_object());
// } 

// int query_player_high_lord(string str) 
// {
// 	return is_administrator(str) && "/secure/login"->test_user(str);
// } 

// string *is_administrators() 
// {
// 	return filter_array(keys(positions), "query_player_high_lord", this_object())+
// 		keys(ADMINISTRATORS);
// } 

// int check_permission(string euid, string *path, int mask);

// string get_root_uid() { return ROOT; }
// string get_bb_uid() { return BACKBONE; }

// string *define_include_dirs() 
// {
// 	return ({ "/include/%s" });
// } 

// int valid_trace() { return 1; }

// void shut(int min) 
// {
// 	"/obj/shut"->shut(min);
// } 

// void remove_checked_master(string name) 
// {
// 	map_delete(checked_master, name);
// } 

// // Wonderflug 96, Secure this baby.
// mapping query_checked_master() { return checked_master + ([ ]); }

// // Allow masters to be unchecked
// void uncheck_master(string str) 
// {
// 	if(!this_player() || !is_administrator(geteuid(this_player(1)))) return;
// 	map_delete(checked_master, str);
// }


