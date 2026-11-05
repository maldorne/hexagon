
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
      cols = destination->query_cols();

    // wrap a line wider than the screen; a shorter one, like a prompt waiting
    // on the same line for what the user types, goes as it is
    if (strlen(message, true) > cols)
      message = sprintf("%-*s", cols, message);
  }

  // With the sidebar of the interface on, a line wider than the text column
  // would run under the panel: every line is wrapped, not only a message of
  // one line. A message wraps to what each of its lines needs, so a box that
  // already fits is left as it is. explode drops a separator at either end of
  // the string, so those are put back after the lines are joined.
  else if (userp(destination) && (int)destination->ui_reserved_cols() > 0 &&
           destination->query_cols() > 0)
  {
    int i, cols, changed;
    string head, tail;

    cols = destination->query_cols();
    changed = false;

    for (i = 0; i < sizeof(pieces); i++)
    {
      if (strlen(pieces[i], true) > cols)
      {
        pieces[i] = sprintf("%-*s", cols, pieces[i]);
        changed = true;
      }
    }

    if (changed)
    {
      head = (message[0] == '\n') ? "\n" : "";
      tail = (message[strlen(message) - 1] == '\n') ? "\n" : "";
      message = head + implode(pieces, "\n") + tail;
    }
  }

  destination->send_message(message);
}