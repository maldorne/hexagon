
// write() - send a message to current player
// void write( mixed str );
// Write a message `str' to current player.

static nomask void write(string str)
{
  object user;

  if (undefinedp(str))
    return;

  user = this_user();

  if (!user)
    return;

  str = fix_string(str);

  user->catch_tell(str);
}
