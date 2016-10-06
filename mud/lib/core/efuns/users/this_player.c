
// this_player - return the object representing the current player
// object this_player( int flag );
// this_player() is exactly the same as this_user().

object this_player(varargs int i) 
{
  return MUDOS->this_player(i);
}

// static nomask object this_player() 
// {
//   return this_user();
// }

// object this_player(varargs int flag)
// {
//   return this_user(flag);
// }
