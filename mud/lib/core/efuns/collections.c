
#include <kernel.h>

// TODO children

// Raskolnikov Nov 96
static object * children(string name)
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

// users - return an array of objects containing all interactive players
// object array users();
// Return an array of objects, containing all interactive players.

static nomask object * users()
{
  object * all;
  int i;

  // if (!this_object())
  //   return ({ });
  // else

  if (object_name(this_object()) == USER_HANDLER)
    all = ::users();
  else
    all = ::find_object(USER_HANDLER)->query_users();

  if (this_user() && this_user()->query_admin())
    return all -= ({ nil });

  for (i = 0; i < sizeof(all); i++)
    if (all[i] && (all[i]->query_invis() == 2))
      all -= ({ all[i] });

  return all -= ({ nil });
}

static nomask object * players()
{
  object * all;
  int i;

  if (object_name(this_object()) == USER_HANDLER)
    all = this_object()->query_players();
  else
    all = ::find_object(USER_HANDLER)->query_players();

  if (this_user() && this_user()->query_admin())
    return all -= ({ nil });

  for (i = 0; i < sizeof(all); i++)
    if (all[i] && (all[i]->user()->query_invis() == 2))
      all -= ({ all[i] });

  return all -= ({ nil });
}

static nomask object * coders()
{
  object * all;
  object * result;
  int i;

  all = users();
  result = ({ });

  for (i = 0; i < sizeof(all); i++)
    if (all[i] && all[i]->query_coder())
      result += ({ all[i] });

  return result;
}
