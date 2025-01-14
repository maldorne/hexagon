
#include <room/room.h>

string * room_zones;

void create()
{
  room_zones = ({ });
}

void set_room_zones(string * str) { room_zones = str; }

string * query_room_zones()
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

  return room_zones;
}

void add_room_zone(string str)
{
  if (member_array(str, room_zones) == -1)
    room_zones += ({ str });
}

mixed * stats()
{
  return ({
    ({ "Move Zone", room_zones }),
          });
}
