

// actions available
static mapping actions;


void create()
{
  actions = ([ ]);
}

void add_action(string function, mixed verbs) 
{
  if (!actions) 
    actions = ([ ]);
  
  if (arrayp(verbs))
  {
    int i;

    for (i = 0; i < sizeof(verbs); i++)
      actions[verbs[i]] = function;
  }
  else if (stringp(verbs))
    actions[verbs] = function;
}

// TODO perform_action or do_action or something like that
// the code that launches the function for a given verb

int perform_action (string verb, varargs string arg) 
{
  if (!actions || !actions[verb]) 
    return 0;
  
  if (arg) 
    return call_other(this_object(), actions[verb], arg);
  else
    return call_other(this_object(), actions[verb]) ;
}

mixed * stats() 
{
  return ({ });
}

