
#include <basic/move.h>

private int move_flag;

// added Mar 3 '93 by Dank to support goback command 
private object prev; 

void create()
{
  prev = nil;
}

object query_prev() { return prev; }

// default to being gettable and dropable...
// modify as you will...
void reset_get() { move_flag |= CAN_BE_DROPPED; }  // can't be gotten 
void set_get() { move_flag &= ~CAN_BE_DROPPED; }   // can be gotten 
void reset_drop() { move_flag |= CAN_BE_GOTTEN; }  // can't be dropped 
void set_drop() { move_flag &= ~CAN_BE_GOTTEN; }   // can be dropped

// these should have been called "query...", but they're not
int drop() { return move_flag & CAN_BE_DROPPED; }
int get() { return move_flag & CAN_BE_DROPPED; }
int gettable() { return !get(); }

void set_move_flag(int i) { move_flag = i; }

int move(mixed dest, varargs mixed messin, mixed messout) 
{
  object destination;
  object previous;

  previous = environment();
  
  if (!dest)
    return MOVE_EMPTY_DEST;

  if (objectp(dest))
    destination = dest;
  else
  {
    destination = load_object(dest);

    // valid destination
    if (!destination)
      return MOVE_EMPTY_DEST;
  }

  // previous environment can let go
  if ((move_flag & CAN_BE_DROPPED) && previous)
		if (!previous->test_remove(this_object()))
    	return MOVE_NO_DROP;
    
  // destination can accept the object
  if (move_flag & CAN_BE_GOTTEN)
  	if (!destination->test_add(this_object()))
    	return MOVE_NO_GET;

  // event_exit
  if (previous)
    event(previous, "exit", messout, destination);
  
  // efun that simulates the mudos inventory and
  // environment handling
  ::move(destination);

	// event_enter
  event(destination, "enter", messin, previous);
    
  return MOVE_OK;
}

void dest_me() 
{
  /*
  int i;
  object* obs;
  object ob;

  if (environment())
    event(environment(), "dest_me");

  // Destruct shadows of this object, Wonderflug 96
  obs = ({ });
  ob = shadow(this_object(), 0);

  while ( ob )
  {
    obs += ({ ob });
    ob = shadow(ob, 0);
  }

  for ( i = 0; i < sizeof(obs); i++ )
    if ( obs[i] )
      destruct(obs[i]);
      */

  ::destruct(this_object());
}

/* Do not I repeat do not mask this function.
 * is used when all else fails to dest the object...
 * If we find you masking this function... We will cut you up
 * into little bits... slowly
 */
nomask mixed dwep() 
{
  ::destruct(this_object());
  return "Destruct With Extreme Prejuce";
}

/* global/basic/move: */
/*
mixed *query_init_data() {
   return ({ "move", move_flag, "set_move_flag/p/" });
}
*/

mixed stats()
{
  return ({
    ({"Move Flag", move_flag, }),
    ({"Previous", prev, }),
     });
}
