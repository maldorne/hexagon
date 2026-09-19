// A room below the surface: a cave, a mine gallery, a sewer tunnel, a
// dungeon chamber. When the room is converted to a location it gets the
// underground component, with the kind the room declares.

inherit "/lib/room.c";

// cave | tunnel | mine | dungeon
private string underground_kind;

void create()
{
  underground_kind = "cave";

  ::create();
}

int query_underground() { return 1; }

string query_underground_kind() { return underground_kind; }
void set_underground_kind(string kind) { underground_kind = kind; }
