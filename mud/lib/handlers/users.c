
// Users handler
// Keeps tracking of users connected and disconnected

// Do not confuse user objects with player objects, the user objects deal
// with connections and the player objects with the game related issues

#include <kernel.h>
#include <user.h>

static mapping users;

void create() 
{
  ::create();
  users = ([ ]);
  
  if (!find_object(USER_OB)) 
    compile_object(USER_OB);
}

// called only from this object, in 'new_connection'
static void add_new_user(object ob, string id) 
{
  // if (base_name(previous_object()) != USER_OB) 
  //   return;

  users += ([ id : allocate(2) ]);

  // values[0] = the user object
  // values[1] = the user name
  users[id][0] = ob;
  users[id][1] = "";
}

void remove_user(object ob, string id) 
{
  // if (base_name(previous_object()) != USER_OB) 
  //   return;

  users[id] = 0;
}

object * query_users()
{
  string * ids;
  int i;
  object * result;

  ids = m_indices(users);
  result = ({ });

  for (i = 0; i < sizeof(ids); i++)
    result += ({ users[ids[i]][0] });

  return result;
}

string * query_user_names()
{
  string * ids;
  int i;
  string * result;

  ids = m_indices(users);
  result = ({ });

  for (i = 0; i < sizeof(ids); i++)
    result += ({ users[ids[i]][1] });

  return result;
}

mapping query_user_data() 
{
  return users;
}

string * query_user_ids() 
{
  return m_indices(users);
}

object find_user(string id) 
{
  if (!users[id])
    return nil;
  
  return users[id];
}

int update_user_name(string id, string name)
{
  if (!users[id])
    return -1;

  // cannot change names of already logged-in players
  if (users[id][1] != "")
    return -1;

  // only the same object can do that (from /lib/core/user/login.c)
  if (users[id][0] != previous_object())
    return -1;

  users[id][1] = name;

  return 1;
}

object new_connection()
{
  object new_user;

  new_user = clone_object(find_object(USER_OB));

  add_new_user(new_user, file_name(new_user));

  return new_user;
}
