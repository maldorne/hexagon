
// Livings handler
// Keeps tracking of objects marked as living
// (set_living_name has been called on them)

#include <living/living.h>
#include <user/user.h>

static mapping _livings;

void create() 
{
  ::create();
  _livings = ([ ]);
}

mapping query_livings() { return _livings; }

void _set_living_name(object ob, string name) 
{
  if (base_name(previous_object()) != USER_OB) 
    return;

  if (previous_object() != ob)
    return;

  if (!_livings[name])
    _livings[name] = ({ ob });
  else
    _livings[name] = _livings[name] + ({ ob });
}

void _remove_living(object ob, string name)
{
  if (!_livings[name])
    return;

  _livings[name] -= ({ ob });

  if (!sizeof(_livings[name]))
    m_delete(_livings, name);
}

object _find_living(string name) 
{
  if (!_livings[name])
    return nil;

  // if (sizeof(_livings[name]) > 1) 
  return _livings[name][0];
}
