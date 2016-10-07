
// void init();
// When the mudlib moves an object "A" inside another object "B", the
// driver (the move_object() efunction) does the following:

// * if "A" is living, causes "A" to call the init() in "B"
// * causes each living object in the inventory of "B" to call init() in
// "A".  regardless of whether "A" is living or not.
// * if "A" is living, causes "A" to call the init() in each object in
// the inventory of "B".

// Note: an object is considered to be living if enable_commands() has
// been called by that object.

// Typically, the init() function in an object is used to call add_action()
// for each command that the object offers.

void init()
{
  stderr(" * init " + object_name(this_object()) + " \n");
  stderr("   - this_object():\n    " + to_string(this_object()) + " \n");
  stderr("   - this_player():\n    " + to_string(this_player()) + " \n");
  stderr("   - this_player(1):\n    " + to_string(this_player(1)) + " \n");
}
