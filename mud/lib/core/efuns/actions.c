
// defined in /lib/core/basic/actions.c

void add_action(string function, mixed verbs);

// TODO command

// Execute 'str' for the object this_object() as a command (matching against
// add_actions and such).  The object must have called enable_commands() for
// this to have any effect.
// In case of failure, 0 is returned, otherwise a numeric value is returned,
// which is the LPC "evaluation cost" of the command.  Bigger numbers mean
// higher cost, but the whole scale is subjective and unreliable.

int command(string action)
{
  return 0;
}

