// A fixed object that can be moved aside to uncover an exit of the place it
// stands in: a stone over a hole, a bookcase over a door. The exit is sealed
// (see reveal_exit in /lib/room/exits.c), so it exists for the map but nobody
// sees it until this is moved; it seals itself again when the place reloads.
// Whatever inherits it gives the description, the exit and the messages.

#include <language.h>

inherit "/lib/obj/fixture.c";

private string reveal_exit;
private string move_message, move_room_message;

void create()
{
  reveal_exit = "";
  move_message = _LANG_MOVABLE_MOVED_ME;
  move_room_message = _LANG_MOVABLE_MOVED_ROOM;
  ::create();
}

// The exit of the place this stands in that moving it uncovers.
void set_reveal_exit(string direction) { reveal_exit = direction; }
string query_reveal_exit() { return reveal_exit; }

// What the mover reads, and what the others in the room read after the
// mover's name.
void set_move_messages(string me, string room)
{
  move_message = me;
  move_room_message = room;
}

void init()
{
  ::init();
  add_action("do_move", _LANG_MOVABLE_VERBS);
}

int do_move(string str)
{
  object place;

  if (!str || !id(str))
  {
    notify_fail(_LANG_MOVABLE_WHAT);
    return 0;
  }

  place = environment();
  if (!place || !strlen(reveal_exit))
    return 0;

  // already uncovered: the way is there for anybody to take
  if (!place->query_sealed_exit(reveal_exit))
  {
    write(_LANG_MOVABLE_ALREADY);
    return 1;
  }

  place->reveal_exit(reveal_exit);
  write(move_message);
  tell_room(place, this_player()->query_cap_name() + " " + move_room_message,
            this_player());
  return 1;
}
