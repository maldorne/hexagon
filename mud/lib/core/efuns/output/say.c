
// say - send a message to all users in the same environment

// varargs void say( string str, object | object array );

// Sends a message to the environment of the originator, all items in the
// same environment, and all items inside of the originator.  The originator
// is this_player(), unless this_player() == 0, in which case, the originator
// is this_object().

// The second argument is optional.  If second argument `obj' is specified,
// the message is sent to all except `obj'.  If `obj' is not an object, but
// an array of objects, all those objects are excluded from receiving the
// message.

static nomask void say(string str, varargs mixed avoid) 
{
  mixed * must_avoid;
  object where;
  object * dest;
  int i;

  must_avoid = ({ });
  dest = ({ });

  if (this_player() != previous_object())
    must_avoid = ({ this_player(), previous_object() });
  else
    must_avoid = ({ this_player() });

  if (!avoid) 
    avoid = must_avoid;
  else
  {
    if (!pointerp(avoid))
      avoid = must_avoid + ({ avoid });
    else
      avoid += must_avoid;
  }

  if (!environment(previous_object()))
  {
    if (this_player() && environment(this_player()))
      where = environment(this_player());
    else
      where = previous_object();
  }
  else
    where = environment(previous_object());

  dest = all_inventory(where) - avoid;

  for (i = 0; i < sizeof(dest); i++)
    event(dest[i], "say", str);
} 
