
// Users handler
// Keeps tracking of users connected and disconnected

#include <kernel.h>
#include <user/user.h>
#include <user/player.h>
#include <mud/config.h>

#define _USER_OB_POS 0
#define _USER_NAME_POS 1
#define _PLAYER_OB_POS 2
#define _PLAYER_NAME_POS 3


static mapping _users;
static mapping _players;

void create()
{
  // anti-cloning
  if (file_name(this_object()) != "/lib/core/users") 
  {
    write("This object cannot be cloned.\n");
    destruct(this_object());
    return;
  }

  ::create();
  _users = ([ ]);
  _players = ([ ]);

  if (!find_object(USER_OB))
    compile_object(USER_OB);

  if (!find_object(PLAYER_OB))
    compile_object(PLAYER_OB);
}

mixed * info() { return ({ _users, _players }); }

// called only from this object, in 'new_connection'
static void add_new_user(object ob)
{
  string id;

  // if (base_name(previous_object()) != USER_OB)
  //   return;

  id = file_name(ob);

  _users += ([ id : allocate(4) ]);

  // values[_USER_OB_POS]      = the user object
  // values[_USER_NAME_POS]    = the user name
  // values[_PLAYER_OB_POS]    = the player object
  // values[_PLAYER_NAME_POS]  = the player name
  _users[id][_USER_OB_POS]     = ob;
  _users[id][_USER_NAME_POS]   = "";
  _users[id][_PLAYER_OB_POS]   = nil;
  _users[id][_PLAYER_NAME_POS] = "";
}

void remove_user(object ob)
{
  string id;

  // if (base_name(previous_object()) != USER_OB)
  //   return;

  id = file_name(ob);

  _players[_users[id][_PLAYER_NAME_POS]] = nil;
  _users[id] = nil;
}

object * query_users()
{
  int i;
  string * ids;
  object * result;

  ids = m_indices(_users);
  result = ({ });

  for (i = 0; i < sizeof(ids); i++)
    result += ({ _users[ids[i]][_USER_OB_POS] });

  return result;
}

object * query_players()
{
  int i;
  string * ids;
  object * result;

  ids = m_indices(_users);
  result = ({ });

  for (i = 0; i < sizeof(ids); i++)
    result += ({ _users[ids[i]][_PLAYER_OB_POS] });

  return result;
}

string * query_user_names()
{
  int i;
  string * ids;
  string * result;

  ids = m_indices(_users);
  result = ({ });

  for (i = 0; i < sizeof(ids); i++)
    result += ({ _users[ids[i]][_USER_NAME_POS] });

  return result;
}

string * query_player_names()
{
  int i;
  string * ids;
  string * result;

  ids = m_indices(_users);
  result = ({ });

  for (i = 0; i < sizeof(ids); i++)
    result += ({ _users[ids[i]][_PLAYER_NAME_POS] });

  return result;
}

mapping query_user_data()
{
  return _users;
}

string * query_user_ids()
{
  return m_indices(_users);
}

// looks for the user using the account name (email) or the player name
object find_user(string id)
{
  // if (!_users[id])
  //   return nil;

  // return _users[id];

  int i;
  string * ids;
  ids = m_indices(_users);

  for (i = 0; i < sizeof(ids); i++)
    if ((_users[ids[i]][_USER_NAME_POS] == id) ||
        (_users[ids[i]][_PLAYER_NAME_POS] == id))
      return _users[ids[i]][_USER_OB_POS];

  return nil;
}

object find_player(string name)
{
  if (!_players[name])
    return nil;

  return _players[name];
}

int update_user(object user, object player)
{
  string id;

  // only the login object can do this (from /lib/core/login.c)
  if (base_name(previous_object(1)) != "/lib/core/login")
    return -1;

  id = file_name(user);

  if (!_users[id])
    return -1;

  // cannot change names of already logged-in users
  if (_users[id][_USER_NAME_POS] != "")
    return -1;

  _users[id][_USER_NAME_POS] = user->query_account_name();

  if (player)
  {
    _users[id][_PLAYER_OB_POS] = player;
    _users[id][_PLAYER_NAME_POS] = player->query_name();

    _players[player->query_name()] = player;
  }

  return 1;
}

object new_connection()
{
  object new_user;

  new_user = clone_object(find_object(USER_OB));

  add_new_user(new_user);

  return new_user;
}
