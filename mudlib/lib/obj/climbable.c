// A fixed object that can be climbed with the climb skill: a tree, a wall, a
// cliff. Reaching the top takes the climber along an exit of the place it
// stands in, usually a sealed one only this climb can take (see force_exit in
// /lib/room/exits.c). Whatever inherits it gives the description and the
// exit, and may redefine event_climbed to do something else at the top.

inherit "/lib/obj/fixture.c";

private string climb_exit;
private int climb_modifier;

void create()
{
  climb_exit = "";
  climb_modifier = 0;
  ::create();
}

// The exit of the place this stands in that the climb leads to.
void set_climb_exit(string direction) { climb_exit = direction; }
string query_climb_exit() { return climb_exit; }

// Added to the climber's ability: positive is easier, 100 never fails.
void set_climb_modifier(int modifier) { climb_modifier = modifier; }
int query_climb_modifier(object climber) { return climb_modifier; }

// What the climb skill asks of anything it is used on.
int query_climbable() { return 1; }

void event_climbed(object climber)
{
  if (environment() && strlen(climb_exit))
    environment()->force_exit(climb_exit, climber);
}
