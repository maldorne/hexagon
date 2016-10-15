
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

static nomask string query_verb()
{
  return MUDOS->query_current_verb();
}

static nomask void notify_fail(string str) 
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

// commands - returns some information about actions the user can take
// array commands();

// Returns an array of an array of 4 items describing the actions that
// are available to this_object().  The first item is the command
// itself (as passed to add_action()).  The second is the set of
// flags (passed to add_action as the third argument, often defaulted
// to 0).  The third is the object that defined the action.  The fourth
// is the function to be called ("&#60;function&#62;" if it is a function pointer).

mixed ** commands(varargs object ob) 
{
  object * targets;
  mapping actions;
  string * verbs;
  mixed ** result;
  int i, j;

  if (!ob)
    ob = this_object();

  result = ({ });
  targets = ({ });

  if (ob->query_role())
    targets += ({ ob->query_role() });

  if (environment(ob))
    targets += ({ environment(ob) }) +
                  all_inventory(environment(ob)) +
                  all_inventory(ob);

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
