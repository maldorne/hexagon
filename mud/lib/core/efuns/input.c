
static nomask void input_to(string func, varargs int flag, mixed arg...) 
{
  object ob;

  // The call should go to the user associated with this_player().
  //    If there isn't one, probably the best bet is the current user.

  // if (this_player()) 
  //   ob = this_player()->query_user();
  
  // if (!ob) 
    ob = this_user();

  if (ob)
    ob->set_input_to(this_object(), func, flag, arg...);
  
  return;
}


