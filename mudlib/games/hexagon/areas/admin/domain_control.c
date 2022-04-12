/* domain administration room */

#include <mud/secure.h>
#include <mud/access.h>

#include "path.h"

inherit "/lib/room.c";

void setup()
{
// string *doms, com;
// int i;

	set_light(100);
	set_short("Domain room");
	set_long("You float in the empty wastes of the ethereal plane. "+
		"In all directions stars twinkle in the black " +
		"curtain of space, just lying there avaiting your creative hand "+
		"for being developed.\n" +
		"Available commands:\n"+
		"  list                         : list all domains.\n"+
		"  list <domain>                : list members of a domain.\n"+
		"  create <domain>              : create a domain (you are lord).\n"+
		"  create <domain> <lord>       : create a domain for someone else.\n" +
		"  add <creator> <domain>       : add a creator to a domain you own.\n" +
		"  delete <creator> <domain>    : remove a creator from a domain you own.\n" +
		"  project <cre> <dom> <proj>   : Set the creators project.\n");
	add_alias("ethereal", "plane");
	add_item("plane", "This plane works as a void of creativity.\n");
	add_alias("curtain", "space");+
	add_item("space", "Space is something that isn't really there. So how can "+
		"you look at it ?\n");
	add_exit(DIR_SOUTH,ADMIN+"admin3","standard");
	seteuid(ROOT);
} /* setup() */

void init()
{
	::init();
	add_action("list", "list");
	add_action("create_domain", "create");
	add_action("add_creator", "add");
	add_action("delete_creator", "delete");
	add_action("do_project", "project");
} /* init() */

int list(string arg)
{
	string *members, *domains, mast;
	int i;
	int cols;

	if (this_player() != this_player(1))
		return 0;
	domains = get_dir("/d/");
	if(!sizeof(domains)) {
		notify_fail("No domains exist.\n");
		return 0;
	}
	if(arg) {
		if(member_array(arg, domains) == -1) {
			notify_fail("No such domain as " + arg + ".\n");
			return 0;
		} else {
			mast = "/d/" + arg + "/master";
			write(capitalize((string)mast->query_dom_lord()) + " is the lord of " +
				arg + ".\nMembers         Project\n");
			members = (string *)mast->query_members();
			cols = (int)this_user()->query_cols();
			for (i=0;i<sizeof(members);i++)
				printf("%-20s %*-=s\n", members[i], cols-16,
				mast->query_project(members[i]));
			return 1;
		}
	}
	write("Current domains:\n");
	for(i=0; i<sizeof(domains); i++) {
		mast = "/d/" + domains[i] + "/master";
		members = (string *)mast->query_members(arg);
		printf("%15s has %d members and is owned by %s.\n", domains[i],
			sizeof((string *)mast->query_members()),
			capitalize((string)mast->query_dom_lord()));
	}
	return 1;
} /* list() */

int create_domain(string arg) {
	string dom, lord;

	if (this_player() != this_player(1))
		return 0;
	if(!arg) {
		notify_fail("Usage: create <domain>\n" +
			"       create <domain> <lord>\n");
		return 0;
	}
	if(!((MASTER->high_programmer(geteuid(previous_object()))) &&
		(previous_object()->query_administrator())))
	{
		notify_fail("You lack the Power of Change on this scale!\n");
		return 0;
	}
	if(sscanf(arg, "%s %s", dom, lord) != 2) {
		dom = arg;
		lord = (string)this_player()->query_name();
	} else {
		lord = (string)this_player()->expand_nickname(lord);
	}
	if (!MASTER->create_domain(dom, lord)) {
		notify_fail("Failed to create domain " + arg + ".\n");
		return 0;
	}
	write("You create the domain " + dom + " with " + capitalize(lord) +
		" as its lord.\n");
	return 1;
} /* create_domain() */

int add_creator(string arg) {
	string dom, cre, mast;

	if (this_player() != this_player(1))
		return 0;
	if (!arg || sscanf(arg, "%s %s", cre, dom) != 2) {
		notify_fail("Usage: add <creator> <domain name>\n");
		return 0;
	}
	if(file_size("/d/" + dom) != -2) {
		notify_fail("No such domain as " + dom + ".\n");
		return 0;
	}
	cre = (string)this_player()->expand_nickname(cre);
	mast = "/d/" + dom + "/master";
	if(!mast->query_dom_manip())
	{
		notify_fail("You are not the Lord of that domain.\n");
		return 0;
	}
	if(!mast->add_member(cre)) {
		notify_fail("Failed to add " + capitalize(cre) + " to domain " +
			dom + ".\n");
		return 0;
	}
	write("Creator " + capitalize(cre) + " now added to domain " + dom + ".\n");
	return 1;
} /* add_creator() */

int delete_creator(string arg) {
	string cre, dom, mast;

	if (this_player() != this_player(1))
		return 0;
	if (!arg || sscanf(arg, "%s %s", cre, dom) != 2) {
		notify_fail("Usage: delete <creator> <domain>\n");
		return 0;
	}
	if(file_size("/d/" + dom) != -2) {
		notify_fail("No such domain as " + dom + ".\n");
		return 0;
	}
	cre = (string)this_player()->expand_nickname(cre);
	mast = "d/" + dom + "/master";
	if(!mast->query_dom_manip())
	{
		notify_fail("You are not the Lord of that domain.\n");
		return 0;
	}
	if (!mast->remove_member(cre)) {
		notify_fail("Failed to remove " + capitalize(cre) + " from domain " +
			dom + "\n");
		return 0;
	}
	write("Member " + capitalize(cre) + " removed from domain " + dom + ".\n");
	return 1;
} /* delete_creator() */

int do_project(string arg) {
	string cre, dom, project, mast;

	if (this_player() != this_player(1))
		return 0;
	if (!arg || sscanf(arg, "%s %s %s", cre, dom, project) != 3) {
		notify_fail("Usage: project <creator> <domain> <project>\n");
		return 0;
	}
	if(file_size("/d/" + dom) != -2) {
		notify_fail("No such domain as " + dom + ".\n");
		return 0;
	}
	cre = (string)this_player()->expand_nickname(cre);
	mast = "d/" + dom + "/master";
	if(!mast->query_dom_manip())
	{
		notify_fail("You are not the Lord of that domain.\n");
		return 0;
	}
	if (!mast->set_project(cre, project)) {
		notify_fail("Failed to set the project " + project + " for " +
			capitalize(cre) + " in the domain " + dom + "\n");
		return 0;
	}
	write("Project for " + capitalize(cre) + " set to " + project +
		" in the domain " + dom + ".\n");
	return 1;
} /* do_project() */
