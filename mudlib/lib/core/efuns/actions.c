
#include <kernel.h>
#include <mud/translations.h>
#include <mud/config.h>

// actions available
static mapping _actions;
// actions that can only be activated by the same object
static mapping _private_actions;

nomask mapping query_actions()
{
  if (!_actions)
    _actions = ([ ]);
  if (!_private_actions)
    _private_actions = ([ ]);

  if (this_player() && (this_object() == this_player()))
    return _actions + _private_actions;

  return _actions;
}

// these actions can be activated from the same object, 
// other objects in the same environment, etc
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

// these actions can be activated only  from the same object
nomask void add_private_action(string function, mixed verbs)
{
  if (!_private_actions)
    _private_actions = ([ ]);

  if (arrayp(verbs))
  {
    int i;

    for (i = 0; i < sizeof(verbs); i++)
      _private_actions[verbs[i]] = function;
  }
  else if (stringp(verbs))
    _private_actions[verbs] = function;
}

// nomask int action_exist(string verb)
// {
//   if (_actions[verb])
//     return 1;
//   return 0;
// }

nomask string query_action(string verb)
{
  if (!_actions)
    _actions = ([ ]);

  if (this_player() && (this_object() == this_player()))
    if (!undefinedp(_private_actions[verb]))
      return _private_actions[verb];

  return _actions[verb];
}

static nomask string query_verb()
{
  return MUDOS->query_current_verb();
}

static nomask void notify_fail(string str)
{
  MUDOS->set_notify_fail_msg(str);
}

// returns the list of every object where inputs from object ob
// will search for available actions
static nomask object * targets(varargs object ob)
{
  object user;
  object * targets;
  object env;

  if (!ob)
    ob = this_object();

  targets = ({ });
  user = ob->user();

  // priority for looking actions
  // first, the user object (our connection)
  // account commands, etc
  if (user)
    targets += ({ user });

  // our role (user privileges)
  if (user && user->query_role())
    targets += ({ user->query_role() });

  // second, our own player ob
  targets += ({ ob });

  // third, our environment and other items in there
  if (env = environment(ob))
    targets += ({ env }) +
               all_inventory(env) -
               ({ ob });

  // last, our inventory
  targets += all_inventory(ob);

  // safety
  targets -= ({ nil });

  return targets;
}

// Execute 'action' for the object this_object() as a command (matching against
// add_actions and such). The object must have called enable_commands() for
// this to have any effect.
// In case of failure, 0 is returned, otherwise a numeric value is returned,
// which is the LPC "evaluation cost" of the command. Bigger numbers mean
// higher cost, but the whole scale is subjective and unreliable.

static int command(string action)
{
  string * words;
  string verb, params;
  object * targets;
  object user;
  int i, found;

  if (!living(this_object()))
    return 0;

  stderr(" * command <" + object_name(this_object()) + "> to do <" + action + ">\n");

  words = explode(action, " ");

  if (!sizeof(words))
    return 0;

  verb = words[0];

  if (sizeof(words) > 1)
    params = implode(words[1..], " ");
  else
    params = "";

  targets = targets(this_object());
  found = FALSE;

  stderr(" * command targets <" + to_string(targets) + ">\n");

  for (i = 0; i < sizeof(targets); i++)
  {
    string func, error;
    error = catch(func = targets[i]->query_action(verb));

    if ((!error) && (func != nil))
    {
      mixed result;

      error = catch(result = call_other(targets[i], func, params));

      if (error)
      { 
        if (LOG_CAUGHT_ERRORS)
        {
          // this will appear in the stderr just after the call trace
          stderr(" * error in action function <" + func + "> in " +
                 object_name(targets[i]) + "\n");
        }
        else
        {
          // print some information about the error anyways (won't have
          // the full call trace)
          stderr(" * error in action function <" + func + "> in " +
                 object_name(targets[i]) + ":\n   " + error + "\n");
        
          // inform the coder, the error has been caught so the driver
          // won't inform about anything
          if (this_player() && this_player()->query_coder()) 
            write("Error in action function <" + func + ">: " + error + "\n");

          write(_LANG_ERROR_HAPPENED);
        }

        MUDOS->set_notify_fail_msg("");
        break;
      }
      else if (result == nil)
      {
        stderr(" * action function <" + func + "> does not exist in " +
               object_name(targets[i]) + "\n");
      }
      else if (result) // function executed, result != 0
      {
        found = TRUE;
        break;
      }
    }
  }

  return found;
}

// commands - returns some information about actions the user can take
// array commands();

// Returns an array of an array of 4 items describing the actions that
// are available to this_object(). The first item is the command
// itself (as passed to add_action()). The second is the set of
// flags (passed to add_action as the third argument, often defaulted
// to 0). The third is the object that defined the action. The fourth
// is the function to be called (<function> if it is a function pointer).

static mixed ** commands(varargs object ob)
{
  object * targets;
  mapping actions;
  string * verbs;
  mixed ** result;
  int i, j;

  if (!ob)
    ob = this_object();

  result = ({ });
  targets = targets(ob);
  actions = ([ ]);

  for (i = 0; i < sizeof(targets); i++)
  {
    actions = targets[i]->query_actions();
    verbs = map_indices(actions);

    for (j = 0; j < sizeof(verbs); j++)
    {
      result += ({ ({ verbs[j], 0, targets[i], actions[verbs[j]], }), });
    }
  }

  return result;
}
