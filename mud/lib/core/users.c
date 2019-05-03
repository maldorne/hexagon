
// Users handler
// Keeps tracking of users connected and disconnected

#include <kernel.h>
#include <user/user.h>
#include <user/player.h>
#include <mud/config.h>

static mapping _users;
static mapping _players;

void create()
{
  ::create();
  _users = ([ ]);
  _players = ([ ]);

  if (!find_object(USER_OB))
    compile_object(USER_OB);

  if (!find_object(PLAYER_OB))
    compile_object(PLAYER_OB);

#ifdef __NETWORK_EXTENSIONS__
  open_port("telnet", TELNET_PORT);
#endif
}

// called only from this object, in 'new_connection'
static void add_new_user(object ob)
{
  string id;

  // if (base_name(previous_object()) != USER_OB)
  //   return;

  id = file_name(ob);

  _users += ([ id : allocate(4) ]);

  // values[0] = the user object
  // values[1] = the user name
  // values[2] = the player object
  // values[3] = the player name
  _users[id][0] = ob;
  _users[id][1] = "";
  _users[id][2] = nil;
  _users[id][3] = "";
}

void remove_user(object ob)
{
  string id;

  // if (base_name(previous_object()) != USER_OB)
  //   return;

  id = file_name(ob);

  _players[_users[id][3]] = nil;
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
    result += ({ _users[ids[i]][0] });

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
    result += ({ _users[ids[i]][2] });

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
    result += ({ _users[ids[i]][1] });

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
    result += ({ _users[ids[i]][3] });

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

object find_user(string id)
{
  if (!_users[id])
    return nil;

  return _users[id];
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
  if (base_name(previous_object()) != "/lib/core/login")
    return -1;

  id = file_name(user);

  if (!_users[id])
    return -1;

  // cannot change names of already logged-in users
  if (_users[id][1] != "")
    return -1;

  _users[id][1] = user->query_account_name();

  if (player)
  {
    _users[id][2] = player;
    _users[id][3] = player->query_name();

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

#ifdef __NETWORK_EXTENSIONS__
object connection(string ipnumber, int port)
{
  stderr(" - connection from: "+ipnumber+" with port "+port+"\n");
  return new_connection();
}

void open(int port)
{
  stderr("Now accepting connections on port "+port+"\n");
}

void close(int force)
{
  stderr("Not accepting any more connections (force "+force+")\n");
}
#endif
