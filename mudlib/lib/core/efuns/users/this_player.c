
// this_player - return the object representing the current player
// object this_player( int flag );

// Return the object representing the current player.  If an int is passed
// to this_player and it is not 0 this function will return this_interactive.
// This means it will return the player who initiated the force, or whatever.
// Useful for security checking in sensative functions.

static object this_player(varargs int i)
{
  return MUDOS->this_player(i);
}

static object this_user()
{
  return MUDOS->this_user();
}

// this_interactive - return the object representing the current player
// object this_interactive();

static object this_interactive()
{
  return this_user();
}
