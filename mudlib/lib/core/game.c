// Standard game master object

// Radix May 10, 1996, Version 1.4
// Bashed from DW lib by Baldrick for FRMud
// Hacked and made more secure - Hamlet 1996
// Made base object and easily settable sub-dir perms - Radix 1996
// Added better error logging for domains - Quark 1996
// locked out domain masters at Baldrick's request ... Raskolnikov 96
// Converted to game object for Hexagon, neverbot 12/2020

#include <language.h>

int open_read, open_write;
string game_coordinator, game_name;
mapping read_perms, write_perms, dir_owners;
string finger_info;
mixed members;

static void set_game_name(string dom) { game_name = dom; }
static void set_game_coordinator(string lord) { game_coordinator = lord; }
static void set_open_read(int i) { open_read = i; }
static void set_open_write(int i) { open_write = i; }
static void set_finger_info(string info) { finger_info = info; }
static void set_read_perms(mapping i) { read_perms = i; }
static void set_write_perms(mapping i) { write_perms = i; }
static void set_dir_owners(mapping i) { dir_owners = i; }
static void setup_perms() { return; }

int query_open_read() { return open_read; }
int query_open_write() { return open_write; }
int query_prevent_shadow() { return 1; }
string query_dom_lord() { return game_coordinator; }
string query_info() { return finger_info; }

void create()
{
  int i;
  mapping map;

  members = ([ ]);
  read_perms = ([ ]);
  write_perms = ([ ]);
  dir_owners = ([ ]);
  open_read = 1;
  open_write = 0;
  finger_info = _LANG_GAME_DEFAULT_FINGER_INFO;
  seteuid("/lib/core/secure"->creator_file(file_name(this_object())));
  restore_object(file_name(this_object()), 1);
  if (pointerp(members))
  {
    map = ([ ]);
    for (i=0;i<sizeof(members);i++)
      map[members[i]] = "No Project";
      members = map;
    }
}

void dest_me() { destruct(this_object()); }

void save_me()
{
  seteuid("/lib/core/secure"->get_root_uid());
  save_object(file_name(this_object()), 1);
  seteuid("/lib/core/secure"->creator_file(file_name(this_object())));
}

// Hamlet added arg here...
int query_dom_manip(varargs string euid)
{
  if (!euid) euid = geteuid(this_player(1));
  if ((euid == query_dom_lord()) ||
    ("/lib/core/secure"->query_admin(euid) &&
     "/lib/core/secure"->high_programmer(geteuid(this_player()))))
    return 1;
  if (lower_case(euid) == game_name)  return 1;
    return 0;
}

// returns 0 if access granted, 1 if not
int query_restricted(mapping perms, string *path, string euid)
{
  string *checks, *step;
  int i,j,caught;
  mixed arr;
  if (!m_sizeof(perms)) return 0;

  checks = keys(perms);
  for (i = 0; i < sizeof(checks); i++)
  {
    step = explode(checks[i],"/");
    if (sizeof(step) > sizeof(path))
      continue;
    caught = 0;
    for (j = 0; j < sizeof(step); j++)
    {
      if (step[j] != path[j]) break;
      if (j == sizeof(step) -1) caught = 1;
    }
    if (!caught) continue;
    // we have a match (I think!)
    arr = perms[checks[i]];
    if (intp(arr))
    {
      if (arr == 1 && !members[euid]) return 1;
      if (arr == 2 && euid != query_dom_lord()) return 1;
      return -1;
    }
    if (!stringp(arr) && sizeof(arr))
      if (member_array(euid,arr) != -1)
    return -1;
  }
  return 0;
}

int valid_read(string *path, string euid, string funct)
{
  int i;
  if (query_dom_manip(euid))
    return 1;
  i = query_restricted(read_perms, path, euid);
  if (i < 0)
    return 1;
  if (i)
    return 0;
  if (query_open_read())
    return 1;
  return (members[euid] != 0);
}

int valid_write(string *path, string euid, string funct)
{
  int i;
  /* lock out domain masters unless its a high programmer
   * Raskolnikov Nov 96
   */
  if ("/lib/core/secure"->query_admin(euid) &&
    "/lib/core/secure"->high_programmer(geteuid(this_player())))
    return 1;
  switch(path[2])
  {
    case("master") :
    case("master.c") :
    case("master.o") :
//  case("master_mods.c") :
//  case("master_mods") :
      return 0;
      break;
  }
  /* now check open write and dom_manip ... Raskolnikov */
  if (query_open_write())
    return 1;
  if (query_dom_manip(euid))
    return 1;
  if (euid == "Dom: "+game_name)
    return 1;
  i = query_restricted(write_perms, path, euid);
  if (i < 0)
    return 1;
  if (i)
    return 0;
  return (members[euid] != 0);
}

/* please note the + ({ })... this is for security reasons.
 */
string *query_members() { return keys(members) + ({ }); }

int add_member(string name)
{
  if (!query_dom_manip(geteuid(this_player(1)))) return 0;
  if (members[name]) return 0;
  members[name] = "No Project";
  save_me();
  return 1;
} /* add_member() */

int remove_member(string name)
{
  // int i;
  if (!query_dom_manip(geteuid(this_player(1)))) return 0;
  if (!members[name]) return 0;
  members = m_delete(members, name);
  save_me();
  return 1;
} /* remove_member() */

int set_project(string name, string pro)
{
  if (!query_dom_manip(geteuid(this_player(1)))) return 0;
  if (!members[name]) return 0;
  if (!pro || pro == "") pro = "No Project";
  members[name] = pro;
  save_me();
  return 1;
} /* set_project() */

int query_member(string name)
{
  return !undefinedp(members[name]) || name == game_coordinator;
} /* query_member() */

string query_project(string name)
{
  if (!members[name])
    return "project unset";
  return members[name];
} /* query_project() */

void smart_log(string error, string where)
{
  write_file("d/"+game_name+"/player_reports", error);
} /* smart_log() */

string log_who(string where)
{
  // Quark, Sept 1996
  if (!dir_owners) dir_owners = ([ ]);

  if (stringp(dir_owners[where]))
    return dir_owners[where];

  return game_coordinator;
} /* log_who() */

string author_file(string *str)
{
  // neverbot, 6/03
  // switch (str[1])
  // {
  //   case "fr" :
  //     return game_coordinator;
  //   default:
  //     return "baldrick";
  // }

  return game_coordinator; // neverbot
} /* author_file() */


// Called from /std/living/death.c
// Thanes should use this to handle deaths in their domains. - Radix
void domain_death(object victim, object enemy)
{
   /*
      /obj/handlers/pk also has many functions and stats
      on all the player kills
     Use that too, don't make your own - Radix
   */
   return;
}

void mud_death(object victim, object enemy)
{
   /*
      Use this one to catch ALL deaths occuring in the MUD
      - Radix
   */
   return;
}

// Thanes should use this to keep their player handlers accurate
void event_retire(object ob)
{
   return;
}
