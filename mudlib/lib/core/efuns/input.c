
// Enable next line of user input to be sent to the local function 'fun' as
// an argument. The input line will not be parsed by the driver.

// Note that input_to is non-blocking which means that the object calling
// input_to does not pause waiting for input. Instead the object continues
// to execute any statements following the input_to. The specified function
// 'fun' will not be called until the user input has been collected.

// If "input_to()" is called more than once in the same execution, only the
// first call has any effect.

// If optional argument 'flag' is non-zero, the line given by the player will
// not be echoed, and is not seen if snooped (this is useful for collecting
// passwords).

// The function 'fun' will be called with the user input as its first argument
// (a string). Any additional arguments supplied to input_to will be passed on to
// 'fun' as arguments following the user input.

static nomask void input_to(string func, varargs int flag, mixed arg...)
{
  object ob;

  // The call should go to the user associated with this_player().
  //    If there isn't one, probably the best bet is the current user.

  // if (this_player())
  //   ob = this_player()->user();

  // if (!ob)

  ob = this_user();

  if (ob)
  {
    ob->set_no_prompt();
    ob->set_input_to(this_object(), func, flag, arg...);
  }

  return;
}


// in_input() - determines if a player is inputting to an input_to

static nomask int in_input(varargs object ob) 
{
  if (!ob)
    ob = this_user();
  
  return ob->query_input_to();
}
