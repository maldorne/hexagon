
#include <room/room.h>

private static int room_create_time;  // time of creation
private static int room_init_time;    // time of previous init
private static int room_stabilize;    // don't bother call_out
private static int clean_up_handle;

// function prototypes
void start_clean_up();

void create()
{
  room_create_time = time();
  clean_up_handle = 0;
  
  start_clean_up();
}

void init()
{
  start_clean_up();
}

void start_clean_up()
{
  int old_call_out;

#ifdef FAST_CLEAN_UP
  // when folk in room, update timers {Laggard}
  old_call_out = remove_call_out( clean_up_handle );
#endif

  room_init_time = time();

#ifdef FAST_CLEAN_UP
  if ( old_call_out < FAST_CLEAN_UP && 
      (room_init_time - room_create_time) < FAST_CLEAN_UP )
  {
    // was merely passing through {Laggard}
    clean_up_handle = call_out( "clean_up_room", FAST_CLEAN_UP, 0 );
  }
  else if ( !room_stabilize )
  {
    clean_up_handle = call_out( "clean_up_room", (old_call_out < 0 ? 0 : old_call_out), 0 );
  }
#endif
}

void update_clean_up()
{
  int old_call_out;

#ifdef FAST_CLEAN_UP
  old_call_out = remove_call_out( clean_up_handle );  // multiple folks in room

  if ( old_call_out > 0 &&
       old_call_out < FAST_CLEAN_UP &&
      (time() - room_create_time) < FAST_CLEAN_UP )
  {
    // was merely passing through {Laggard}
    clean_up_handle = call_out( "clean_up_room", FAST_CLEAN_UP, 0 );
  }
  else if ( !room_stabilize )
  {
    clean_up_handle = call_out( "clean_up_room", SLOW_CLEAN_UP, 0 );
  }
#endif
}

// function called by the driver before swapping
int clean_up( varargs int flag )
{
  object * arr;
  int i, elapsed_time;

  arr = deep_inventory( this_object() );
  i = sizeof( arr );
  elapsed_time = time() - room_create_time;

  if (this_object()->query_property(NO_CLEAN_UP_PROP))
    return 1;
  if (this_object()->query_property("corpse_here"))
    return 1;

  // check for inherited room
  if ( flag )
  {
#ifdef CLEAN_UP_LOG
    log_file( CLEAN_UP_LOG, ctime( time() )
      + " inherited "
      + (room_init_time ? time() - room_init_time : "*0*")
      + "/"
      + elapsed_time
      + " seconds ("
      + memory_info( this_object() )
      + " bytes) "
      + file_name( this_object() )
      + ".\n" );
#endif
    return 0;
  }

  // Loop to find if user inside the room somehow {Begosh}
  // Also check for longer term room usage {Laggard}
  while( i-- )
  {
    if ( userp( arr[i] )
    ||  elapsed_time > SLOW_CLEAN_UP )
    {
#ifdef CLEAN_UP_LOG
      log_file( CLEAN_UP_LOG, ctime( time() )
        + " stabilized "
        + (room_init_time ? time() - room_init_time : "*0*")
        + "/"
        + elapsed_time
        + " seconds ("
        + memory_info( this_object() )
        + " bytes) "
        + file_name( this_object() )
        + ".\n" );
#endif
      // room is frequently used, stop cleaning up
      room_stabilize = 1;
      return 1;
    }
  }

#ifdef CLEAN_UP_LOG
  log_file( CLEAN_UP_LOG, ctime( time() )
    + " clean_up "
    + (room_init_time ? time() - room_init_time : "*0*")
    + "/"
    + elapsed_time
    + " seconds ("
    + memory_info( this_object() )
    + " bytes) "
    + file_name( this_object() )
    + ".\n" );
#endif

  this_object()->dest_me();
  return 0; // don't call back
}

#ifdef FAST_CLEAN_UP
// use call_out to attempt faster clean up {Laggard}
// this is almost but not quite the same as the preceeding function.
// the differences are not subtle.
int clean_up_room( int flag )
{
  object * arr;
  int i, elapsed_time;

  if (this_object()->query_property(NO_CLEAN_UP_PROP))
    return 1;
  if (this_object()->query_property("corpse_here"))
    return 1;

  stderr(" ~ clean_up_room check " + object_name(this_object()) + "\n");

  elapsed_time = time() - room_init_time;
  arr = deep_inventory( this_object() );
  i = sizeof( arr );

  if (room_stabilize)
    return 0;

  remove_call_out( clean_up_handle );

  // Loop to find if user inside the room somehow {Begosh}
  // Also check for recent living (monster?) arrival {Laggard}
  while( i-- )
  {
    if ( userp( arr[i] ) || (arr[i]->query_property(NO_CLEAN_UP_PROP))
    ||  (living( arr[i] ) && elapsed_time < SLOW_CLEAN_UP) )
    {
      // we do a call_out to kill the room later if we can ;)
      clean_up_handle = call_out( "clean_up_room", SLOW_CLEAN_UP, 0 );
      return 1;
    }
  }

#ifdef CLEAN_UP_LOG
  log_file( CLEAN_UP_LOG, ctime( time() )
    + " "
    + elapsed_time
    + "/"
    + (time() - room_create_time)
    + " seconds ("
    + memory_info( this_object() )
    + " bytes) "
    + file_name( this_object() )
    + ".\n" );
#endif

  stderr(" ~ clean_up_room " + object_name(this_object()) + " (done)\n");

  this_object()->dest_me();
  return 0; // don't call back
}
#endif
