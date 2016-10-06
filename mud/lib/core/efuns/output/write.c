
// write() - send a message to current player
// void write( mixed str );
// Write a message `str' to current player. `str' can also be a number, which
// will be translated to a string.

static nomask void write(string str) 
{
  object user;

  if (nullp(str))
    return;

  user = this_user();

  if (!user)
    user = this_player();

  if (!user)
    return;

  user->send_message(str);
}
