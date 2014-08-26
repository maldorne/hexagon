
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

// Called only from this object, in 'new_connection'
static void add_new_user(object ob, string name) 
{
  // if (base_name(previous_object()) != USER_OB) 
  //   return;
  users += ([ name : ob ]);
}

void remove_user(object ob, string name) 
{
  // if (base_name(previous_object()) != USER_OB) 
  //   return;
  users[name] = 0;
}

object *query_users() 
{
  return m_values(users);
}

string *query_user_names() 
{
  return m_indices(users);
}

object find_user(string name) 
{
  if (member_array(name, m_indices(users)) == -1) 
    return nil;
  
  return users[name];
}

object new_connection()
{
  object new_user;

  new_user = clone_object(find_object(USER_OB));

  add_new_user(new_user, "test");

  return new_user;
}


