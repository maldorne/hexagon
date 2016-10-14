
#include <kernel.h>

// actions available
static mapping _actions;

mapping query_actions() 
{ 
  if (!_actions) 
    _actions = ([ ]);
  return _actions; 
}

nomask void add_action(string function, mixed verbs) 
{
  if (!_actions) 
    _actions = ([ ]);
  
  if (arrayp(verbs))
  {
    int i;

    for (i = 0; i < sizeof(verbs); i++)
      _actions[verbs[i]] = function;
  }
  else if (stringp(verbs))
    _actions[verbs] = function;
}

nomask int action_exist(string verb)
{
  if (_actions[verb])
    return 1;
  return 0;
}

nomask string query_action(string verb)
{
  if (!_actions) 
    _actions = ([ ]);
  
  return _actions[verb];
}

nomask string query_verb()
{
  return MUDOS->query_current_verb();
}

nomask void notify_fail(string str) 
{
  MUDOS->set_notify_fail_msg(str);
}

// Execute 'action' for the object this_object() as a command (matching against
// add_actions and such).  The object must have called enable_commands() for
// this to have any effect.
// In case of failure, 0 is returned, otherwise a numeric value is returned,
// which is the LPC "evaluation cost" of the command.  Bigger numbers mean
// higher cost, but the whole scale is subjective and unreliable.

int command(string action)
{
  object role;
  string * words;
  string verb, params;
  object * targets;
  object env;
  int i, found;

  stderr(" * command <" + object_name(this_object()) + "> to do <" + action + ">\n");

  words = explode(action, " ");

  if (!sizeof(words))
    return 0;

  verb = words[0];
  
  if (sizeof(words) > 1)
    params = implode(words[1..], " ");
  else
    params = "";

  targets = ({ });
  found = FALSE;

  // priority for looking actions
  // first, our role (user privileges)
  role = this_object()->query_role();
  if (role)
    targets += ({ role });

  // second, our own user ob
  targets += ({ this_object() });

  // third, our environment and other items in there
  if (env = environment(this_object()))
    targets += ({ env }) + 
               all_inventory(env) - 
               ({ this_object() });

  // last, our inventory
  targets += all_inventory(this_object());

  stderr(" * command targets <" + to_string(targets) + ">\n");

  for (i = 0; i < sizeof(targets); i++)
  {
    string func, error;

    error = catch(func = targets[i]->query_action(verb));

    if ((!error) && (func != nil))
    {
      mixed result;

      result = call_other(targets[i], func, params);
      
      if (result == nil)
      {
        write("Se ha producido un error.\n");
        notify_fail("");
        stderr(" * command: wrong action function for <" + func + "> in " + 
               object_name(targets[i]) + "\n");
        // stderr("   -> " + error + "\n");
      }

      // function executed
      if (result)
      {
        found = TRUE;
        break;
      }
    }
  }

  return found;
}
