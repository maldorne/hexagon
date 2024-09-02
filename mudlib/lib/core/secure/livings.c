
// Livings handler
// Keeps tracking of objects marked as living
// (set_living_name has been called on them)

#include <living/living.h>
#include <user/user.h>
#include <npc/npc.h>

static mapping _livings;

void create()
{
  // anti-cloning
  if (file_name(this_object()) != "/lib/core/secure/livings") 
  {
    write("This object cannot be cloned.\n");
    destruct(this_object());
    return;
  }

  ::create();
  _livings = ([ ]);
}

mapping query_livings_mapping() { return _livings; }

object * query_livings()
{
  int i;
  string * indices;
  object * result;

  result = ({ });

  indices = map_indices(_livings);

  for (i = 0; i < sizeof(indices); i++)
    result += _livings[indices[i]];

  return result;
}

void _set_living_name(object ob, string name)
{
  // TODO check it's a player object or it inherits from npc object

  // stderr("TEST " + name + " " + to_string(previous_objects()) + "\n");
  // if ((base_name(previous_object()) != PLAYER_OB) &&
  //     (base_name(previous_object()) != NPC_OB) )
  //   return;

  if (previous_object(1) != ob)
    return;

  // players in login
  if (_livings["object"])
    _livings["object"] -= ({ ob });

  if (undefinedp(_livings[name]))
    _livings[name] = ({ ob });
  else
    _livings[name] = _livings[name] + ({ ob }) - ({ nil });
}

void remove_living(object ob)
{
  if (undefinedp(_livings[ob->query_name()]))
    return;

  _livings[ob->query_name()] -= ({ ob });

  if (!sizeof(_livings[ob->query_name()]))
    m_delete(_livings, ob->query_name());
}

object _find_living(string name)
{
  if (undefinedp(_livings[name]))
    return nil;

  _livings[name] -= ({ nil });

  if (sizeof(_livings[name]) > 0)
    return _livings[name][0];

  return nil;
}

int _is_in_livings(object ob)
{
  object * list;
  string name;
  int i;

  name = ob->query_name();

  if (undefinedp(_livings[name]))
    return FALSE;

  list = _livings[name];

  for (i = 0; i < sizeof(list); i++)
    if (list[i] == ob)
      return TRUE;

  return FALSE;
}

// avoid discard or destructions
void dest_me()
{
  error("This object does not allow you to destroy it.\n");
}

void dwep()
{
  error("This object does not allow you to destroy it.\n");
}
