
// this_player - return the object representing the current player
// object this_player( int flag );
// this_player() is exactly the same as this_user().

object this_player(varargs int i) 
{
  return MUDOS->this_player(i);
}

// this_interactive - return the object representing the current player
// object this_interactive();

// Return the object representing the player that caused the calling function
// to be called.  This returns what this_player() was originally even if
// it changed later due to enable_commands() or command()

object this_interactive()
{
  return this_user();
}
