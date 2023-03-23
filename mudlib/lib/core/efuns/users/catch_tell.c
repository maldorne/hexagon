
// catch_tell - provides an interface to interact with users
// void catch_tell( string message );

// Whenever the driver
// has something to tell an object due to say(), shout(), tell_object(), etc.
// catch_tell in the player object will be called with the message to
// be printed.  The message can be displayed, discarded, or modified in
// any way desired.  This is a useful way to have flexible earmuffs, or
// to support intelligent clients.

// prototypes
static nomask string fix_string(string str);

void catch_tell(string message)
{
  object destination;
  string *pieces;

  if (!strlen(message))
    return;

  destination = this_object();

  // do not catch_tell over player objects, use the user object
  if (destination->query_player())
    destination = destination->user();

  if (!interactive(destination))
    return;

  message = fix_string(message);

  // if we have \n inside the message, let's assume it has
  // already been prepared, so we do not need to sprintf again
  pieces = explode(message, "\n");

  if (sizeof(pieces) == 1)
  {
    int cols;
    cols = 80;

    // width fix, ignore the control characters used for colors
    if (userp(destination) && destination->query_cols())
      cols = destination->query_cols() + (strlen(message) - visible_strlen(message));

    message = sprintf("%-*s", cols, message);
  }

  // if (!destination)
  //   return;

  destination->send_message(message);
}