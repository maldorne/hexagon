
// catch_tell - provides an interface to interact with users
// void catch_tell( string message );

// Whenever the driver
// has something to tell an object due to say(), shout(), tell_object(), etc.
// catch_tell in the player object will be called with the message to
// be printed.  The message can be displayed, discarded, or modified in
// any way desired.  This is a useful way to have flexible earmuffs, or
// to support intelligent clients.

void catch_tell( string message )
{
  if (interactive(this_object()))
    this_object()->send_message(message);
}