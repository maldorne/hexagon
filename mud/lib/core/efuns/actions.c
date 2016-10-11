
#include <kernel.h>

// actions available
static mapping _actions;

// debug
mapping query_actions() { return _actions; }

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
  object old_this_player;
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

  // save this_player to restore it afterwards
  old_this_player = this_player();

  // set this_object as currect this_player
  MUDOS->set_initiator_object(this_object());

  verb = words[0];
  
  if (sizeof(words) > 1)
    params = implode(words[1..], " ");
  else
    params = "";

  targets = ({ });
  found = FALSE;

  if (env = environment(this_object()))
    targets += ({ env }) + 
               all_inventory(env) - 
               ({ this_object() });

  targets += ({ this_object() });
  targets += all_inventory(this_object());

  role = this_object()->query_role();
  if (role)
    targets += ({ role });

  stderr(" * command targets <" + to_string(targets) + ">\n");

  for (i = 0; i < sizeof(targets); i++)
  {
    string func;
    string error;

    if ((func = targets[i]->query_action(verb)) != nil)
    {
      mixed result;
      error = catch(result = call_other(targets[i], func, params));

      // wrong function definition, maybe wrong prototype
      // add_action functions should be in the form
      //   int func(string str) or
      //   int func(string str, varargs...)
      if ((result == nil) || (error != nil))
      {
        stderr(" * command: wrong action function for <" + func + "> in " + 
               object_name(targets[i]) + "\n");
        stderr("   -> " + error + "\n");
      }

      // function executed
      if (result)
      {
        found = TRUE;
        break;
      }
    }
  }

  // restore this_player()
  MUDOS->set_initiator_object(old_this_player);

  return found;
}
