
#include <room/room.h>

static string room_zone;

void create()
{
  room_zone = "nowhere";  
}

void set_zone(string str) { room_zone = str; }

string query_zone()
{
/*
#ifdef FAST_CLEAN_UP
  // monsters call this to move, but may not actually come here,
  // so potential clean_up_room {Laggard}
  if ( !room_stabilize  &&  !room_init_time )
  {
    room_init_time = time();
    clean_up_handle = call_out( "clean_up_room", FAST_CLEAN_UP, 0 );
  }
#endif
*/

  return room_zone;
}

mixed stats()
{
  return ({
    ({ "Move Zone", room_zone }),
          });
}