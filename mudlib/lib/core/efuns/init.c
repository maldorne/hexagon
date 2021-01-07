
// void init();
// When the mudlib moves an object "A" inside another object "B", the
// driver (the move_object() efunction) does the following:

// * if "A" is living, causes "A" to call the init() in "B"
// * if "A" is living, causes "A" to call the init() in each object in
//   the inventory of "B", including "A" itself.
// * causes each living object in the inventory of "B" to call init() in
//   "A".  regardless of whether "A" is living or not.

// Note: an object is considered to be living if set_living_name() has
// been called by that object.

// Typically, the init() function in an object is used to call add_action()
// for each command that the object offers.

void init()
{
  stderr(" * init " + object_name(this_object()) + " \n");
  stderr("   - this_object()  : " + object_name(this_object()) + "\n");
  stderr("   - this_player()  : " + (this_player() ? object_name(this_player()) : "nil") + "\n");
  stderr("   - this_player(1) : " + (this_player(1) ? object_name(this_player(1)) : "nil") + "\n");
  stderr("   - this_user()    : " + (this_user() ? object_name(this_user()) : "nil") + "\n");
}
