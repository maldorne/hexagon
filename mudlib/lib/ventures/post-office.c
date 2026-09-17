// Post office room: players read and send their mail here.
// The behaviour lives in /lib/ventures/post-office-actions.c, shared with
// the post office component for locations.

inherit room    "/lib/room.c";
inherit actions "/lib/ventures/post-office-actions.c";

int query_post_office() { return 1; }

void create()
{
  room::create();
  create_sign();
}

void init()
{
  room::init();
  actions::init();
}
