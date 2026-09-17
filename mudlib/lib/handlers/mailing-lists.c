// Mailing lists: a name anyone can write to that stands for several
// characters. Based on the Discworld mail tracker, rewritten for
// Hexagon, neverbot 09/2026.
//
// Two kinds of list:
// - stored lists, created by an administrator and kept by their controllers,
//   who add and remove members;
// - game lists, one per game, whose members are the coordinator and the
//   members of that game's master object. They follow the game on their own
//   and nobody edits them here.

#include <mud/secure.h>
#include <files/postal.h>

#define LIST_MEMBERS     "members"
#define LIST_CONTROLLERS "controllers"

// list name -> ([ "members" : ({ }), "controllers" : ({ }) ])
mapping lists;

void create()
{
  seteuid(ROOT);
  lists = ([ ]);
  restore_object(MAILING_LISTS_SAVE, 1);
}

private void save_me()
{
  save_object(MAILING_LISTS_SAVE, 1);
}

// The character doing the change, and whether an administrator plays it.
private string query_actor()
{
  if (!this_user() || !this_user()->player())
    return "";

  return this_user()->player()->query_name();
}

private int actor_is_admin()
{
  return this_user() && this_user()->query_admin();
}

private string * query_game_names()
{
  string * dirs, * result;
  int i;

  dirs = get_dir("/games/");
  result = ({ });

  for (i = 0; i < sizeof(dirs); i++)
    if (file_exists("/games/" + dirs[i] + "/master.c"))
      result += ({ dirs[i] });

  return result;
}

int query_game_list(string name)
{
  return member_array(name, query_game_names()) != -1;
}

int query_list(string name)
{
  return !undefinedp(lists[name]) || query_game_list(name);
}

string * query_lists()
{
  return map_indices(lists) + query_game_names();
}

string * query_members(string name)
{
  object master;
  string coordinator;
  string * result;

  if (lists[name])
    return lists[name][LIST_MEMBERS] + ({ });

  if (!query_game_list(name))
    return ({ });

  if (!(master = load_object("/games/" + name + "/master")))
    return ({ });

  result = master->query_members();
  coordinator = master->query_game_coordinator();

  if (strlen(coordinator) && member_array(coordinator, result) == -1)
    result += ({ coordinator });

  return result;
}

string * query_controllers(string name)
{
  if (!lists[name])
    return ({ });

  return lists[name][LIST_CONTROLLERS] + ({ });
}

int query_controller(string name, string who)
{
  return lists[name] && member_array(who, lists[name][LIST_CONTROLLERS]) != -1;
}

private int can_change(string name)
{
  return lists[name] && (actor_is_admin() || query_controller(name, query_actor()));
}

// Only administrators create lists; the creator becomes its first member
// and controller. A list cannot take the name of a character or a game.
int create_list(string name)
{
  string creator;

  name = lower_case(name);
  creator = query_actor();

  if (!actor_is_admin() || !strlen(creator))
    return 0;

  if (query_list(name) || player_exists(name))
    return 0;

  lists[name] = ([ LIST_MEMBERS : ({ creator }), LIST_CONTROLLERS : ({ creator }) ]);
  save_me();
  return 1;
}

int delete_list(string name)
{
  if (!can_change(name))
    return 0;

  map_delete(lists, name);
  save_me();
  return 1;
}

int add_member(string name, string who)
{
  who = lower_case(who);

  if (!can_change(name) || !player_exists(who) ||
      member_array(who, lists[name][LIST_MEMBERS]) != -1)
    return 0;

  lists[name][LIST_MEMBERS] += ({ who });
  save_me();
  return 1;
}

// A controller is always a member too.
int add_controller(string name, string who)
{
  who = lower_case(who);

  if (!can_change(name) || !player_exists(who) ||
      member_array(who, lists[name][LIST_CONTROLLERS]) != -1)
    return 0;

  lists[name][LIST_CONTROLLERS] += ({ who });

  if (member_array(who, lists[name][LIST_MEMBERS]) == -1)
    lists[name][LIST_MEMBERS] += ({ who });

  save_me();
  return 1;
}

int remove_controller(string name, string who)
{
  who = lower_case(who);

  if (!can_change(name) || member_array(who, lists[name][LIST_CONTROLLERS]) == -1)
    return 0;

  lists[name][LIST_CONTROLLERS] -= ({ who });
  save_me();
  return 1;
}

// Removing a member also takes away their control. A list left without
// members is deleted.
int remove_member(string name, string who)
{
  who = lower_case(who);

  if (!can_change(name) || member_array(who, lists[name][LIST_MEMBERS]) == -1)
    return 0;

  lists[name][LIST_MEMBERS] -= ({ who });
  lists[name][LIST_CONTROLLERS] -= ({ who });

  if (!sizeof(lists[name][LIST_MEMBERS]))
    map_delete(lists, name);

  save_me();
  return 1;
}

// A character that stops existing leaves every list.
void retire_user(string who)
{
  string * names;
  int i;

  who = lower_case(who);

  if (query_actor() != who && !actor_is_admin())
    return;
  names = map_indices(lists);

  for (i = 0; i < sizeof(names); i++)
  {
    lists[names[i]][LIST_MEMBERS] -= ({ who });
    lists[names[i]][LIST_CONTROLLERS] -= ({ who });

    if (!sizeof(lists[names[i]][LIST_MEMBERS]))
      map_delete(lists, names[i]);
  }

  save_me();
}
