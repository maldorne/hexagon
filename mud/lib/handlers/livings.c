
// Livings handler
// Keeps tracking of objects marked as living
// (set_living_name has been called on them)

#include <living/living.h>
#include <user/user.h>
#include <npc/npc.h>

static mapping _livings;

void create() 
{
  ::create();
  _livings = ([ ]);
}

mapping query_livings_mapping() { return _livings; }

object * query_livings() 
{
  string * indices;
  int i;
  object * result;

  result = ({ });

  indices = map_indices(_livings);

  for (i = 0; i < sizeof(indices); i++)
    result += _livings[indices[i]];

  return result; 
}

void _set_living_name(object ob, string name) 
{  
  // TODO check it's a user object or it inherits from npc object

  // stderr("TEST " + name + " " + to_string(previous_objects()) + "\n");
  // if ((base_name(previous_object()) != USER_OB) &&
  //     (base_name(previous_object()) != NPC_OB) )
  //   return;

  if (previous_object() != ob)
    return;

  // players in login
  if (_livings["object"])
    _livings["object"] -= ({ ob });

  if (!_livings[name])
    _livings[name] = ({ ob });
  else
    _livings[name] = _livings[name] + ({ ob });
}

void remove_living(object ob)
{
  if (!_livings[ob->query_name()])
    return;

  _livings[ob->query_name()] -= ({ ob });

  if (!sizeof(_livings[ob->query_name()]))
    m_delete(_livings, ob->query_name());
}

object _find_living(string name) 
{
  if (!_livings[name])
    return nil;

  // if (sizeof(_livings[name]) > 1) 
  return _livings[name][0];
}
