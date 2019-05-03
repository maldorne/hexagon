
// this_player - return the object representing the current player
// object this_player( int flag );

object this_player(varargs int i)
{
  return MUDOS->this_player(i);
}

object this_user()
{
  return MUDOS->this_user();
}

// this_interactive - return the object representing the current player
// object this_interactive();

object this_interactive()
{
  return this_user();
}
