
#include <kernel.h>

// TODO children

// Raskolnikov Nov 96
object * children(string name) 
{
  //  int i;
  //  object *obs; 
  //  string *block_list;

  //  block_list = ({
  //    "/global/coordinator",
  //    "/global/coordinator.c",
  //    "/global/administrator",
  //    "/global/administrator.c",
  //  });

  //  obs = ::children(name);
  //  if(this_player() && this_player()->query_admin())
  //    return obs;
  
  //  for (i = 0; i < sizeof(obs); i++)
  //    if (member_array(obs[i], block_list))
  //      if(obs[i]->query_invis() == 2)
  //   obs -= ({ obs[i] });

  //  return obs;

  return ({ });

}

// from auto.c
static object * users()
{
  if (!this_object()) 
    return ({ });
  else if (object_name(this_object()) == USER_HANDLER) 
    return ::users();
  else 
    return ::find_object(USER_HANDLER)->query_users();
}

// static object * users()
// {
//    object * all;
//    int i;
//    all = ::users();
//    if (this_player() && this_player()->query_admin())
//       return all;
//    for (i = 0; i < sizeof(all); i++)
//       if (all[i]->query_invis() == 2) 
//          all -= ({ all[i] });
//    return all;
// }
