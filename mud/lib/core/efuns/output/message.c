// message - deliver messages to "living" objects

// void message( mixed class, mixed message, mixed target, mixed exclude );

// message() calls receive_message(mixed class, mixed message) in all objects
// in the target list excluding those in the exclude list. This basically
// tells the object the message.

// Class is the type of message (used for clients and such). An example
// would be 'combat', 'shout', 'emergency' etc.  Any LPC value can be
// passed, though.

// Message is usually a string containing the text to send, though it
// can also be any value.

// Target is a list of objects to be sent the message. This can be either a 
// single object string or object pointer, or may be an array of either.
// If a target is non-living all objects in its environment will receive
// the message.

// Exclude is a list of objects that should not receive the message. This
// can either be one object or an array of objects.

static void message(string message, string class, mixed target, varargs mixed exclude) 
{
  int i;

  if (objectp(target))
    target = ({ target });
  else if (stringp(target))
    target = ({ find_object(target) });
  else if (!pointerp(target))
    return;

  message = fix_string(message);

  if (exclude) 
  {
    if (!pointerp(exclude)) 
      exclude = ({ exclude });

    target -= exclude;
  }

  if (strlen(class))
    message = class + " " + message;

  for (i = 0; i < sizeof(target); i++)
    target[i]->catch_tell(message);
}
