
#include <mud/secure.h>
#include <mud/patch_version.h>
#include <mud/config.h>
#include <mud/version.h>
#include <files/log.h>

inherit permission "/lib/core/secure/permission.c";
inherit bastards   "/lib/core/secure/bastards.c";

string * preload;
string * call_out_preload;

static int done;
static mapping snoop_list;

void create()
{
  permission::create();
  bastards::create();

  preload = ({ });
  call_out_preload = ({ });

  done = FALSE;
  snoop_list = ([ ]);

  // from /lib/core/secure/preload.c, included below
  call_out("load_secure_object", 0);
}

// every valid_xxx function
#include "/lib/core/secure/checks.c"
#include "/lib/core/secure/files.c"

#include "/lib/core/secure/crash.c"
#include "/lib/core/secure/coders.c"
#include "/lib/core/secure/domains.c"

#include "/lib/core/secure/ed_stuff.c"
#include "/lib/core/secure/logging.c"
#include "/lib/core/secure/retire.c"

#include "/lib/core/secure/preload.c"


int query_admin(string str)
{
  return ((positions[str] == POS_ADMIN) || (is_administrator(str)));
}

// int is_admin(string str)
// {
//   return positions[str] == POS_ADMIN;
// }

// string *query_admins()
// {
//   return filter_array(keys(positions), "is_admin", this_object());
// }

// int query_player_high_lord(string str)
// {
//   return is_administrator(str) && "/lib/core/login"->test_user(str);
// }

// for compatibility
int high_programmer(string str) { return query_admin(str); }


int check_permission(string euid, string *path, int mask);

string get_root_uid() { return ROOT; }
string get_bb_uid() { return BACKBONE; }

// Radix, July 1996
// Called from create() in /std/object.c
// if creator made me, tag their name to it

string get_create_me(string tmp)
{
  int i;
  object obj;

  obj = initial_object();

  if (obj->query_player())
    obj = obj->user();

  // for (i = sizeof(obj)-1; i > -1; --i)
  //    if (obj[i]->query_coder())
  //       return geteuid(obj[i]);

  if (obj->query_coder())
    return geteuid(obj);

  return tmp;
}

string *define_include_dirs()
{
  return ({ "/include/%s" });
}

int valid_trace() { return 1; }

void shut(int min)
{
  "/obj/shut"->shut(min);
}

void remove_checked_master(string name)
{
  map_delete(checked_master, name);
}

// Wonderflug 96, Secure this baby.
mapping query_checked_master() { return checked_master + ([ ]); }

// Allow masters to be unchecked
void uncheck_master(string str)
{
  if(!this_player() || !is_administrator(geteuid(this_player(1)))) return;
  map_delete(checked_master, str);
}
